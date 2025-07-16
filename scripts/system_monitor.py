#!/usr/bin/env python3
"""Comprehensive System Monitor for Notre Dame CRC

This script monitors GPU availability, CPU usage, memory usage, disk space,
and network status across the CRC cluster. It provides real-time visualization
of system resources with an extensible architecture for adding new collectors.

Usage
-----
$ module load conda               # 确保 Python3 可用
$ python scripts/gpu_monitor.py    # 默认 1 秒刷新一次

选项::

    -i, --interval INT  刷新间隔秒数 (默认 1)
    -m, --mode STR      监控模式: gpu, system, all (默认 all)

Dependencies
------------
* Python ≥ 3.8
* 可选: rich ‑ 若安装则显示彩色表格，否则退化为纯文本。
* 可选: psutil ‑ 若安装则提供更详细的系统信息。

"""
from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import time
from dataclasses import dataclass
from typing import List, Dict, Any, Optional
import getpass
import xml.etree.ElementTree as ET


@dataclass
class NodeStatus:
    """Represent GPU availability for a single node."""

    node: str
    free_gpus: int

    def as_row(self) -> List[str]:
        return [self.node, str(self.free_gpus)]


@dataclass
class SystemStatus:
    """Represent system resource status."""

    cpu_percent: float
    memory_percent: float
    disk_percent: float
    load_average: List[float]
    uptime: float
    processes: int

    def _format_uptime(self) -> str:
        """Return uptime as HH:MM:SS string."""
        total_seconds = int(self.uptime)
        minutes, seconds = divmod(total_seconds, 60)
        hours, minutes = divmod(minutes, 60)
        return f"{hours:02d}:{minutes:02d}:{seconds:02d}"

    def as_dict(self) -> Dict[str, Any]:
        return {
            "CPU": f"{self.cpu_percent:.1f}%",
            "Memory": f"{self.memory_percent:.1f}%",
            "Disk": f"{self.disk_percent:.1f}%",
            "Load": f"{self.load_average[0]:.2f}",
            "Uptime": self._format_uptime(),
            "Processes": str(self.processes)
        }


class CRCGPUCollector:
    """Collect GPU availability using `free_gpus.sh`."""

    CMD = ["free_gpus.sh", "@crc_gpu"]
    PATTERN = re.compile(r"Free GPU cards on (\S+) : (\d+)")

    def collect(self) -> List[NodeStatus]:
        try:
            completed = subprocess.run(
                self.CMD,
                check=True,
                capture_output=True,
                text=True,
            )
        except (subprocess.CalledProcessError, FileNotFoundError) as exc:
            print(f"[Error] Unable to execute {' '.join(self.CMD)}: {exc}")
            return []

        statuses: List[NodeStatus] = []
        for line in completed.stdout.splitlines():
            match = self.PATTERN.match(line)
            if match:
                statuses.append(
                    NodeStatus(node=match.group(
                        1), free_gpus=int(match.group(2)))
                )
        return statuses


