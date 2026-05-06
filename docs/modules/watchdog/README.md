# watchdog daemon

## What it does

Runs as a background process on the host server. Continuously monitors system health via `/proc` and cgroup v2, applies a Round Robin scheduler among its internal threads, uses a POSIX semaphore to protect concurrent log writes, and responds to detected anomalies by signaling offending processes.

## Build

```sh
make -C src/watchdog
```

Produces `src/watchdog/watchdog` binary. Requires `gcc` and POSIX headers (`<semaphore.h>`, `<pthread.h>`).

## Run (development)

```sh
sudo ./src/watchdog/watchdog
```

Logs appear on stdout and are forwarded to journald when running under systemd.

## Install (production)

```sh
sudo make install
```

Copies the binary to `/usr/local/sbin/watchdog` and installs the systemd unit. See [infra/systemd.md](../../infra/systemd.md).

## Environment variables

| Variable | Default | Description |
|---|---|---|
| `WD_POLL_MS` | `1000` | Polling interval in milliseconds |
| `WD_LOG_PATH` | `/var/log/watchdog/events.log` | Event log path |
| `WD_LOAD_THRESHOLD` | `4.0` | /proc/loadavg 1-min value that triggers alert |
| `WD_MEM_FREE_MB` | `200` | Free memory below which alert triggers |

## Exit codes

| Code | Meaning |
|---|---|
| `0` | Clean shutdown (SIGTERM received) |
| `1` | Fatal error (see stderr / journald) |
| `2` | Permission error (needs root or cap_sys_ptrace) |

## Internal modules

- [scheduler.md](scheduler.md) — Round Robin over internal threads
- [semaphores.md](semaphores.md) — POSIX semaphore on the event log
- [monitoring.md](monitoring.md) — /proc and cgroup readers
- [logging.md](logging.md) — log format and rotation
