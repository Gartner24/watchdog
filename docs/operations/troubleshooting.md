# Troubleshooting

## watchdog won't start

**Symptom:** `systemctl start watchdog` fails immediately.

**Check:**
```sh
journalctl -u watchdog -n 30
```

Common causes:
- Binary missing: run `sudo make install` first.
- Permission error (exit code 2): watchdog needs root. Verify `User=root` in the unit file.
- Log directory missing: `sudo mkdir -p /var/log/watchdog && sudo chmod 750 /var/log/watchdog`.
- Semaphore leftover from a crashed previous run: `ls /dev/shm/sem.watchdog_log` — if it exists, delete it: `sudo rm /dev/shm/sem.watchdog_log`.

---

## No /proc readings in logs

**Symptom:** watchdog starts but logs show zeros or errors reading `/proc/...`.

**Check:**
- `/proc` is mounted: `mount | grep proc`. It always is on Linux — if not, reboot.
- cgroup v2 is active: `ls /sys/fs/cgroup/cgroup.controllers`. If this file is missing, the system uses cgroup v1. See the cgroup v2 migration note in [monitoring.md](../modules/watchdog/monitoring.md).

---

## Fork bomb hung the attacker VM

**Symptom:** SSH to the attacker VM is unresponsive during or after the demo.

**Fix (from the hypervisor host):**
```sh
virsh destroy phantom-vm   # or equivalent for your hypervisor
```

Restart the VM cleanly. This is expected behavior if `--depth` is set too high. Use `--depth 5` or lower for safer demos.

---

## Tailscale not reachable

**Symptom:** `tailscale ping <server>` from attacker VM times out.

**Check:**
1. Is Tailscale running on both machines? `tailscale status` on each.
2. Is the machine key expired? Log in again: `sudo tailscale up`.
3. Are ACLs blocking it? Check the Tailscale admin console — confirm `tag:phantom` → `tag:server` is allowed.
4. Is UFW blocking Tailscale port? `sudo ufw status` — port 41641/udp must be open.

---

## watchdog keeps killing its own processes

**Symptom:** watchdog logs show SIGKILL events at idle (no phantom running).

**Cause:** The thresholds (`WD_LOAD_THRESHOLD`, `WD_MEM_FREE_MB`) are set too low for the server's normal load.

**Fix:** Increase the thresholds in `/etc/systemd/system/watchdog.service`, then:
```sh
sudo systemctl daemon-reload && sudo systemctl restart watchdog
```

---

## Log file grows too fast

**Symptom:** `/var/log/watchdog/events.log` fills disk.

**Fix:** Reduce `WD_POLL_MS` (increase the value) to poll less frequently. The logrotate config handles daily rotation, but an active demo generates ~1 line/second. Consider redirecting logs to journald only during the demo by setting `WD_LOG_PATH=/dev/null` temporarily.