class SystemResourceCollector:
    """Collect system resource information."""

    def __init__(self) -> None:
        self._psutil_available = False
        try:
            import psutil
            self._psutil = psutil
            self._psutil_available = True
        except ImportError:
            pass

    def collect(self) -> SystemStatus:
        if self._psutil_available:
            return self._collect_with_psutil()
        else:
            return self._collect_with_commands()

    def _collect_with_psutil(self) -> SystemStatus:
        """Collect system info using psutil (more accurate)."""
        import psutil

        cpu_percent = psutil.cpu_percent(interval=0.1)
        memory = psutil.virtual_memory()
        disk = psutil.disk_usage('/')
        load_avg = psutil.getloadavg()
        uptime = time.time() - psutil.boot_time()
        processes = len(psutil.pids())

        return SystemStatus(
            cpu_percent=cpu_percent,
            memory_percent=memory.percent,
            disk_percent=disk.percent,
            load_average=list(load_avg),
            uptime=uptime,
            processes=processes
        )

    def _collect_with_commands(self) -> SystemStatus:
        """Collect system info using system commands (fallback)."""
        try:
            # CPU usage
            cpu_cmd = "top -bn1 | grep 'Cpu(s)' | awk '{print $2}' | cut -d'%' -f1"
            cpu_result = subprocess.run(
                cpu_cmd, shell=True, capture_output=True, text=True)
            cpu_percent = float(cpu_result.stdout.strip() or "0")

            # Memory usage
            mem_cmd = "free | grep Mem | awk '{printf \"%.1f\", $3/$2 * 100.0}'"
            mem_result = subprocess.run(
                mem_cmd, shell=True, capture_output=True, text=True)
            memory_percent = float(mem_result.stdout.strip() or "0")

            # Disk usage
            disk_cmd = "df / | tail -1 | awk '{print $5}' | cut -d'%' -f1"
            disk_result = subprocess.run(
                disk_cmd, shell=True, capture_output=True, text=True)
            disk_percent = float(disk_result.stdout.strip() or "0")

            # Load average
            load_cmd = "uptime | awk -F'load average:' '{print $2}' | awk '{print $1}' | tr -d ','"
            load_result = subprocess.run(
                load_cmd, shell=True, capture_output=True, text=True)
            load_avg = float(load_result.stdout.strip() or "0")

            # Uptime
            uptime_cmd = "uptime -p | awk '{print $2}' | tr -d 's'"
            uptime_result = subprocess.run(
                uptime_cmd, shell=True, capture_output=True, text=True)
            uptime = float(uptime_result.stdout.strip() or "0")

            # Process count
            proc_cmd = "ps aux | wc -l"
            proc_result = subprocess.run(
                proc_cmd, shell=True, capture_output=True, text=True)
            processes = int(proc_result.stdout.strip() or "0")

            return SystemStatus(
                cpu_percent=cpu_percent,
                memory_percent=memory_percent,
                disk_percent=disk_percent,
                load_average=[load_avg, 0, 0],
                uptime=uptime,
                processes=processes
            )

        except Exception as exc:
            print(f"[Warning] System resource collection failed: {exc}")
            return SystemStatus(0, 0, 0, [0, 0, 0], 0, 0)


class NetworkCollector:
    """Collect network interface information."""

    def collect(self) -> Dict[str, Dict[str, str]]:
        try:
            # Get network interface stats
            cmd = "cat /proc/net/dev | grep -E '^(eth|en|lo)' | awk '{print $1, $2, $10}'"
            result = subprocess.run(
                cmd, shell=True, capture_output=True, text=True)

            interfaces = {}
            for line in result.stdout.splitlines():
                if line.strip():
                    parts = line.split()
                    if len(parts) >= 3:
                        iface = parts[0].rstrip(':')
                        rx_bytes = int(parts[1])
                        tx_bytes = int(parts[2])

                        # Convert to human readable
                        rx_mb = rx_bytes / (1024 * 1024)
                        tx_mb = tx_bytes / (1024 * 1024)

                        interfaces[iface] = {
                            "RX": f"{rx_mb:.1f}MB",
                            "TX": f"{tx_mb:.1f}MB"
                        }

            return interfaces
        except Exception as exc:
            print(f"[Warning] Network collection failed: {exc}")
            return {}


# ------------------ New: User Resource Collector -------------------


class MyResource:
    """Represent a GPU reservation owned by current user."""

    def __init__(self, job_id: str, node: str, gpu_count: int, state: str):
        self.job_id = job_id
        self.node = node
        self.gpu_count = gpu_count
        self.state = state  # r, qw, etc.

    def as_row(self) -> List[str]:
        return [self.job_id, self.node, str(self.gpu_count), self.state]


