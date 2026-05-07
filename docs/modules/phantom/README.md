# phantom module

## What it does

Built on the attacker VM, then copied and **run directly on the server** through an SSH session over the Tailscale tunnel. The fork bomb is local to wherever the binary runs — it must execute on the server for watchdog to observe and respond to it. The Tailscale tunnel is the channel that lets the operator SSH from the isolated VM to the server.

## Prerequisites

1. The VM is joined to the same Tailscale tailnet as the server.
2. The VM has no outbound route to the public internet (no NAT beyond tailnet).
3. The server's `watchdog` daemon is running: `systemctl is-active watchdog`.
4. The phantom binary has been copied to the server: `scp phantom deploy@<server>:~/`.
5. Run as a **non-root** user on the server (phantom refuses to run as root).

## Build

```sh
# On the attacker VM (or locally)
make -C src/phantom
```

Produces `src/phantom/phantom` binary. Requires only `gcc`.

## Copy to server

```sh
# From the attacker VM, over Tailscale
scp src/phantom/phantom deploy@<server-tailnet-hostname>:~/phantom
```

## Run (on the server, via SSH from the VM)

```sh
ssh deploy@<server-tailnet-hostname>
./phantom --depth 6 --sleep-ms 50 --duration 30
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
