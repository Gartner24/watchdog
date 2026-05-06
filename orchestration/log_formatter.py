"""Pretty-prints watchdog event log with before/during/after attack markers."""
import sys
import re
from datetime import datetime, timezone


ALERT_PATTERN = re.compile(r"\[ALERT\]")
INFO_PATTERN  = re.compile(r"\[INFO\]")
WARN_PATTERN  = re.compile(r"\[WARN\]")


def colorize(line: str) -> str:
    if ALERT_PATTERN.search(line):
        return f"\033[91m{line}\033[0m"  # red
    if WARN_PATTERN.search(line):
        return f"\033[93m{line}\033[0m"  # yellow
    return line


def format_file(path: str) -> None:
    with open(path) as f:
        for line in f:
            print(colorize(line), end="")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("usage: log_formatter.py <events.log>")
        sys.exit(1)
    format_file(sys.argv[1])
