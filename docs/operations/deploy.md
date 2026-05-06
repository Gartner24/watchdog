# Deploy

End-to-end provisioning on a fresh Ubuntu Server 22.04 LTS.

## Prerequisites

- A machine with Ubuntu 22.04 LTS and a public IP.
- Root SSH access (temporary).
- The `watchdog` repository cloned locally.

## Steps

### 1. Harden the server

Follow [infra/server-hardening.md](../infra/server-hardening.md) in full.
Outcome: non-root `deploy` user, SSH key-only, custom port.

### 2. Set up the firewall

Follow [infra/firewall.md](../infra/firewall.md).
Outcome: UFW active, custom SSH port open, fail2ban running.

### 3. Install Tailscale

Follow [infra/tailscale.md](../infra/tailscale.md).
Outcome: server is a tailnet node, ACLs applied.

### 4. Install Docker (optional, for reverse proxy only)

Follow [infra/docker.md](../infra/docker.md).
Skip this step if you are not running the Nginx reverse proxy.

### 5. Build and install watchdog

```sh
# On the server, as deploy user
git clone https://github.com/Gartner24/watchdog.git
cd watchdog
sudo make install
```

This builds `src/watchdog/`, copies the binary to `/usr/local/sbin/watchdog`, installs the systemd unit, creates `/var/log/watchdog/`, and installs the logrotate config.

### 6. Enable and start

```sh
sudo systemctl enable --now watchdog
sudo systemctl status watchdog
```

Confirm: status should be `active (running)`. Logs should appear in journald.

```sh
journalctl -u watchdog -n 20
```

## Post-deploy check

```sh
# watchdog is running
systemctl is-active watchdog

# Log file exists
ls -lh /var/log/watchdog/events.log

# UFW is active
sudo ufw status

# Tailscale is up
tailscale status
```

All four must succeed before running the demo.

## Related docs

- [run-demo.md](run-demo.md) — what to do after deploy
- [troubleshooting.md](troubleshooting.md) — if any step above fails
