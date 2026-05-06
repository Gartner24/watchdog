#!/usr/bin/env bash
set -euo pipefail
echo "[demo] starting watchdog"
sudo systemctl start watchdog
echo "[demo] watchdog active. streaming logs (Ctrl+C to stop)..."
journalctl -u watchdog -f
