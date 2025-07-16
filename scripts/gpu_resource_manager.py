#!/usr/bin/env python3
"""Smart GPU Resource Manager for Notre Dame CRC

This script provides intelligent GPU resource management with the following features:
1. Continuous monitoring of GPU availability
2. Dynamic acquisition of free GPUs
3. Multi-GPU collaboration support
4. Automatic program scaling across multiple GPUs

Usage
-----
$ module load conda
$ python scripts/gpu_resource_manager.py

Features
--------
- Monitor GPU availability in real-time
- Automatically acquire free GPUs
- Support multi-GPU program execution
- Dynamic scaling of running programs
- Resource usage tracking and optimization

"""
from __future__ import annotations

import argparse
import json
import os
import re
import signal
import subprocess
import sys
import threading
import time
from dataclasses import dataclass, asdict
import xml.etree.ElementTree as ET
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Set, Any
import logging
import getpass

# Configure logging
log_file = os.path.join(os.path.dirname(__file__), '..', 'gpu_manager.log')
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler(log_file),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)


@dataclass
class GPUNode:
    """Represent a GPU node with its resources."""

    node_name: str
    total_gpus: int
    free_gpus: int
    allocated_gpus: int = 0
    last_updated: Optional[datetime] = None

    def __post_init__(self):
        if self.last_updated is None:
            self.last_updated = datetime.now()

    def is_available(self) -> bool:
        """Check if node has free GPUs."""
        return self.free_gpus > 0

    def can_allocate(self, num_gpus: int) -> bool:
        """Check if node can allocate specified number of GPUs."""
        return self.free_gpus >= num_gpus

    def allocate_gpus(self, num_gpus: int) -> bool:
        """Allocate GPUs from this node."""
        if self.can_allocate(num_gpus):
            self.free_gpus -= num_gpus
            self.allocated_gpus += num_gpus
            self.last_updated = datetime.now()
            return True
        return False

    def release_gpus(self, num_gpus: int) -> bool:
        """Release GPUs back to this node."""
        if self.allocated_gpus >= num_gpus:
            self.allocated_gpus -= num_gpus
            self.free_gpus += num_gpus
            self.last_updated = datetime.now()
            return True
        return False


# Added job_id field to track real SGE jobs
@dataclass
class GPUAllocation:
    """Track GPU allocation for a specific task."""

    task_id: str
    node_name: str
    gpu_count: int
    allocated_at: datetime
    job_id: Optional[str] = None  # SGE job number
    program_pid: Optional[int] = None
    status: str = "allocated"  # allocated, running, completed, failed

    def to_dict(self) -> Dict[str, Any]:
        return {
            "task_id": self.task_id,
            "node_name": self.node_name,
            "gpu_count": self.gpu_count,
            "allocated_at": self.allocated_at.isoformat(),
            "job_id": self.job_id,
            "program_pid": self.program_pid,
            "status": self.status
        }


class GPUResourceCollector:
    """Collect GPU availability information."""

    CMD = ["free_gpus.sh", "@crc_gpu"]
    PATTERN = re.compile(r"Free GPU cards on (\S+) : (\d+)")

    def collect_gpu_status(self) -> Dict[str, GPUNode]:
        """Collect current GPU status from all nodes."""
        try:
            completed = subprocess.run(
                self.CMD,
                check=True,
                capture_output=True,
                text=True,
            )
        except (subprocess.CalledProcessError, FileNotFoundError) as exc:
            logger.error(f"Unable to execute {' '.join(self.CMD)}: {exc}")
            return {}

        nodes = {}
        for line in completed.stdout.splitlines():
            match = self.PATTERN.match(line)
            if match:
                node_name = match.group(1)
                free_gpus = int(match.group(2))
                # Assume each node has 4 GPUs (A10 nodes typically have 4 GPUs)
                total_gpus = 4
                nodes[node_name] = GPUNode(
                    node_name=node_name,
                    total_gpus=total_gpus,
                    free_gpus=free_gpus
                )

        return nodes


