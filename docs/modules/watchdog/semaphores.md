# Semaphores

## Why a semaphore is needed

The `monitor` task and the `responder` task can both trigger writes to the shared event log file at the same time. Without synchronization, concurrent writes produce interleaved or corrupted log lines.

A POSIX named semaphore serializes access: only one task holds the write lock at a time. The other blocks until the lock is released. This is intentionally simple — a mutex would also work, but the semaphore demonstrates the OS primitive studied in class.

## API used

```c
#include <semaphore.h>

sem_t *log_sem = sem_open("/watchdog_log", O_CREAT, 0600, 1);  // initial value = 1

// before writing
sem_wait(log_sem);
write_log_event(event);
sem_post(log_sem);

// cleanup
sem_close(log_sem);
sem_unlink("/watchdog_log");
```

`sem_open` with initial value `1` creates a binary semaphore (mutex-like). `sem_wait` decrements to 0 (acquires). `sem_post` increments back to 1 (releases).

## Failure modes

| Scenario | Result | Handling |
|---|---|---|
| `sem_open` fails | `log_sem == SEM_FAILED` | Fatal — watchdog exits with code 1 |
| Task killed while holding sem | Sem stays at 0, other tasks block forever | Avoided by not using SIGKILL on own threads; responder only kills phantom-spawned PIDs |
| Semaphore name collision on restart | `O_CREAT` is idempotent | Safe — `sem_unlink` in shutdown cleans it up |

## Code map

| Source file | Contents |
|---|---|
| `src/watchdog/semaphore/log_sem.c` | `sem_open`, `sem_wait`, `sem_post`, `sem_close` wrappers |
| `src/watchdog/semaphore/log_sem.h` | Function signatures |

## Related docs

- [logging.md](logging.md) — the log file that the semaphore protects
- [scheduler.md](scheduler.md) — which tasks contend for the semaphore
