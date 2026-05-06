# systemd unit

The `watchdog` daemon runs as a systemd service. The unit file lives at `deploy/systemd/watchdog.service` and is installed to `/etc/systemd/system/watchdog.service` by `deploy/install.sh`.

## Unit file overview

```ini
[Unit]
Description=watchdog surveillance daemon
After=network.target

[Service]
Type=simple
User=root
ExecStart=/usr/local/sbin/watchdog
Restart=on-failure
RestartSec=5
StandardOutput=journal
StandardError=journal
SyslogIdentifier=watchdog
Environment=WD_POLL_MS=1000
Environment=WD_LOG_PATH=/var/log/watchdog/events.log
Environment=WD_LOAD_THRESHOLD=4.0
Environment=WD_MEM_FREE_MB=200

[Install]
WantedBy=multi-user.target
```

The full file is at `deploy/systemd/watchdog.service`. Edit environment variables there before installing.

## Install and enable

```sh
sudo make install
# or manually:
sudo cp deploy/systemd/watchdog.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now watchdog
```

## Manage

```sh
sudo systemctl status watchdog
sudo systemctl restart watchdog
sudo systemctl stop watchdog
```

## View logs

```sh
# Stream live
journalctl -u watchdog -f

# Last 100 lines
journalctl -u watchdog -n 100

# Since boot
journalctl -u watchdog -b
```

## Related docs

- [logging.md](../modules/watchdog/logging.md) — log format and rotation
- [deploy.md](../operations/deploy.md) — when to run `make install` in the deploy sequence
