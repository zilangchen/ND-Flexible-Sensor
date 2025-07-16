#!/usr/bin/env python3
"""GPU Monitor for Notre Dame CRC

This script periodically calls `free_gpus.sh @crc_gpu` to collect GPU
availability across the CRC GPU partition and displays the results in a
read-only dashboard.  It is designed with an extensible structure so that
future collectors or visualizers can be plugged in with minimal changes.

Usage
-----
$ module load conda               # 确保 Python3 可用
$ python scripts/gpu_monitor.py    # 默认 30 秒刷新一次

选项::

    -i, --interval INT  刷新间隔秒数 (默认 30)

Dependencies
------------
* Python ≥ 3.8
* 可选: rich ‑ 若安装则显示彩色表格，否则退化为纯文本。

"""
from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
import time
from dataclasses import dataclass
from typing import List


@dataclass
class NodeStatus:
    """Represent GPU availability for a single node."""

    node: str
    free_gpus: int

    def as_row(self) -> List[str]:
        return [self.node, str(self.free_gpus)]


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
                    NodeStatus(node=match.group(1), free_gpus=int(match.group(2)))
                )
        return statuses


class ConsoleRenderer:
    """Render GPU status to the terminal using rich if available."""

    def __init__(self) -> None:
        try:
            from rich.table import Table
            from rich.console import Console

            self._Table = Table  # type: ignore
            self._console = Console()
            self._rich_available = True
        except ImportError:
            # Fallback to plain text
            self._rich_available = False

    def render(self, statuses: List[NodeStatus]) -> None:
        if not statuses:
            print("No GPU status collected. Check command availability.")
            return

        if self._rich_available:
            table = self._Table(title="CRC GPU Monitor")
            table.add_column("Node", style="cyan")
            table.add_column("Free GPUs", justify="right", style="green")
            for status in statuses:
                table.add_row(*status.as_row())

            # Clear screen before printing new table
            self._console.clear()
            self._console.print(table)
        else:
            # Simple text output
            os.system("clear || cls")
            print("Node\tFree GPUs")
            print("-" * 20)
            for status in statuses:
                print(f"{status.node}\t{status.free_gpus}")


class GPUMonitor:
    """Main application orchestrating collection and rendering."""

    def __init__(self, interval: int = 30) -> None:
        self.interval = max(5, interval)  # 防止过于频繁
        self.collector = CRCGPUCollector()
        self.renderer = ConsoleRenderer()

    def run(self) -> None:
        try:
            while True:
                statuses = self.collector.collect()
                self.renderer.render(statuses)
                time.sleep(self.interval)
        except KeyboardInterrupt:
            print("\n[Info] GPU monitor stopped by user.")


def parse_args(args: List[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="CRC GPU real-time monitor")
    parser.add_argument(
        "-i",
        "--interval",
        type=int,
        default=30,
        help="刷新间隔（秒），最小 5 秒，默认 30 秒",
    )
    return parser.parse_args(args)


def main() -> None:
    ns = parse_args()
    monitor = GPUMonitor(interval=ns.interval)
    monitor.run()


if __name__ == "__main__":
    main() 