class GPUResourceManager:
    """Main GPU resource management system."""

    def __init__(self, config_file: str = "gpu_manager_config.json"):
        self.config_file = config_file
        self.nodes: Dict[str, GPUNode] = {}
        self.allocations: Dict[str, GPUAllocation] = {}
        self.collector = GPUResourceCollector()
        self.running = False
        self.monitor_thread = None
        self.lock = threading.Lock()
        self._display_status = False  # Flag for status display mode

        # Load configuration
        self.config = self.load_config()

        # Discover existing GPU jobs of current user
        self._sync_allocations_with_qstat(add_new=True)

        # Setup signal handlers
        signal.signal(signal.SIGINT, self.signal_handler)
        signal.signal(signal.SIGTERM, self.signal_handler)

    def load_config(self) -> Dict[str, Any]:
        """Load configuration from file."""
        default_config = {
            "monitor_interval": 5,  # seconds
            "max_gpus_per_task": 4,
            "auto_scale": True,
            "min_reserved_gpus": 0,
            "max_reserved_gpus": 2,
            "max_total_gpus": 8,
            "managed_job_prefixes": ["NDFS_"],
            "program_template": {
                "command": "python your_program.py",
                "args": ["--gpus", "{gpu_list}"],
                "env_vars": {"CUDA_VISIBLE_DEVICES": "{gpu_list}"}
            },
            "notification": {
                "enabled": False,
                "email": "zchen27@nd.edu"
            }
        }

        if os.path.exists(self.config_file):
            try:
                with open(self.config_file, 'r') as f:
                    config = json.load(f)
                    default_config.update(config)
            except Exception as e:
                logger.warning(f"Failed to load config: {e}")

        return default_config

    def save_config(self):
        """Save current configuration to file."""
        try:
            with open(self.config_file, 'w') as f:
                json.dump(self.config, f, indent=2)
        except Exception as e:
            logger.error(f"Failed to save config: {e}")

    def signal_handler(self, signum, frame):
        """Handle shutdown signals."""
        logger.info("Received shutdown signal, cleaning up...")
        self.stop()
        sys.exit(0)

    def start_monitoring(self):
        """Start GPU monitoring in background thread."""
        self.running = True
        self.monitor_thread = threading.Thread(
            target=self._monitor_loop, daemon=True)
        self.monitor_thread.start()
        logger.info("GPU monitoring started")

    def stop(self):
        """Stop GPU monitoring and cleanup."""
        self.running = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5)
        logger.info("GPU monitoring stopped")

    def _monitor_loop(self):
        """Main monitoring loop."""
        while self.running:
            try:
                # Sync SGE job status first to get the real state of the world
                self._sync_allocations_with_qstat()
                # Update our view of free GPUs based on cluster state
                self._update_gpu_status()
                # Act on the latest information to reserve more GPUs if needed
                self._manage_reserved_gpus()

                # If in status display mode, print current status
                if hasattr(self, '_display_status') and self._display_status:
                    self._display_current_status()

                time.sleep(self.config["monitor_interval"])
            except Exception as e:
                logger.error(f"Error in monitoring loop: {e}", exc_info=True)
                time.sleep(5)  # Longer sleep on error

    def _display_current_status(self):
        """Display current status in a formatted way for continuous monitoring."""
        # Ensure we have the latest data before displaying
        self._update_gpu_status()
        self._sync_allocations_with_qstat(add_new=True)

        # Clear screen for better readability
        os.system('clear')

        print("\n" + "="*80)
        print(
            f"GPU Resource Manager - Live Status  |  User: {getpass.getuser()}")
        print(f"Time: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("="*80)

        status = self.get_status()
        summary = status["summary"]

        # Summary section
        print("\n📊 RESOURCE SUMMARY:")
        print("-" * 50)
        print(f"🖥️  Total Nodes:      {summary['total_nodes']}")
        print(f"🎯 Total GPUs:       {summary['total_gpus']}")
        print(f"🟢 Free GPUs:        {summary['free_gpus']}")
        print(f"🔵 My Running GPUs:  {summary.get('running', 0)}")
        print(f"🟡 My Pending GPUs:  {summary.get('pending', 0)}")
        print(f"📋 Active Tasks:     {summary['active_tasks']}")

        # Node details
        if status["nodes"]:
            print("\n🏢 CLUSTER NODES:")
            print("-" * 50)
            for node_name, node_data in sorted(status["nodes"].items()):
                free = node_data['free_gpus']
                total = node_data['total_gpus']
                allocated = node_data['allocated_gpus']
                status_icon = "🟢" if free > 0 else "🔴"
                print(
                    f"{status_icon} {node_name:<20} {free}/{total} free ({allocated} allocated)")
        else:
            print("\n🏢 CLUSTER NODES: Not available")

        # My GPU jobs
        if status["allocations"]:
            print("\n🚀 MY GPU JOBS:")
            print("-" * 50)
            for task_id, alloc_data in sorted(status["allocations"].items()):
                job_info = f"Job {alloc_data['job_id']} ({alloc_data['task_id']})"
                status_icon = "🔵" if alloc_data['status'] == 'running' else "🟡"
                node_display = alloc_data['node_name'] if alloc_data['node_name'] != 'pending' else 'waiting...'
                print(f"{status_icon} {job_info:<40}")
                print(
                    f"     └─ {alloc_data['gpu_count']} GPU(s) on {node_display} [{alloc_data['status']}]")
        else:
            print("\n🚀 MY GPU JOBS: None")

        print("\n" + "="*80)
        print("Press Ctrl+C to exit")
        print("="*80)

    def _update_gpu_status(self):
        """Update GPU status from cluster."""
        with self.lock:
            new_nodes = self.collector.collect_gpu_status()

            # Update existing nodes
            for node_name, new_node in new_nodes.items():
                # Compute allocated GPUs from total - free for accurate stats
                new_node.allocated_gpus = new_node.total_gpus - new_node.free_gpus
                if node_name in self.nodes:
                    # Preserve allocation count only if larger (historical tracking)
                    old_node = self.nodes[node_name]
                    if old_node.allocated_gpus > new_node.allocated_gpus:
                        new_node.allocated_gpus = old_node.allocated_gpus
                self.nodes[node_name] = new_node

            # Remove nodes that are no longer available
            removed_nodes = set(self.nodes.keys()) - set(new_nodes.keys())
            for node_name in removed_nodes:
                del self.nodes[node_name]

    def _manage_reserved_gpus(self):
        """Proactively reserve free GPUs up to the configured limit."""
        with self.lock:
            # 1. Count current automatic reservations
            prefixes = self.config.get("managed_job_prefixes", ["NDFS_"])
            auto_prefix = f"{prefixes[0]}auto_"
            auto_reservations = [
                alloc for alloc in self.allocations.values()
                if alloc.task_id.startswith(auto_prefix)
                and alloc.status in ("running", "pending")
            ]
            num_auto_gpus = sum(alloc.gpu_count for alloc in auto_reservations)

            # 2. Count total managed GPUs
            total_managed_gpus = sum(
                alloc.gpu_count for alloc in self.allocations.values()
                if alloc.status in ("running", "pending")
            )

            # 3. Get configuration limits
            max_auto_gpus = self.config.get("max_reserved_gpus", 2)
            max_total_gpus = self.config.get("max_total_gpus", 8)

            # 4. Calculate how many more auto GPUs we can request
            needed_gpus = max_auto_gpus - num_auto_gpus
            if needed_gpus <= 0:
                return  # We have enough or too many auto-reserved GPUs

            gpus_can_add_before_total_limit = max_total_gpus - total_managed_gpus
            gpus_to_request = min(needed_gpus, gpus_can_add_before_total_limit)

            if gpus_to_request <= 0:
                return

            # 5. Check total free GPUs available in the cluster
            total_free_gpus = sum(
                node.free_gpus for node in self.nodes.values())
            if total_free_gpus == 0:
                return  # No GPUs to reserve

            # 6. Request GPUs one by one up to the calculated need
            num_to_acquire = min(gpus_to_request, total_free_gpus)
            if num_to_acquire > 0:
                logger.info(
                    f"Attempting to acquire {num_to_acquire} new GPU(s) to meet reservation target.")
                for _ in range(num_to_acquire):
                    task_id = f"{auto_prefix}{int(time.time() * 1000)}"
                    allocation = self.allocate_gpus(
                        task_id=task_id, num_gpus=1, manual=False)
                    if not allocation:
                        logger.warning(
                            f"Failed to submit reservation job for {task_id}. Will retry next cycle.")
                        break  # Stop trying if one submission fails

    def start_program(self, task_id: str, program_config: Dict[str, Any]) -> bool:
        """Placeholder for program start logic."""
        logger.info(
            f"Starting program for task {task_id} with config {program_config}")
        # This would start the user's actual computation on the allocated resources.
        return True

    def allocate_gpus(self, task_id: str, num_gpus: int = 1, manual: bool = False) -> Optional[GPUAllocation]:
        """Allocate GPUs by submitting a real SGE reservation job (qsub sleep)."""
        with self.lock:
            # Enforce limits
            reserved_now = sum(a.gpu_count for a in self.allocations.values(
            ) if a.status in ("running", "pending"))
            max_total = self.config.get("max_total_gpus", 8)
            if reserved_now + num_gpus > max_total:
                logger.warning("Request exceeds max_total_gpus limit")
                return None

            # Submit job to SGE, let scheduler handle placement
            submit_cmd = [
                "qsub", "-q", "gpu",
                "-l", f"gpu_card={num_gpus}",
                "-N", task_id,
                "-b", "y",
                "-now", "no",  # Do not wait, fail immediately if resources are unavailable
                "sleep", "31536000"  # ~1 year, acts as resource holder
            ]
            try:
                proc = subprocess.run(
                    submit_cmd,
                    capture_output=True,
                    text=True,
                    check=True,
                )
                # qsub output: Your job 123456 ("task_id") has been submitted
                match = re.search(r"Your job (\d+)", proc.stdout)
                if not match:
                    logger.error(
                        f"Could not parse job ID from qsub output: {proc.stdout}")
                    return None
                job_id = match.group(1)

                allocation = GPUAllocation(
                    task_id=task_id,
                    node_name="pending",
                    gpu_count=num_gpus,
                    allocated_at=datetime.now(),
                    job_id=job_id,
                    status="pending",
                )
                self.allocations[task_id] = allocation
                logger.info(
                    f"Submitted reservation job {job_id} for task {task_id}")
                return allocation
            except subprocess.CalledProcessError as exc:
                logger.error(f"Failed to submit reservation job: {exc.stderr}")
                return None

    def release_gpus(self, task_id: str) -> bool:
        """Release GPUs by deleting the SGE job."""
        with self.lock:
            alloc = self.allocations.get(task_id)
            if not alloc:
                logger.warning(f"Task {task_id} not found in allocations")
                return False

            if alloc.job_id:
                try:
                    subprocess.run(["qdel", alloc.job_id], check=True)
                    logger.info(f"qdel {alloc.job_id} for task {task_id}")
                except subprocess.CalledProcessError as exc:
                    logger.error(
                        f"Failed to qdel job {alloc.job_id}: {exc.stderr}")
                    return False

            # Remove allocation tracking
            self.allocations.pop(task_id, None)
            return True

    def _sync_allocations_with_qstat(self, add_new: bool = False):
        """Synchronize allocation statuses with qstat -xml output."""
        try:
            # Only get jobs for current user to avoid parsing huge XML with all users' jobs
            current_user = getpass.getuser()
            proc = subprocess.run(
                ["qstat", "-u", current_user, "-xml"],
                capture_output=True, text=True, check=True)
            root = ET.fromstring(proc.stdout)
        except subprocess.CalledProcessError as exc:
            # This can happen if there are no jobs, which is not an error
            if "Following jobs do not exist" in exc.stderr or not exc.stderr:
                # Clear all non-pending jobs if qstat is empty
                with self.lock:
                    for alloc in list(self.allocations.values()):
                        if alloc.status != "pending":
                            alloc.status = "completed"
                return
            logger.error(f"qstat failed: {exc.stderr}")
            return
        except ET.ParseError as exc:
            logger.error(f"Failed to parse qstat XML: {exc}")
            return

        running_jobs = {}
        for job_elem in root.findall(".//job_list"):
            job_id = job_elem.findtext("JB_job_number")
            job_name = job_elem.findtext("JB_name")
            state = job_elem.findtext("state")  # r = running, qw = pending
            queue_name = job_elem.findtext("queue_name") or "unknown"
            # exec_host like "gpu@qa-a10-024" appears in queue_name
            node = queue_name if "@" in queue_name else "pending"
            slots_text = job_elem.findtext("slots") or "1"
            try:
                slots = int(slots_text.strip())
            except ValueError:
                slots = 1
            running_jobs[job_id] = {
                "job_name": job_name,
                "state": state,
                "node": node,
                "slots": slots,
            }

        with self.lock:
            # Update existing allocations & mark seen
            seen_ids = set()
            # Use a copy of values for safe iteration while modifying dict
            for alloc in list(self.allocations.values()):
                if alloc.job_id and alloc.job_id in running_jobs:
                    job_info = running_jobs[alloc.job_id]
                    alloc.status = "running" if job_info["state"] == "r" else "pending"
                    # Update node name only if it was pending before
                    if alloc.node_name == "pending" and job_info["node"] != "pending":
                        alloc.node_name = job_info["node"]
                    alloc.gpu_count = job_info["slots"]
                    seen_ids.add(alloc.job_id)
                elif alloc.job_id:
                    # Job disappeared => completed or deleted
                    alloc.status = "completed"

            # Optionally add new allocations for jobs not yet tracked
            prefixes = self.config.get("managed_job_prefixes", [])
            if add_new:
                for job_id, info in running_jobs.items():
                    if job_id in seen_ids:
                        continue
                    if not info["node"].startswith("gpu@") and info["node"] != "pending":
                        continue
                    # Filter by prefix
                    if prefixes and not any(info["job_name"].startswith(p) for p in prefixes):
                        continue
                    task_id = f"{info['job_name']}_{job_id}"
                    self.allocations[task_id] = GPUAllocation(
                        task_id=task_id,
                        node_name=info["node"],
                        gpu_count=info["slots"],
                        allocated_at=datetime.now(),
                        job_id=job_id,
                        status="running" if info["state"] == "r" else "pending",
                    )

            # Remove completed allocations
            for tid in list(self.allocations.keys()):
                if self.allocations.get(tid) and self.allocations[tid].status == "completed":
                    logger.info(
                        f"Removing completed/deleted job {self.allocations[tid].job_id} from tracking.")
                    self.allocations.pop(tid, None)

    def use_job(self, task_id: str):
        """Deletes a placeholder job and immediately requests an interactive session."""
        self._sync_allocations_with_qstat(add_new=True)  # Get latest info
        with self.lock:
            # Find allocation by job ID or task ID
            alloc = next((a for a in self.allocations.values()
                         if a.job_id == task_id or a.task_id == task_id), None)
            if not alloc:
                print(
                    f"Error: Task or Job ID '{task_id}' not found in managed jobs.")
                self.print_status()
                return

            if alloc.status != "running":
                print(
                    f"Error: Job '{alloc.job_id}' is not in a 'running' state. Current state: {alloc.status}")
                return

            job_id = alloc.job_id
            queue_name = alloc.node_name  # Should be the full queue name now
            gpu_count = alloc.gpu_count

        # Drop the lock before running subprocesses
        print(
            f"Attempting to convert placeholder job {job_id} to an interactive session...")

        try:
            print(f"[Step 1/2] Releasing placeholder job: qdel {job_id}")
            subprocess.run(["qdel", str(job_id)],
                           capture_output=True, text=True, check=True)
            print(f"Placeholder job {job_id} released successfully.")
        except subprocess.CalledProcessError as e:
            print(
                f"Error: Failed to delete job {job_id}. It might have already finished.")
            print(f"SGE Error: {e.stderr}")
            print("Aborting.")
            return

        print(f"[Step 2/2] Requesting interactive session on {queue_name}...")
        print("-" * 50)

        qrsh_args = ["qrsh", "-q", queue_name, "-l", f"gpu_card={gpu_count}"]
        os.execvp("qrsh", qrsh_args)

    def get_status(self) -> Dict[str, Any]:
        """Get current status of all nodes and allocations."""
        with self.lock:
            return {
                "nodes": {name: asdict(node) for name, node in self.nodes.items()},
                "allocations": {task_id: alloc.to_dict() for task_id, alloc in self.allocations.items()},
                "summary": {
                    "total_nodes": len(self.nodes),
                    "total_gpus": sum(node.total_gpus for node in self.nodes.values()),
                    "free_gpus": sum(node.free_gpus for node in self.nodes.values()),
                    "running": sum(a.gpu_count for a in self.allocations.values() if a.status == "running"),
                    "pending": sum(a.gpu_count for a in self.allocations.values() if a.status == "pending"),
                    "allocated_gpus": sum(a.gpu_count for a in self.allocations.values() if a.status in ("running", "pending")),
                    "active_tasks": len([a for a in self.allocations.values() if a.status in ("running", "pending")])
                }
            }

    def print_status(self):
        """Print current status in a formatted way."""
        # Always get the latest data before printing
        self._update_gpu_status()
        self._sync_allocations_with_qstat(add_new=True)

        status = self.get_status()

        print("\n" + "="*60)
        print("GPU Resource Manager Status")
        print("="*60)

        # Summary
        summary = status["summary"]
        print(f"Total Nodes: {summary['total_nodes']}")
        print(f"Total GPUs: {summary['total_gpus']}")
        print(f"Free GPUs: {summary['free_gpus']}")
        print(
            f"My GPUs - Running: {summary.get('running', 0)}, Pending: {summary.get('pending', 0)}")
        print(f"Active Tasks: {summary['active_tasks']}")

        # Node details
        if status["nodes"]:
            print("\nNode Details:")
            print("-" * 40)
            for node_name, node_data in sorted(status["nodes"].items()):
                print(f"{node_name}: {node_data['free_gpus']}/{node_data['total_gpus']} free "
                      f"({node_data['allocated_gpus']} allocated)")
        else:
            print("\nNode Details: Not available")

        # Allocation details (only user GPU jobs)
        if status["allocations"]:
            print("\nMy GPU Jobs:")
            print("-" * 40)
            for task_id, alloc_data in sorted(status["allocations"].items()):
                job_info = f"{alloc_data['job_id']} ({alloc_data['task_id']})"
                print(f"{job_info:<30} -> {alloc_data['gpu_count']} GPU(s) on {alloc_data['node_name']} "
                      f"[{alloc_data['status']}]")

        print("="*60)


def main():
    """Main function."""
    parser = argparse.ArgumentParser(description="Smart GPU Resource Manager")
    parser.add_argument(
        "--config", default="gpu_manager_config.json", help="Configuration file")
    parser.add_argument("--monitor-only", action="store_true",
                        help="Only monitor, don't manage")
    parser.add_argument("--allocate", help="Allocate GPUs for a task (manual)")
    parser.add_argument("--gpus", type=int, default=1,
                        help="Number of GPUs to allocate with --allocate")
    parser.add_argument("--release", help="Release GPUs for a task")
    parser.add_argument("--start-program", help="Start program for a task")
    parser.add_argument("--use", metavar="JOB_ID_OR_TASK_ID",
                        help="Convert a running placeholder job to an interactive session")
    parser.add_argument("--status", action="store_true",
                        help="Show current status once and exit")
    parser.add_argument("--daemon", action="store_true",
                        help="Run in daemon mode (no status display)")

    args = parser.parse_args()

    # Initialize manager
    manager = GPUResourceManager(args.config)

    if args.use:
        manager.use_job(args.use)
        return

    if args.status:
        # Just show status and exit
        manager.print_status()
        return

    if args.allocate:
        # Manual allocation
        task_name = f"{manager.config.get('managed_job_prefixes', ['NDFS_'])[0]}{args.allocate}"
        allocation = manager.allocate_gpus(
            task_name, num_gpus=args.gpus, manual=True)
        if allocation:
            print(
                f"Submitted reservation job for task {task_name} (Job ID: {allocation.job_id})")
        else:
            print("Failed to allocate GPUs")
        return

    if args.release:
        # Release GPUs for a task
        if manager.release_gpus(args.release):
            print(f"Released GPUs for task {args.release}")
        else:
            print("Failed to release GPUs")
        return

    if args.start_program:
        # Start a program
        if manager.start_program(args.start_program, {}):
            print(f"Started program for task {args.start_program}")
        else:
            print("Failed to start program")
        return

    # Determine the running mode
    # If no specific arguments are provided, default to live status display mode
    if not any([args.monitor_only, args.daemon]):
        # Default: Live status display mode
        manager._display_status = True
        print("Starting GPU Resource Manager in live status display mode...")
        print("Press Ctrl+C to exit")
        time.sleep(2)  # Brief pause to show the message
    elif args.daemon:
        # Daemon mode: management only, no status display
        manager._display_status = False
        print("Starting GPU Resource Manager in daemon mode...")
    elif args.monitor_only:
        # Monitor only mode with periodic status display
        manager._display_status = True
        print("Starting GPU Resource Manager in monitor-only mode...")

    # Start monitoring
    try:
        manager.start_monitoring()

        # Main loop
        while True:
            time.sleep(10)
            # The background thread handles the status display
            # Main thread just keeps the program alive

    except KeyboardInterrupt:
        logger.info("Shutting down...")
        manager.stop()


if __name__ == "__main__":
    main()
