#!/usr/bin/env bash
set -euo pipefail

cd "${LIVI_DEV_DIR:-$HOME/LIVI-dev}"

exec pnpm --dir scripts/tools telemetry:set \
  _repeatMs=500 \
  speedKph=87 \
  rpm=2200 \
  gear=6 \
  ambientC=16.5 \
  coolantC=89 \
  oilC=96 \
  iatC=31 \
  boostKpa=75 \
  batteryV=14.2 \
  fuelPct=58 \
  rangeKm=610 \
  consumptionLPer100Km=6.7 \
  consumptionAvgLPer100Km=7.1 \
  dpfLoadPct=42 \
  dpfRegenActive=false \
  dpfDifferentialKpa=1.8 \
  dpfTempC=310
