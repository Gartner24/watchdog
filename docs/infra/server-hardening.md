# Server hardening

> Adapted from [vps-setup/vps-setup.md](https://github.com/Gartner24/vps-setup).

## 1. Create a non-root user

```sh
adduser deploy
usermod -aG sudo deploy
```

All subsequent steps run as `deploy` with `sudo`. Root direct login will be disabled.

## 2. SSH key authentication

From your local machine:

```sh
ssh-copy-id -i ~/.ssh/id_ed25519.pub deploy@<server-ip>
```

Test the key login works before disabling password auth.

## 3. Harden sshd_config

Edit `/etc/ssh/sshd_config`:

```
Port 2222                     # change from default 22
PermitRootLogin no
PasswordAuthentication no
PubkeyAuthentication yes
AllowUsers deploy
MaxAuthTries 3
```

Restart SSH:

```sh
sudo systemctl restart ssh
```

Open a second terminal to verify login still works before closing the first session.

## 4. Custom SSH port

Update UFW to allow the new port before restarting SSH (see [firewall.md](firewall.md)).

If using the default port 22, skip this — but the custom port reduces log noise from automated scanners.

## 5. System updates

```sh
sudo apt update && sudo apt upgrade -y
sudo apt install -y unattended-upgrades
sudo dpkg-reconfigure --priority=low unattended-upgrades
```

## Verification

```sh
# From another terminal / device
ssh -p 2222 deploy@<server-ip>        # must succeed
ssh -p 2222 root@<server-ip>          # must fail (PermitRootLogin no)
```

## Related docs

- [firewall.md](firewall.md) — UFW rules that protect the SSH port