class MyResourceCollector:
    """Collect current user's GPU reservations by parsing qstat -xml."""

    def collect(self) -> List[MyResource]:
        user = getpass.getuser()
        try:
            proc = subprocess.run(
                ["qstat", "-xml", "-u", user], capture_output=True, text=True, check=True)
            root = ET.fromstring(proc.stdout)
        except subprocess.CalledProcessError as exc:
            print(f"[Warning] qstat failed: {exc.stderr}")
            return []
        except ET.ParseError as exc:
            print(f"[Warning] Failed to parse qstat XML: {exc}")
            return []

        resources: List[MyResource] = []
        for job_elem in root.findall(".//job_list"):
            job_id = job_elem.findtext("JB_job_number") or "?"
            state = job_elem.findtext("state") or "?"
            queue_name = job_elem.findtext("queue_name") or "pending"
            node = queue_name.split(
                ".")[0] if "." in queue_name else queue_name

            # Determine if this is a GPU job: queue_name starts with 'gpu@'
            if queue_name.startswith("gpu@"):
                # GPU count fallback to <slots> value if hard_request not present
                slots_text = job_elem.findtext("slots") or "1"
                try:
                    gpu_count = int(slots_text.strip())
                except ValueError:
                    gpu_count = 1
                resources.append(MyResource(job_id, node, gpu_count, state))

        return resources


# ------------------ New: Manager Status Collector -------------------


