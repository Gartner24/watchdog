"""Starts/stops watchdog and captures journald output around a demo window."""
import subprocess
import sys


def start():
    subprocess.run(["systemctl", "start", "watchdog"], check=True)
    print("[runner] watchdog started")


def stop():
    subprocess.run(["systemctl", "stop", "watchdog"], check=True)
    print("[runner] watchdog stopped")


def stream_logs():
    proc = subprocess.Popen(
        ["journalctl", "-u", "watchdog", "-f", "--no-pager"],
        stdout=sys.stdout,
    )
    return proc


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: runner.py start|stop|logs")
        sys.exit(1)
    cmd = sys.argv[1]
    if cmd == "start":
        start()
    elif cmd == "stop":
        stop()
    elif cmd == "logs":
        p = stream_logs()
        try:
            p.wait()
        except KeyboardInterrupt:
            p.terminate()
    else:
        print(f"unknown command: {cmd}")
        sys.exit(1)
