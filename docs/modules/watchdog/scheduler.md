# Round Robin scheduler

## Theory

Round Robin (RR) is a preemptive CPU scheduling algorithm that assigns each runnable process or thread a fixed time slice (quantum). When the quantum expires, the current task is moved to the back of the ready queue and the next one runs. No task can monopolize the CPU: every task gets fair progress over time.

Key property: under high load, RR degrades gracefully — all tasks slow down proportionally rather than some tasks starving while others run.

## How watchdog uses it

The daemon maintains three internal worker tasks in a circular queue:

| Task | Responsibility |
|---|---|
| `monitor` | Reads /proc files and cgroup counters |
| `log-writer` | Flushes the in-memory event buffer to disk |
| `responder` | Applies mitigations (SIGKILL) when thresholds are exceeded |

The main thread acts as the scheduler. On each tick (configurable via `WD_POLL_MS`), it grants the next task in the queue a fixed time slot, then advances the pointer. This ensures the monitor is never blocked waiting for a slow disk write, and the responder is never starved by a high-frequency monitor loop.

## Time quantum

Default quantum = `WD_POLL_MS / 3`. For the default 1000ms poll interval, each task gets ~333ms before yielding. This is adjustable for demo purposes — a shorter quantum makes the RR behavior more visible in logs.

## Pseudocode

```
queue = [monitor, log_writer, responder]
i = 0
loop:
    task = queue[i % 3]
    run task for QUANTUM ms
    i++
    sleep until next tick
```

## Code map

| Source file | Contents |
|---|---|
| `src/watchdog/scheduler/rr_scheduler.c` | Queue management, tick loop |
| `src/watchdog/scheduler/rr_scheduler.h` | `rr_task_t` struct, function signatures |
| `src/watchdog/main.c` | Scheduler initialization |

## Related docs

- [monitoring.md](monitoring.md) — what the `monitor` task actually reads
- [semaphores.md](semaphores.md) — how `log-writer` protects concurrent access
