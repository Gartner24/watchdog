#!/usr/bin/env python3
"""
Colorize watchdog log output.

Usage:
  sudo journalctl -u watchdog -f        | python3 scripts/colorlog.py
  sudo tail -f /var/log/watchdog/events.log | python3 scripts/colorlog.py
"""
import sys
import re

R = '\033[0m'       # reset

# styles
BOLD  = '\033[1m'
DIM   = '\033[2m'

# colors
GRAY    = '\033[90m'
GREEN   = '\033[32m'
YELLOW  = '\033[33m'
RED     = '\033[31m'
BRED    = '\033[1;31m'
BWHITE  = '\033[1;37m'
CYAN    = '\033[36m'
MAGENTA = '\033[35m'
BLUE    = '\033[34m'
ORANGE  = '\033[38;5;214m'

LEVEL_COLOR = {
    'ALERT': f'{BRED}{BOLD}',
    'WARN ': f'{ORANGE}{BOLD}',
    'INFO ': GREEN,
    'DEBUG': DIM,
}

MODULE_COLOR = {
    'monitor  ': CYAN,
    'responder': MAGENTA,
    'scheduler': BLUE,
    'watchdog ': BWHITE,
}

# keywords to highlight wherever they appear
KEYWORDS = {
    'SIGKILL':            f'{BRED}{BOLD}',
    'threshold exceeded': f'{RED}{BOLD}',
    'whitelisted':        f'{YELLOW}{BOLD}',
    'shutting down':      f'{YELLOW}',
    'starting':           f'{GREEN}{BOLD}',
    'Round Robin':        f'{CYAN}',
    'fatal:':             f'{BRED}{BOLD}',
    'warning:':           f'{YELLOW}',
}


def colorize(line: str) -> str:
    # journalctl prefix: "May 21 06:25:03 hostname watchdog[PID]: "
    # dim everything before the first [
    bracket = line.find('[')
    if bracket > 0:
        prefix = line[:bracket]
        rest   = line[bracket:]
        line   = f'{DIM}{prefix}{R}{rest}'

    # timestamp [2026-05-21T06:25:03Z]
    line = re.sub(
        r'(\[\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z\])',
        f'{GRAY}\\1{R}', line
    )

    # log level [ALERT] [WARN ] [INFO ] [DEBUG]
    def replace_level(m):
        lvl = m.group(1)
        color = LEVEL_COLOR.get(lvl, '')
        return f'[{color}{lvl}{R}]'
    line = re.sub(r'\[([A-Z ]{5})\]', replace_level, line)

    # module name [monitor   ] etc.
    def replace_module(m):
        mod = m.group(1)
        color = MODULE_COLOR.get(mod, DIM)
        return f'[{color}{mod}{R}]'
    line = re.sub(r'\[([a-z ]{9})\]', replace_module, line)

    # keywords
    for word, color in KEYWORDS.items():
        line = line.replace(word, f'{color}{word}{R}')

    # key=value pairs: dim the key=, bold the value
    line = re.sub(
        r'(?<!\033\[)(\b[a-z_]+)=([\w./:-]+)',
        f'{DIM}\\1={R}{BOLD}\\2{R}', line
    )

    return line


def main():
    try:
        for raw in sys.stdin:
            sys.stdout.write(colorize(raw))
            sys.stdout.flush()
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()
