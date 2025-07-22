#!/usr/bin/env python3
"""
Comprehensive GPU Resource Manager for Notre Dame CRC

A unified management system integrating GPU resource management 
with comprehensive system monitoring capabilities.
"""

import json
import os
import re
import subprocess
import threading
import time
import xml.etree.ElementTree as ET
from dataclasses import dataclass
from datetime import datetime
from typing import Dict, List, Optional
import getpass
import argparse
import signal
import sys


@dataclass
class GPUNode:
    """Simple GPU node representation"""
    name: str
    total_gpus: int
    free_gpus: int


@dataclass
class GPUJob:
    """Enhanced GPU job representation with detailed information"""
    job_id: str
    task_name: str
    gpu_count: int
    cpu_count: int  # 新增CPU数量
    status: str  # 'running', 'pending'
    node: str
    submit_time: str = ""
    priority: str = ""
    queue_position: int = 0
    resource_requests: Dict[str, str] = None

    def __post_init__(self):
        if self.resource_requests is None:
            self.resource_requests = {}

    def get_state_description(self) -> str:
        """Get human readable state description"""
        state_map = {
            "r": "运行中",
            "qw": "排队中",
            "t": "传输中",
            "s": "暂停",
            "h": "保持",
            "E": "错误"
        }
        return state_map.get(self.status, f"未知({self.status})")

    def format_resources(self) -> str:
        """Format resource requests for display"""
        parts = []

        # 添加GPU信息
        if self.gpu_count > 0:
            parts.append(f"{self.gpu_count}GPU")

        # 添加CPU信息
        if self.cpu_count > 0:
            parts.append(f"{self.cpu_count}CPU")

        # 添加其他资源信息
        if self.resource_requests:
            for resource, value in self.resource_requests.items():
                if resource == "mem_free":
                    if value.endswith("G"):
                        parts.append(f"{value}-RAM")
                    else:
                        parts.append(f"{value}")
                elif resource == "h_vmem":
                    parts.append(f"{value}")
                elif resource not in ["gpu_card"]:  # 跳过已处理的gpu_card
                    parts.append(f"{value}")

        return ",".join(parts)[:20] if parts else f"{self.gpu_count}GPU,{self.cpu_count}CPU"


@dataclass
class SystemStatus:
    """System resource status"""
    cpu_percent: float
    memory_percent: float
    disk_percent: float
    load_average: List[float]
    uptime: float
    processes: int

    def format_uptime(self) -> str:
        """Format uptime as HH:MM:SS"""
        total_seconds = int(self.uptime)
        minutes, seconds = divmod(total_seconds, 60)
        hours, minutes = divmod(minutes, 60)
        return f"{hours:02d}:{minutes:02d}:{seconds:02d}"


