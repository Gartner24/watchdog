# Firewall

> Adapted from [vps-setup/vps-setup.md](https://github.com/Gartner24/vps-setup).

## UFW rules for this project

```sh
sudo ufw default deny incoming
sudo ufw default allow outgoing

# Allow the custom SSH port (change 2222 to your port)
sudo ufw allow 2222/tcp comment "SSH"

# Allow Tailscale (UDP 41641 is the WireGuard port Tailscale uses)
sudo ufw allow 41641/udp comment "Tailscale"

# If using the reverse proxy, allow HTTPS and HTTP
sudo ufw allow 80/tcp comment "HTTP (redirect only)"
sudo ufw allow 443/tcp comment "HTTPS"

sudo ufw enable
sudo ufw status verbose
```

## fail2ban for SSH brute-force

Install:

```sh
sudo apt install -y fail2ban
```

Create `/etc/fail2ban/jail.local`:

```ini
[DEFAULT]
bantime  = 1h
findtime = 10m
maxretry = 5

[sshd]
enabled = true
port    = 2222
logpath = %(sshd_log)s
backend = %(sshd_backend)s
```

Enable and start:

```sh
sudo systemctl enable --now fail2ban
sudo fail2ban-client status sshd
```

## Verify UFW is active

```sh
sudo ufw status
# Status: active
```

Attempt SSH on port 22 (the old default) from another device — it must time out.

## Related docs

- [server-hardening.md](server-hardening.md) — sets the SSH port that UFW must allow
- [tailscale.md](tailscale.md) — Tailscale's own firewall behavior (it bypasses UFW for tailnet traffic)
