# /proc and cgroup monitoring

## What is read and why

The `monitor` task collects system metrics by reading virtual files exposed by the kernel. No external tools or libraries are required — `/proc` and `/sys/fs/cgroup` are always available on Linux.

## /proc files

| File | What is extracted | Why |
|---|---|---|
| `/proc/loadavg` | 1-min load average | Quick indicator of overall CPU pressure |
| `/proc/[pid]/stat` | CPU ticks per process | Identify which PIDs are consuming the most CPU |
| `/proc/meminfo` | `MemFree`, `MemAvailable`, `Cached` | Detect memory exhaustion |
| `/proc/vmstat` | `pgmajfault` | Page fault rate — indicates swap pressure |

### Reading example (`/proc/loadavg`)

```
0.52 0.38 0.21 1/312 4582
```
Fields: 1-min avg, 5-min avg, 15-min avg, running/total threads, last PID.

watchdog reads the first field and compares against `WD_LOAD_THRESHOLD`.

## cgroup v2

cgroup v2 (unified hierarchy) is mounted at `/sys/fs/cgroup/` on Ubuntu 22.04+.

| File | What is read |
|---|---|
| `/sys/fs/cgroup/system.slice/cpu.stat` | `usage_usec` — cumulative CPU time in slice |
| `/sys/fs/cgroup/system.slice/memory.current` | Current memory usage in bytes |

When phantom's fork bomb runs, all spawned processes land in the same cgroup. Watching the cgroup counters allows watchdog to detect the burst without iterating every PID individually.

## Sampling cadence

The `monitor` task runs every `WD_POLL_MS` milliseconds (default 1000). On each cycle it reads all files above and stores a snapshot. The delta between consecutive snapshots is what triggers alerts.

## Code map

| Source file | Contents |
|---|---|
| `src/watchdog/monitor/proc_reader.c` | `/proc` file parsers |
| `src/watchdog/monitor/cgroup_reader.c` | cgroup v2 file parsers |
| `src/watchdog/monitor/monitor.c` | Sampling loop, delta calculation, threshold checks |
| `src/watchdog/monitor/monitor.h` | `wd_metrics_t` struct, function signatures |

## Related docs

- [scheduler.md](scheduler.md) — how `monitor` is scheduled
- [data-flow.md](../../architecture/data-flow.md) — how metrics feed the response