class GPUMonitor:
    """基础监控层 - 负责收集GPU状态"""

    def __init__(self):
        self.cmd = ["free_gpus.sh", "@crc_gpu"]
        self.pattern = re.compile(r"Free GPU cards on (\S+) : (\d+)")

    def get_gpu_nodes(self) -> Dict[str, GPUNode]:
        """获取所有GPU节点状态"""
        try:
            result = subprocess.run(
                self.cmd, capture_output=True, text=True, check=True
            )
            nodes = {}
            for line in result.stdout.splitlines():
                match = self.pattern.match(line)
                if match:
                    node_name = match.group(1)
                    free_gpus = int(match.group(2))
                    nodes[node_name] = GPUNode(
                        name=node_name,
                        total_gpus=4,  # A10 nodes have 4 GPUs
                        free_gpus=free_gpus
                    )
            return nodes
        except (subprocess.CalledProcessError, FileNotFoundError):
            return {}

    def get_my_jobs(self) -> List[GPUJob]:
        """获取当前用户的GPU作业，包含详细信息"""
        try:
            user = getpass.getuser()
            result = subprocess.run(
                ["qstat", "-u", user, "-xml"],
                capture_output=True, text=True, check=True
            )

            jobs = []
            root = ET.fromstring(result.stdout)

            # Get queue positions for pending jobs
            queue_positions = self._get_queue_positions()

            for job_elem in root.findall(".//job_list"):
                job_id = job_elem.findtext("JB_job_number", "")
                task_name = job_elem.findtext("JB_name", "")
                state = job_elem.findtext("state", "")
                queue_name = job_elem.findtext("queue_name", "pending")
                slots_text = job_elem.findtext("slots", "1")
                submit_time = job_elem.findtext("JB_submission_time", "")
                priority = job_elem.findtext("JAT_prio", "")

                # Check if it's a GPU job by queue name or resource request
                is_gpu_job = False
                gpu_count = 0
                cpu_count = int(slots_text) if slots_text.isdigit() else 1

                # Method 1: For running jobs, check queue name
                if queue_name and "gpu@" in queue_name:
                    is_gpu_job = True
                    # For running GPU jobs, get GPU count from resource requests
                    resource_requests = self._get_job_resources(job_id)
                    if "gpu_card" in resource_requests:
                        gpu_count = int(resource_requests["gpu_card"])
                    else:
                        # 默认假设GPU作业至少有1个GPU
                        gpu_count = 1

                # Method 2: For pending jobs, check hard_resource_list
                if not is_gpu_job:
                    hard_resource = job_elem.find(".//hard_resource_list")
                    if hard_resource is not None:
                        resource_text = hard_resource.text or ""
                        if "gpu_card" in resource_text:
                            is_gpu_job = True
                            # Extract GPU count from resource request
                            gpu_match = re.search(
                                r"gpu_card=(\d+)", resource_text)
                            gpu_count = int(gpu_match.group(1)
                                            ) if gpu_match else 1

                # Only include GPU jobs
                if not is_gpu_job:
                    continue

                status = "running" if state == "r" else "pending"
                node = queue_name if "@" in queue_name else "pending"

                # Get queue position and resource details
                queue_position = queue_positions.get(
                    job_id, 0) if status == "pending" else 0
                resource_requests = self._get_job_resources(job_id)

                jobs.append(GPUJob(
                    job_id=job_id,
                    task_name=task_name,
                    gpu_count=gpu_count,
                    cpu_count=cpu_count,
                    status=status,
                    node=node,
                    submit_time=submit_time,
                    priority=priority,
                    queue_position=queue_position,
                    resource_requests=resource_requests
                ))

            return jobs
        except (subprocess.CalledProcessError, ET.ParseError):
            return []

    def _get_queue_positions(self) -> Dict[str, int]:
        """Calculate queue positions for all pending jobs"""
        try:
            result = subprocess.run(
                ["qstat", "-xml"], capture_output=True, text=True, check=True)
            root = ET.fromstring(result.stdout)
        except (subprocess.CalledProcessError, ET.ParseError):
            return {}

        # Extract pending jobs with priorities
        pending_jobs = []
        for job_elem in root.findall(".//job_list[@state='pending']"):
            job_id = job_elem.findtext("JB_job_number")
            priority_text = job_elem.findtext("JAT_prio") or "0"
            try:
                priority = float(priority_text)
            except ValueError:
                priority = 0.0

            if job_id:
                pending_jobs.append((job_id, priority))

        # Sort by priority (higher priority = lower position in queue)
        pending_jobs.sort(key=lambda x: x[1], reverse=True)

        # Create position mapping
        positions = {}
        for i, (job_id, _) in enumerate(pending_jobs, 1):
            positions[job_id] = i

        return positions

    def _get_job_resources(self, job_id: str) -> Dict[str, str]:
        """Get detailed resource requests for a specific job"""
        try:
            result = subprocess.run(
                ["qstat", "-j", job_id], capture_output=True, text=True, check=True)
            output = result.stdout
        except subprocess.CalledProcessError:
            return {}

        resources = {}
        for line in output.split('\n'):
            if line.startswith('hard_resource_list:'):
                resource_part = line.split(':', 1)[1].strip()
                for item in resource_part.split(','):
                    if '=' in item:
                        key, value = item.split('=', 1)
                        resources[key.strip()] = value.strip()
                break

        return resources


