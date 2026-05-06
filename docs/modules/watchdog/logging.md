# Logging

## Log file

Events are written to `/var/log/watchdog/events.log`. The directory is created by `deploy/install.sh` with `750` permissions (root:root).

## Log line format

```
[ISO8601_TIMESTAMP] [LEVEL] [TASK] message key=value key=value ...
```

Example:

```
[2026-05-06T02:15:30Z] [INFO]  [monitor]   poll load=3.82 mem_free_mb=145 pgmajfault_delta=42
[2026-05-06T02:15:30Z] [ALERT] [monitor]   threshold exceeded load=3.82 threshold=4.0
[2026-05-06T02:15:31Z] [INFO]  [log-writer] flushed events=2
[2026-05-06T02:15:31Z] [WARN]  [responder] sending SIGKILL pid=18432 cgroup_cpu_usec=9200000
```

## journald integration

When `watchdog` runs under systemd, all stdout output is automatically forwarded to the journal. Observe in real time:

```sh
journalctl -u watchdog -f
```

Filter by level (requires structured logging — see `WD_LOG_JSON=1`):

```sh
journalctl -u watchdog -f -p warning
```

## Log rotation

`logrotate` config is installed to `/etc/logrotate.d/watchdog` by `deploy/install.sh`:

```
/var/log/watchdog/events.log {
    daily
    rotate 7
    compress
    missingok
    notifempty
    postrotate
        systemctl kill -s HUP watchdog
    endscript
}
```

## Related docs

- [semaphores.md](semaphores.md) — concurrent write protection
- [systemd.md](../../infra/systemd.md) — journald setup
