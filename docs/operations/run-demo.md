# Live demo runbook

End-to-end guide to run the watchdog + phantom demo. Based on verified runs against Ubuntu 22.04 VPS.

---

## Prerequisites

- watchdog installed and running: `systemctl is-active watchdog` returns `active`
- phantom binary built: `ls src/phantom/phantom`
- Three terminal sessions open on the server (or two on server + one on attacker VM)

---

## Step 0 - Tune the load threshold (required for modern VPS hardware)

Modern VPS hosts have multiple cores and low idle load. The default threshold of `4.0` will never trigger on a lightly loaded server. Lower it before the demo.

```bash
sudo sed -i 's/WD_LOAD_THRESHOLD=4.0/WD_LOAD_THRESHOLD=0.2/' /etc/systemd/system/watchdog.service
sudo systemctl daemon-reload
sudo systemctl restart watchdog
```

Confirm it took effect:

```bash
sudo journalctl -u watchdog -n 3
# Should show: starting poll_ms=1000 load_threshold=0.20
```

You only need to do this once. The value persists across restarts.

---

## Step 1 - Terminal 1: stream live journal logs

```bash
sudo journalctl -u watchdog -f | python3 scripts/colorlog.py
```

Leave this running. At idle you will see one line per second:

```
[INFO ] [monitor] poll load=0.04 mem_free_mb=4500 pgmajfault_delta=0 cpu_usec_delta=18000
```

This confirms the Round Robin scheduler is ticking and `/proc` reads are working.

---

## Step 2 - Terminal 2: stream the log file on disk

```bash
sudo tail -f /var/log/watchdog/events.log | python3 scripts/colorlog.py
```

This shows the same output written to disk. Both terminals should display identical lines at identical timestamps. This proves the semaphore-protected dual-write is working: the daemon holds the POSIX semaphore, writes to the file, then releases it — preventing concurrent log corruption.

---

## Step 3 - Terminal 3: launch phantom (the attack)

phantom is a fork bomb. It runs **on the server** as a non-root user. It will be refused if run as root.

```bash
./src/phantom/phantom --depth 8 --sleep-ms 5 --duration 60
```

Expected output:

```
[phantom] pid=XXXXX depth=8 sleep_ms=5 duration_s=60
[phantom] launching fork bomb - kill -TERM XXXXX to abort
```

phantom will now recursively fork up to 2^8 = 256 leaf processes sleeping for 60 seconds each. CPU usage spikes. Load average climbs.

---

## Step 4 - Watch the response sequence

Switch to Terminal 1 or 2. Within 5-15 seconds of launching phantom you will see this sequence:

**Phase 1 - normal:**
```
[INFO ] [monitor] poll load=0.04 ...
```

**Phase 2 - load rising (phantom spawning):**
```
[INFO ] [monitor] poll load=0.11 ... cpu_usec_delta=299331
[INFO ] [monitor] poll load=0.18 ... cpu_usec_delta=274847
[INFO ] [monitor] poll load=0.40 ... cpu_usec_delta=108056
```

**Phase 3 - threshold breach and kill:**
```
[ALERT] [monitor] threshold exceeded load=0.40 threshold=0.20 breach_mask=0x1
[INFO ] [responder] processing alert breach_mask=0x1 suspect_pid=254267 load=0.40
[WARN ] [responder] SIGKILL pid=254267 ticks=540 load=0.40
```

The responder dequeued the alert from the ring buffer, identified the top CPU-consuming process, and sent SIGKILL.

**Phase 4 - whitelisted system processes (expected):**
```
[WARN ] [responder] suspect pid=15901 is whitelisted - skipping
```

After phantom's children die, a system process (sshd, kernel thread) may briefly appear as top consumer. The whitelist blocks the daemon from killing it. This is correct behavior.

**Phase 5 - recovery:**
```
[INFO ] [monitor] poll load=0.18 ...
[INFO ] [monitor] poll load=0.13 ...
[INFO ] [monitor] poll load=0.08 ...
```

No more ALERT lines. Load decays below threshold. System back to normal.

---

## What each log field means

```
[2026-05-21T06:25:03Z] [ALERT] [monitor] threshold exceeded load=0.40 threshold=0.20 mem_free_mb=4424 min=200 breach_mask=0x1
```

| Field | Value | Meaning |
|---|---|---|
| timestamp | `06:25:03Z` | UTC wall clock |
| level | `ALERT` | threshold crossed, alert pushed to queue |
| module | `monitor` | emitted by the monitor task in the RR scheduler |
| `load` | `0.40` | 1-minute load average read from `/proc/loadavg` |
| `threshold` | `0.20` | configured limit (`WD_LOAD_THRESHOLD`) |
| `mem_free_mb` | `4424` | free RAM read from `/proc/meminfo` |
| `breach_mask` | `0x1` | bitmask: bit 0 = load breach, bit 1 = memory breach, bit 2 = pgfault breach |

```
[WARN ] [responder] SIGKILL pid=254267 ticks=540 load=0.40
```

| Field | Value | Meaning |
|---|---|---|
| level | `WARN` | destructive action taken |
| module | `responder` | emitted by the responder task in the RR scheduler |
| `pid` | `254267` | top CPU process identified via `/proc/N/stat` scan |
| `ticks` | `540` | CPU ticks consumed by that process |

---

## Full Tailscale demo (attacker VM scenario)

The above runs phantom directly on the server via SSH. For the full demo with an isolated attacker VM:

**On the attacker VM:**

```bash
# Copy the phantom binary to the server over the Tailscale tunnel
scp src/phantom/phantom gartner@<server-tailscale-ip>:~/phantom

# SSH into the server from the VM
ssh gartner@<server-tailscale-ip>

# Run phantom on the server (you are now inside the server)
./phantom --depth 8 --sleep-ms 5 --duration 60
```

phantom runs locally on the server regardless — the Tailscale tunnel is the SSH channel. The fork bomb is always local to the machine watchdog monitors.

Verify the VM is isolated before the demo:

```bash
# Must FAIL (VM has no internet)
curl --max-time 5 https://example.com

# Must SUCCEED (Tailscale tunnel works)
tailscale ping <server-machine-name>
```

---

## Save the log for academic submission

```bash
sudo cp /var/log/watchdog/events.log ~/demo-$(date +%Y%m%dT%H%M%S).log
```

The file contains the full before/during/after sequence with UTC timestamps.

---

## Stop and clean up

```bash
# If phantom is still running
kill -TERM <phantom-pid>

# Stop watchdog after the demo
sudo systemctl stop watchdog

# Re-enable for next run
sudo systemctl start watchdog
```

---

## Related docs

- [deploy.md](deploy.md) - install watchdog from scratch
- [troubleshooting.md](troubleshooting.md) - if anything fails
- [data-flow.md](../architecture/data-flow.md) - internals during each phase
- [tailscale.md](../infra/tailscale.md) - Tailscale ACL and VM isolation setup