class SystemMonitor:
    """系统资源监控层"""

    def get_system_status(self) -> SystemStatus:
        """获取系统资源状态"""
        try:
            # CPU usage
            cpu_cmd = "grep 'cpu ' /proc/stat | awk '{usage=($2+$4)*100/($2+$3+$4+$5)} END {print usage}'"
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
            load_cmd = "cat /proc/loadavg | awk '{print $1, $2, $3}'"
            load_result = subprocess.run(
                load_cmd, shell=True, capture_output=True, text=True)
            load_values = [float(x)
                           for x in load_result.stdout.strip().split()]

            # Uptime
            uptime_cmd = "cat /proc/uptime | awk '{print $1}'"
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
                load_average=load_values,
                uptime=uptime,
                processes=processes
            )

        except Exception:
            return SystemStatus(0, 0, 0, [0, 0, 0], 0, 0)


class GPUJobManager:
    """作业管理层 - 负责GPU申请和释放"""

    def __init__(self, config: Dict):
        self.config = config
        self.job_prefix = config.get("job_prefix", "NDFS_")

    def request_gpu(self, task_name: str) -> Optional[str]:
        """申请单个GPU"""
        job_name = f"{self.job_prefix}{task_name}_{int(time.time())}"

        cmd = [
            "qsub", "-q", "gpu",
            "-l", "gpu_card=1",
            "-N", job_name,
            "-b", "y",
            "sleep", "86400"  # 24 hours
        ]

        try:
            result = subprocess.run(
                cmd, capture_output=True, text=True, check=True
            )
            # Extract job ID
            match = re.search(r"Your job (\d+)", result.stdout)
            if match:
                job_id = match.group(1)
                print(f"✅ 申请成功: Job {job_id} ({job_name})")
                return job_id
        except subprocess.CalledProcessError as e:
            print(f"❌ 申请失败: {e.stderr.strip()}")

        return None

    def release_gpu(self, job_id: str) -> bool:
        """释放GPU"""
        try:
            subprocess.run(["qdel", job_id], check=True)
            print(f"✅ 释放成功: Job {job_id}")
            return True
        except subprocess.CalledProcessError as e:
            print(f"❌ 释放失败: {e.stderr.strip()}")
            return False

    def auto_manage(self, nodes: Dict[str, GPUNode], jobs: List[GPUJob]) -> None:
        """自动管理GPU资源"""
        if not self.config.get("auto_manage", False):
            return

        # Count managed jobs
        managed_jobs = [
            job for job in jobs if job.task_name.startswith(self.job_prefix)]
        running_gpus = sum(
            job.gpu_count for job in managed_jobs if job.status == "running")
        pending_gpus = sum(
            job.gpu_count for job in managed_jobs if job.status == "pending")
        total_managed = running_gpus + pending_gpus

        min_required = self.config.get("min_reserved_gpus", 1)
        max_allowed = self.config.get("max_reserved_gpus", 2)
        max_total = self.config.get("max_total_gpus", 4)

        # Calculate free GPUs in cluster
        total_free = sum(node.free_gpus for node in nodes.values())

        # Auto acquire if below minimum
        if total_managed < min_required and total_managed < max_total and total_free > 0:
            needed = min(min_required - total_managed,
                         max_total - total_managed)
            print(
                f"🤖 自动管理: 当前{total_managed}个GPU，需要至少{min_required}个，申请{needed}个")
            for i in range(needed):
                self.request_gpu(f"auto_{i+1}")


