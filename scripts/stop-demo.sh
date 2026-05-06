#!/usr/bin/env bash
set -euo pipefail
echo "[demo] stopping watchdog"
sudo systemctl stop watchdog
echo "[demo] done"
