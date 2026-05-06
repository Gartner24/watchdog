# Data flow

## Attack timeline

```
A. IDLE
   watchdog polls /proc every N seconds.
   Round Robin scheduler rotates among: [monitor] [log-writer] [responder].
   Semaphore protects the shared event log file.

B. ATTACK STARTS
   phantom (on attacker VM) executes fork bomb through Tailscale tunnel.
   Process count on server spikes. CPU load climbs.

C. WATCHDOG DETECTS
   monitor thread reads /proc/loadavg, /proc/meminfo, /proc/vmstat.
   cgroup v2 usage counters exceed defined thresholds.
   monitor writes an alert event to the shared log (semaphore-protected write).

D. RESPONSE
   responder thread activates: applies SIGKILL to processes exceeding cgroup limits.
   Scheduler continues rotating threads under load — demonstrates RR behavior under stress.

E. RECOVERY
   Process count normalizes. /proc readings return to baseline.
   watchdog continues polling. Event log shows before/during/after metrics.
```

## Data paths

| Path | Direction | What |
|---|---|---|
| `/proc/loadavg` | server → watchdog | 1-min load average |
| `/proc/[pid]/stat` | server → watchdog | per-process CPU ticks |
| `/proc/meminfo` | server → watchdog | free/available memory |
| `/proc/vmstat` | server → watchdog | page fault counters |
| `/sys/fs/cgroup/...` | server → watchdog | cgroup v2 CPU and memory usage |
| `/var/log/watchdog/events.log` | watchdog → disk | structured event log |
| `journald` | watchdog → systemd | stdout forwarded to journal |

## Algorithm involvement per phase

| Phase | Algorithm | File |
|---|---|---|
| A, B, C, D, E | Round Robin scheduler | [scheduler.md](../modules/watchdog/scheduler.md) |
| C, D | POSIX semaphore (log access) | [semaphores.md](../modules/watchdog/semaphores.md) |
| A, B, C | /proc + cgroup monitoring | [monitoring.md](../modules/watchdog/monitoring.md) |
