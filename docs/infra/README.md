# Infrastructure

> Adapted from [vps-setup](https://github.com/Gartner24/vps-setup) and [vps-proxy](https://github.com/Gartner24/vps-proxy).
> Only the subset relevant to this project is included here. For the full setup (multi-project reverse proxy, certificate management, advanced hardening), refer to those repos directly.

## What is covered here

| Doc | Contents |
|---|---|
| [server-hardening.md](server-hardening.md) | SSH hardening, non-root user, custom port |
| [firewall.md](firewall.md) | UFW rules + fail2ban for SSH |
| [docker.md](docker.md) | Docker install (used for reverse proxy only, NOT for watchdog) |
| [tailscale.md](tailscale.md) | Tailscale setup, ACLs for attacker tunnel |
| [systemd.md](systemd.md) | watchdog.service unit, install, manage |

## Assumed starting point

A freshly provisioned Ubuntu Server 22.04 LTS with:
- Root SSH access (temporary, hardened away in `server-hardening.md`)
- A public IP
- No other services running
