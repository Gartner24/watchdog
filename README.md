# watchdog

> Linux server surveillance and threat response daemon — Operating Systems project, Universidad Tecnologica de Pereira.

---

## Documentation

Full module docs, infrastructure guides, and the operations runbook live in [`docs/`](docs/README.md).

| Quick links | |
|---|---|
| [Architecture overview](docs/architecture/overview.md) | System topology and nodes |
| [Data flow](docs/architecture/data-flow.md) | What happens during an attack |
| [Deploy](docs/operations/deploy.md) | Provision a fresh server |
| [Run demo](docs/operations/run-demo.md) | Live demo runbook |

---

## What is this?

**watchdog** is a system daemon that runs in the background on a real Linux server and continuously monitors its health. When it detects abnormal behavior, it activates internal subsystems to make automatic resource management and protection decisions, applying core OS algorithms studied in class.

It works alongside **phantom**, an isolated attacker module that launches a controlled fork bomb against the server through a Tailscale VPN tunnel, forcing watchdog to detect, react, and mitigate in real time.

---

## Modules

### watchdog (daemon)

Runs on the host server. Monitors:

- Active processes and CPU usage
- Memory pressure and page faults via `/proc`
- Shared resource access via semaphores

Applies the following OS algorithms:

- **Round Robin scheduling** - manages CPU time between competing processes
- **Semaphores** - controls concurrent access to the event log
- **cgroups monitoring** - tracks memory and CPU limits per process group

### phantom (attacker)

Runs from an isolated virtual machine. Connects to the server via **Tailscale VPN** and launches a fork bomb attack, stressing the scheduler and system resources directly. The VM has no outbound access to external networks — the attack is fully contained.

---

## Infrastructure

This project runs on top of a production-grade Linux server with the following stack already operational:

- **OS** - Ubuntu Server (hardened)
- **Reverse proxy** - Nginx + Docker + Certbot (TLS via Cloudflare DNS-01)
- **Firewall** - UFW + fail2ban (SSH brute-force protection)
- **SSH** - key-only authentication, non-root user, custom port
- **VPN** - Tailscale (remote access and attacker tunnel)
- **Container runtime** - Docker + Docker Compose

Infrastructure references:
- [vps-setup](https://github.com/Gartner24/vps-setup) — server hardening guide and tooling
- [vps-proxy](https://github.com/Gartner24/vps-proxy) — Nginx reverse proxy stack

---

## Stack

| Layer | Technology |
|---|---|
| Core algorithms | C |
| Orchestration and logging | Python |
| Process monitoring | `/proc`, `cgroups`, `vmstat` |
| Network tunnel | Tailscale VPN |
| Attacker environment | Isolated VM |

---

## Project status

- [x] Base infrastructure operational
- [x] Architecture defined
- [x] watchdog daemon - Round Robin scheduler
- [x] watchdog daemon - Semaphore log access
- [x] watchdog daemon - /proc + cgroups monitor
- [x] phantom - fork bomb module
- [x] Tailscale tunnel setup
- [x] Live demo and log output confirmed

---

## Log colors

Pipe any log output through `scripts/colorlog.py` for a color-coded view:

```bash
sudo journalctl -u watchdog -f            | python3 scripts/colorlog.py
sudo tail -f /var/log/watchdog/events.log | python3 scripts/colorlog.py
```

### What the log levels mean

- **INFO** (green) - Everything is normal. The daemon completed one poll cycle: it read `/proc/loadavg`, `/proc/meminfo`, and `/proc/N/stat` for all processes. No action taken.
- **ALERT** (bright red) - A metric crossed a configured threshold. The 1-minute load average exceeded `WD_LOAD_THRESHOLD`, or free memory dropped below `WD_MEM_FREE_MB`. The monitor pushed an alert onto the ring buffer queue for the responder to handle.
- **WARN** (orange) - The responder dequeued an alert and took a destructive action: it sent `SIGKILL` to the process consuming the most CPU ticks at that moment.

### What the keywords mean

- **threshold exceeded** - The load average or free memory crossed the configured limit. The daemon read this from `/proc/loadavg` and `/proc/meminfo`. This is the trigger for everything that follows.
- **SIGKILL** - The operating system signal that immediately terminates a process with no cleanup. Unlike SIGTERM, the target process cannot catch or ignore it. Watchdog uses this as a last resort to remove the top offending process.
- **whitelisted** - The process identified as the top CPU consumer is a protected system process (e.g. sshd, kernel threads). Watchdog refuses to kill it to avoid destabilizing the server. This is correct behavior — the daemon keeps alerting until load decays naturally.
- **breach_mask** - A bitmask that encodes which thresholds were crossed at the same time. `0x1` means load average breach, `0x2` means memory breach, `0x4` means page fault spike. Multiple bits can be set simultaneously.

### What the modules mean

- **monitor** - Reads system metrics every poll cycle and decides whether to raise an alert.
- **responder** - Consumes alerts from the queue and applies the kill action.
- **scheduler** - The Round Robin task scheduler that gives equal CPU time slices to monitor, responder, and the log writer.
- **watchdog** - The daemon itself, emitting startup and shutdown events.

### Color reference

| Color | What it marks |
|---|---|
| Gray/dim | Timestamp and journalctl prefix |
| Green | `[INFO ]` - normal poll, no action |
| Orange/bold | `[WARN ]` - SIGKILL was sent |
| Bright red/bold | `[ALERT]` - threshold breached, alert queued |
| Cyan | `[monitor]` module output |
| Magenta | `[responder]` module output |
| Blue | `[scheduler]` module output |
| White/bold | `[watchdog]` daemon lifecycle events |
| Bright red/bold | `SIGKILL` keyword |
| Red/bold | `threshold exceeded` phrase |
| Yellow/bold | `whitelisted` - kill blocked, system process protected |
| Dim key + bold value | All `key=value` metric pairs |

---

## Academic context

Operating Systems — Universidad Tecnológica de Pereira
Topics covered: process scheduling, semaphores, memory management, OS security
