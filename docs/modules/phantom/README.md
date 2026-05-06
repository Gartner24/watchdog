# phantom module

## What it does

Runs from an isolated virtual machine. Connects to the target server through the Tailscale tailnet and launches a controlled fork bomb, rapidly spawning child processes to stress the server's scheduler, CPU, and memory. watchdog must detect and respond.

## Prerequisites

1. The VM is joined to the same Tailscale tailnet as the server.
2. The VM has no outbound route to the public internet (no NAT beyond tailnet).
3. The server's `watchdog` daemon is running: `systemctl is-active watchdog`.
4. Tailscale connectivity is confirmed: `tailscale ping <server-tailnet-ip>`.

## Build

```sh
make -C src/phantom
```

Produces `src/phantom/phantom` binary. Requires only `gcc`.

## Run

```sh
# From the attacker VM
./phantom --target <server-tailnet-ip> --depth 6 --sleep-ms 50
```

| Flag | Default | Description |
|---|---|---|
| `--depth` | `6` | Max fork depth (limits blast radius) |
| `--sleep-ms` | `50` | Sleep between fork waves (ms) |
| `--duration` | `30` | Seconds before phantom self-terminates |

## Kill switch

If the demo gets out of control:

```sh
# On the server (SSH in from operator device)
sudo pkill -9 -f phantom
sudo cgroupctl -g system.slice --kill  # if cgroup delegation is set up
```

## Module docs

- [fork-bomb.md](fork-bomb.md) — attack mechanics
- [isolation.md](isolation.md) — VM network isolation requirements