class EnhancedConsoleRenderer:
    """Render comprehensive system status to the terminal."""

    def __init__(self) -> None:
        try:
            from rich.table import Table
            from rich.console import Console
            from rich.layout import Layout
            from rich.panel import Panel
            from rich.text import Text

            self._Table = Table
            self._Console = Console
            self._Layout = Layout
            self._Panel = Panel
            self._Text = Text
            self._console = Console()
            self._rich_available = True
        except ImportError:
            self._rich_available = False

    def render(self, gpu_statuses: List[NodeStatus],
               system_status: SystemStatus,
               network_info: Dict[str, Dict[str, str]],
               my_resources: List[MyResource],
               running_count: int,
               pending_count: int,
               mode: str = "all") -> None:

        if self._rich_available:
            self._render_rich(gpu_statuses, system_status, network_info,
                              my_resources, running_count, pending_count, mode)
        else:
            self._render_text(gpu_statuses, system_status, network_info,
                              my_resources, running_count, pending_count, mode)

    def _render_rich(self, gpu_statuses: List[NodeStatus],
                     system_status: SystemStatus,
                     network_info: Dict[str, Dict[str, str]],
                     my_resources: List[MyResource],
                     running_count: int,
                     pending_count: int,
                     mode: str) -> None:
        """Render using rich library with enhanced layout."""
        from rich.layout import Layout
        from rich.panel import Panel
        from rich.table import Table
        from rich.text import Text

        # Clear screen
        self._console.clear()

        # Create layout
        layout = Layout()

        # Split layout into sections
        layout.split_column(
            Layout(name="upper"),
            Layout(name="lower")
        )

        # Create panels for different sections
        panels = []

        # Build combined System panel containing System, GPU, Network tables side-by-side
        from rich.columns import Columns
        from rich.table import Table

        # Build individual tables
        sys_table = Table(title="💻 System", show_header=True)
        sys_table.add_column("Metric", style="yellow")
        sys_table.add_column("Value", justify="right", style="white")
        for m, v in system_status.as_dict().items():
            sys_table.add_row(m, v)

        gpu_table = None
        if mode in ["all", "gpu"] and gpu_statuses:
            gpu_table = Table(title="🎮 GPU", show_header=True)
            gpu_table.add_column("Node", style="cyan")
            gpu_table.add_column("Free", justify="right", style="green")
            for status in gpu_statuses:
                gpu_table.add_row(status.node, str(status.free_gpus))

        # Removed network output per user request
        # My resources table (always include even if empty)
        my_table = Table(title="🔒 My GPU Reservations", show_header=True)
        my_table.add_column("JobID", style="magenta")
        my_table.add_column("Node", style="cyan")
        my_table.add_column("GPU", justify="right", style="green")
        my_table.add_column("State", style="white")
        for res in my_resources:
            my_table.add_row(*res.as_row())

        summary_text = f"Running: {running_count}  Pending: {pending_count}"
        my_table.add_row("-", "Summary", "-",
                         summary_text, style="bold yellow")

        # Build column tables list including My resource table
        col_tables = [tbl for tbl in [sys_table, gpu_table, my_table] if tbl]

        combined_panel = Panel(Columns(col_tables),
                               title="System Status (Sys + GPU + My) ")
        panels.append(combined_panel)

        # Add timestamp
        timestamp = Text(
            f"Last updated: {time.strftime('%Y-%m-%d %H:%M:%S')}", style="dim")
        panels.append(Panel(timestamp, title="Info"))

        # Display all panels
        for panel in panels:
            self._console.print(panel)
            self._console.print()  # spacing

    def _render_text(self, gpu_statuses: List[NodeStatus],
                     system_status: SystemStatus,
                     network_info: Dict[str, Dict[str, str]],
                     my_resources: List[MyResource],
                     running_count: int,
                     pending_count: int,
                     mode: str) -> None:
        """Fallback text rendering."""
        os.system("clear || cls")

        print("=" * 60)
        print(f"CRC System Monitor - {time.strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 60)

        if mode in ["all", "gpu"] and gpu_statuses:
            print("\n🎮 GPU Status:")
            print("-" * 30)
            print("Node\t\tFree GPUs")
            print("-" * 30)
            for status in gpu_statuses:
                print(f"{status.node}\t{status.free_gpus}")

        if mode in ["all", "system"]:
            print("\n💻 System Resources:")
            print("-" * 30)
            sys_dict = system_status.as_dict()
            for metric, value in sys_dict.items():
                print(f"{metric}: {value}")

        # Network output removed

        print("\n🔒 My GPU Reservations:")
        if my_resources:
            print("JobID\tNode\tGPU\tState")
            print("-" * 30)
            for res in my_resources:
                print("\t".join(res.as_row()))

        print(f"Summary - Running: {running_count}  Pending: {pending_count}")

        print("\n" + "=" * 60)


class ComprehensiveMonitor:
    """Main application orchestrating all collectors and renderers."""

    def __init__(self, interval: int = 1, mode: str = "all") -> None:
        self.interval = max(1, interval)
        self.mode = mode
        self.gpu_collector = CRCGPUCollector()
        self.system_collector = SystemResourceCollector()
        self.network_collector = NetworkCollector()
        self.my_collector = MyResourceCollector()
        self.renderer = EnhancedConsoleRenderer()

    def run(self) -> None:
        try:
            while True:
                # Collect data
                gpu_statuses = self.gpu_collector.collect() if self.mode in [
                    "all", "gpu"] else []
                system_status = self.system_collector.collect() if self.mode in [
                    "all", "system"] else SystemStatus(0, 0, 0, [0, 0, 0], 0, 0)
                network_info = self.network_collector.collect() if self.mode in [
                    "all", "network"] else {}

                my_resources = self.my_collector.collect()

                # Calculate running and pending counts directly
                running_count = sum(
                    r.gpu_count for r in my_resources if r.state == 'r')
                pending_count = sum(
                    r.gpu_count for r in my_resources if r.state == 'qw')

                # Render
                self.renderer.render(gpu_statuses, system_status, network_info,
                                     my_resources, running_count, pending_count, self.mode)

                time.sleep(self.interval)
        except KeyboardInterrupt:
            print("\n[Info] System monitor stopped by user.")


def parse_args(args: List[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="CRC Comprehensive System Monitor")
    parser.add_argument(
        "-i",
        "--interval",
        type=int,
        default=1,
        help="刷新间隔（秒），最小 1 秒，默认 1 秒",
    )
    parser.add_argument(
        "-m",
        "--mode",
        type=str,
        choices=["gpu", "system", "network", "all"],
        default="all",
        help="监控模式: gpu(仅GPU), system(仅系统), network(仅网络), all(全部)",
    )
    return parser.parse_args(args)


def main() -> None:
    ns = parse_args()
    monitor = ComprehensiveMonitor(interval=ns.interval, mode=ns.mode)
    monitor.run()


if __name__ == "__main__":
    main()
