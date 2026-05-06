# Live demo runbook

## Setup checklist (before starting)

- [ ] Server: `systemctl is-active watchdog` returns `active`
- [ ] Attacker VM: joined to tailnet, no internet access (verified per [isolation.md](../modules/phantom/isolation.md))
- [ ] Operator device: joined to tailnet, `tailscale ping <server>` succeeds
- [ ] phantom binary built on the VM: `ls src/phantom/phantom`

## Step 1 — Open three terminals

| Terminal | Location | Purpose |
|---|---|---|
| A | Server | Watch watchdog logs live |
| B | Attacker VM | Launch phantom |
| C | Operator device (or server) | Watch system metrics |

## Step 2 — Terminal A: stream logs

```sh
journalctl -u watchdog -f
```

Leave this running. You will see idle poll lines at 1-second intervals.

## Step 3 — Terminal C: watch system metrics

```sh
watch -n 1 "uptime; free -h; ps aux --sort=-%cpu | head -15"
```

Record the baseline: load average ~0.x, memory free, process count.

## Step 4 — Terminal B: launch phantom

```sh
./phantom --target <server-tailnet-hostname> --depth 6 --sleep-ms 50 --duration 30
```

## Step 5 — Observe

- **Terminal C**: load average spikes, process count climbs, free memory drops.
- **Terminal A**: watchdog logs switch from `[INFO]` to `[ALERT]` then `[WARN]` (SIGKILL events).
- Within 30 seconds (or at `--duration` expiry), load begins to fall as phantom self-terminates and watchdog kills stragglers.

## Step 6 — Capture log excerpt

```sh
# On the server
sudo cp /var/log/watchdog/events.log ~/demo-$(date +%Y%m%dT%H%M%S).log
```

This log contains before/during/after metrics suitable for academic submission.

## Step 7 — Stop and verify recovery

After phantom exits, verify the server returns to baseline:

```sh
uptime           # load back under 1.0
ps aux | wc -l   # process count back to normal
systemctl is-active watchdog  # still running
```

## Related docs

- [troubleshooting.md](troubleshooting.md) — if anything goes wrong during the demo
- [data-flow.md](../architecture/data-flow.md) — what is happening inside watchdog during each phase
