# watchdog

> Linux server surveillance and threat response daemon — Operating Systems project, Universidad Tecnológica de Pereira.

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
- [ ] watchdog daemon - Round Robin scheduler
- [ ] watchdog daemon - Semaphore log access
- [ ] watchdog daemon - /proc + cgroups monitor
- [ ] phantom - fork bomb module
- [ ] Tailscale tunnel setup
- [ ] Live demo and log output

---

## Academic context

Operating Systems — Universidad Tecnológica de Pereira
Topics covered: process scheduling, semaphores, memory management, OS security