class ComprehensiveStatusDisplay:
    """综合状态显示层 - 负责全面的状态信息展示"""

    @staticmethod
    def show_status(nodes: Dict[str, GPUNode], jobs: List[GPUJob],
                    system_status: SystemStatus) -> None:
        """显示综合状态信息"""
        os.system('clear')

        print("\n" + "="*80)
        print(
            f"🖥️  综合GPU资源管理器 - {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"👤 用户: {getpass.getuser()}")
        print("="*80)

        # 集群GPU状态和系统资源状态横向排列
        total_gpus = sum(node.total_gpus for node in nodes.values())
        free_gpus = sum(node.free_gpus for node in nodes.values())

        # 计算总CPU数量
        total_running_cpus = sum(
            job.cpu_count for job in jobs if job.status == "running")
        total_pending_cpus = sum(
            job.cpu_count for job in jobs if job.status == "pending")

        print(f"\n📊 集群状态                        💻 系统资源")
        print(
            f"   总GPU数: {total_gpus:<12}           CPU使用率: {system_status.cpu_percent:.1f}%")
        print(
            f"   空闲GPU: {free_gpus:<12}           内存使用率: {system_status.memory_percent:.1f}%")
        print(
            f"   GPU使用率: {((total_gpus - free_gpus) / total_gpus * 100):.1f}%{'':6}     磁盘使用率: {system_status.disk_percent:.1f}%")
        print(
            f"   我的运行CPU: {total_running_cpus:<8}        系统负载: {system_status.load_average[0]:.2f}")
        print(
            f"   我的排队CPU: {total_pending_cpus:<8}        运行时间: {system_status.format_uptime()}")
        print(f"{'':35}        进程数: {system_status.processes}")

        # GPU节点详情
        if nodes:
            print(f"\n🏢 GPU节点详情:")
            for node in sorted(nodes.values(), key=lambda x: x.name):
                status_icon = "🟢" if node.free_gpus > 0 else "🔴"
                usage_percent = ((node.total_gpus - node.free_gpus) /
                                 node.total_gpus * 100) if node.total_gpus > 0 else 0
                print(
                    f"   {status_icon} {node.name}: {node.free_gpus}/{node.total_gpus} 空闲 ({usage_percent:.0f}%使用)")

        # 我的GPU作业详情
        if jobs:
            running_jobs = [job for job in jobs if job.status == "running"]
            pending_jobs = [job for job in jobs if job.status == "pending"]

            print(f"\n🚀 我的GPU作业详情:")
            print(f"   运行中: {len(running_jobs)} 个作业")
            print(f"   排队中: {len(pending_jobs)} 个作业")

            if running_jobs:
                print(f"\n   📋 运行中的作业:")
                for job in running_jobs:
                    resources_info = job.format_resources()
                    node_display = job.node.replace(
                        "gpu@", "") if "gpu@" in job.node else job.node
                    priority_display = f"{float(job.priority):.3f}" if job.priority and job.priority.replace(
                        '.', '').isdigit() else "N/A"
                    print(f"   🔵 Job {job.job_id}: {job.task_name}")
                    print(
                        f"        └─ {resources_info} @ {node_display} (优先级: {priority_display})")

            if pending_jobs:
                print(f"\n   ⏳ 排队中的作业:")
                for job in pending_jobs:
                    resources_info = job.format_resources()
                    position_info = f"队列位置: #{job.queue_position}" if job.queue_position > 0 else "队列位置: 未知"
                    priority_display = f"{float(job.priority):.3f}" if job.priority and job.priority.replace(
                        '.', '').isdigit() else "N/A"
                    print(f"   🟡 Job {job.job_id}: {job.task_name}")
                    print(
                        f"        └─ {resources_info} ({position_info}, 优先级: {priority_display})")

            # 统计信息
            total_running_gpus = sum(job.gpu_count for job in running_jobs)
            total_pending_gpus = sum(job.gpu_count for job in pending_jobs)
            print(f"\n   📈 统计信息:")
            print(
                f"      运行中GPU: {total_running_gpus} 个, CPU: {total_running_cpus} 个")
            print(
                f"      排队中GPU: {total_pending_gpus} 个, CPU: {total_pending_cpus} 个")
            print(
                f"      总计GPU: {total_running_gpus + total_pending_gpus} 个, CPU: {total_running_cpus + total_pending_cpus} 个")
        else:
            print(f"\n🚀 我的GPU作业: 无")

        print("\n" + "="*80)
        print("🔧 管理功能: 按 Ctrl+C 退出")
        print("💡 提示: 使用 --help 查看更多选项")
        print("="*80)


class ComprehensiveGPUManager:
    """综合GPU资源管理器主类"""

    def __init__(self, config_file: str = "gpu_manager_config_simplified.json"):
        self.config_file = config_file
        self.config = self.load_config()
        self.gpu_monitor = GPUMonitor()
        self.system_monitor = SystemMonitor()
        self.job_manager = GPUJobManager(self.config)
        self.display = ComprehensiveStatusDisplay()
        self.running = False

        # 注册信号处理
        signal.signal(signal.SIGINT, self.shutdown)
        signal.signal(signal.SIGTERM, self.shutdown)

    def load_config(self) -> Dict:
        """加载配置"""
        default_config = {
            "monitor_interval": 2,
            "min_reserved_gpus": 1,
            "max_reserved_gpus": 2,
            "max_total_gpus": 4,
            "job_prefix": "NDFS_",
            "auto_manage": True
        }

        if os.path.exists(self.config_file):
            try:
                with open(self.config_file, 'r') as f:
                    config = json.load(f)
                    default_config.update(config)
            except Exception:
                pass

        return default_config

    def shutdown(self, signum, frame):
        """优雅关闭"""
        print("\n正在关闭...")
        self.running = False
        sys.exit(0)

    def run_monitor(self):
        """运行监控模式"""
        print("🔍 启动综合监控模式...")
        self.running = True

        try:
            while self.running:
                nodes = self.gpu_monitor.get_gpu_nodes()
                jobs = self.gpu_monitor.get_my_jobs()
                system_status = self.system_monitor.get_system_status()

                self.display.show_status(nodes, jobs, system_status)
                time.sleep(self.config["monitor_interval"])
        except KeyboardInterrupt:
            pass

    def run_manage(self):
        """运行管理模式"""
        print("🤖 启动自动管理模式...")
        self.running = True

        try:
            while self.running:
                nodes = self.gpu_monitor.get_gpu_nodes()
                jobs = self.gpu_monitor.get_my_jobs()
                system_status = self.system_monitor.get_system_status()

                # 自动管理
                self.job_manager.auto_manage(nodes, jobs)

                # 显示状态
                self.display.show_status(nodes, jobs, system_status)
                time.sleep(self.config["monitor_interval"])
        except KeyboardInterrupt:
            pass

    def show_status(self):
        """显示单次状态"""
        nodes = self.gpu_monitor.get_gpu_nodes()
        jobs = self.gpu_monitor.get_my_jobs()
        system_status = self.system_monitor.get_system_status()
        self.display.show_status(nodes, jobs, system_status)

    def request_gpus(self, count: int):
        """手动申请GPU"""
        print(f"📝 申请 {count} 个GPU...")
        for i in range(count):
            job_id = self.job_manager.request_gpu(f"manual_{i+1}")
            if not job_id:
                break
            time.sleep(0.5)  # 避免过快提交

    def release_job(self, job_id: str):
        """释放作业"""
        self.job_manager.release_gpu(job_id)


def main():
    parser = argparse.ArgumentParser(description="综合GPU资源管理器")
    parser.add_argument("--config", default="gpu_manager_config_simplified.json",
                        help="配置文件路径")
    parser.add_argument("--status", action="store_true",
                        help="显示状态后退出")
    parser.add_argument("--manage", action="store_true",
                        help="启动自动管理模式")
    parser.add_argument("--request", type=int, metavar="N",
                        help="申请N个GPU")
    parser.add_argument("--release", metavar="JOB_ID",
                        help="释放指定作业")

    args = parser.parse_args()

    manager = ComprehensiveGPUManager(args.config)

    if args.status:
        manager.show_status()
    elif args.manage:
        manager.run_manage()
    elif args.request:
        manager.request_gpus(args.request)
    elif args.release:
        manager.release_job(args.release)
    else:
        # 默认监控模式
        manager.run_monitor()


if __name__ == "__main__":
    main()
