#!/usr/bin/env bash
set -euo pipefail

BINARY_SRC="src/watchdog/watchdog"
BINARY_DST="/usr/local/sbin/watchdog"
UNIT_SRC="deploy/systemd/watchdog.service"
UNIT_DST="/etc/systemd/system/watchdog.service"
LOG_DIR="/var/log/watchdog"
LOGROTATE_DST="/etc/logrotate.d/watchdog"

if [[ $EUID -ne 0 ]]; then
    echo "error: must run as root (sudo ./deploy/install.sh)" >&2
    exit 1
fi

echo "[install] copying binary to $BINARY_DST"
install -m 755 "$BINARY_SRC" "$BINARY_DST"

echo "[install] installing systemd unit"
install -m 644 "$UNIT_SRC" "$UNIT_DST"

echo "[install] creating log directory"
mkdir -p "$LOG_DIR"
chmod 750 "$LOG_DIR"

echo "[install] installing logrotate config"
cat > "$LOGROTATE_DST" << 'EOF'
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
EOF

echo "[install] reloading systemd"
systemctl daemon-reload

echo "[install] done. enable with: systemctl enable --now watchdog"
