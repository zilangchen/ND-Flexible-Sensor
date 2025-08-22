#!/usr/bin/env python3
"""
Real-time 10x10 ΔC heatmap from ESP32+FDC2214 over serial.

Usage:
  python scripts/visualize/serial_heatmap.py /dev/cu.usbserial-XXXX 115200

Notes:
  - Script will send 'c' once to switch firmware into CSV mode.
  - Firmware can toggle modes via serial: 'h' ASCII, 'c' CSV, 'q' quiet.
"""

import sys
import time
from typing import Optional

import numpy as np

try:
    import serial  # type: ignore
except Exception as exc:  # pragma: no cover
    print("Please install pyserial: pip install pyserial")
    raise

try:
    import matplotlib.pyplot as plt  # type: ignore
    from matplotlib.animation import FuncAnimation  # type: ignore
except Exception as exc:  # pragma: no cover
    print("Please install matplotlib: pip install matplotlib")
    raise


def open_serial(port: str, baud: int) -> serial.Serial:
    ser = serial.Serial(port, baud, timeout=0.05)
    time.sleep(0.2)
    # Flush any boot logs
    ser.reset_input_buffer()
    # Request CSV mode
    try:
        ser.write(b"c")
        ser.flush()
    except Exception:
        pass
    return ser


def read_frame(ser: serial.Serial, rows: int, cols: int) -> Optional[np.ndarray]:
    matrix = []
    start_time = time.time()
    while True:
        # Avoid blocking indefinitely
        if time.time() - start_time > 1.0:
            return None
        try:
            raw = ser.readline()
        except Exception:
            return None
        if not raw:
            continue
        try:
            line = raw.decode("utf-8", errors="ignore").strip()
        except Exception:
            continue
        if not line:
            # Empty line -> separator; continue reading
            if matrix:
                # Some firmwares emit an empty line after frame; ignore
                continue
            else:
                continue

        # Ignore non-CSV lines like frame stats
        parts = line.split(",")
        if len(parts) != cols:
            # Not a valid row; reset accumulator
            matrix = []
            continue
        try:
            row_vals = [float(x) for x in parts]
        except ValueError:
            matrix = []
            continue

        matrix.append(row_vals)
        if len(matrix) == rows:
            return np.array(matrix, dtype=float)


def main() -> None:
    port = sys.argv[1] if len(sys.argv) > 1 else "/dev/ttyUSB0"
    baud = int(sys.argv[2]) if len(sys.argv) > 2 else 115200
    rows, cols = 10, 10

    ser = open_serial(port, baud)

    # Matplotlib setup
    plt.ion()
    fig, ax = plt.subplots()
    img = ax.imshow(np.zeros((rows, cols)), cmap="viridis")
    cbar = plt.colorbar(img, ax=ax)
    ax.set_title("ΔC Heatmap (F)")
    ax.set_xticks([])
    ax.set_yticks([])

    # Optionally hold color scale steady by commenting the auto-clim lines
    def update(_):
        frame = None
        # Try multiple reads between renders to lower latency
        for _ in range(40):
            f = read_frame(ser, rows, cols)
            if f is not None:
                frame = f
                break
        if frame is None:
            return img
        img.set_data(frame)
        # Auto scale to current frame range
        vmin = float(np.min(frame))
        vmax = float(np.max(frame))
        if vmax - vmin < 1e-18:
            vmax = vmin + 1e-18
        img.set_clim(vmin=vmin, vmax=vmax)
        return img

    ani = FuncAnimation(fig, update, interval=50, blit=False)
    try:
        plt.show(block=True)
    finally:
        try:
            ser.close()
        except Exception:
            pass


if __name__ == "__main__":
    main()

