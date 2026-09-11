#!/usr/bin/env bash
set -euo pipefail

CUSTOM_DIR="${LIVI_CUSTOM_DIR:-$HOME/.config/LIVI/custom}"
THEME_DIR="${LIVI_THEME_DIR:-$CUSTOM_DIR/themes}"
ACTIVE_THEME="$CUSTOM_DIR/espace-theme.css"
ACTIVE_GAUGES="$CUSTOM_DIR/espace-gauges.json"

usage() {
  cat <<'EOF'
Usage:
  theme-switch.sh list
  theme-switch.sh <theme>
  theme-switch.sh off

Examples:
  theme-switch.sh list
  theme-switch.sh espace-oem
  theme-switch.sh espace-sport
  theme-switch.sh espace-retro
  theme-switch.sh espace-minimal
  theme-switch.sh off

The selected preset can contain two files:
  <theme>.css   -> ~/.config/LIVI/custom/espace-theme.css
  <theme>.json  -> ~/.config/LIVI/custom/espace-gauges.json

With the Espace hot-reload patches installed, LIVI applies palette, gauge renderer,
needle style and geometry changes in about one second without restart.
EOF
}

mkdir -p "$CUSTOM_DIR" "$THEME_DIR"

copy_atomic() {
  local source_file="$1"
  local destination_file="$2"
  local tmp_file

  tmp_file="$(mktemp "$CUSTOM_DIR/.theme-switch.XXXXXX")"
  trap 'rm -f "$tmp_file"' RETURN
  cat "$source_file" > "$tmp_file"
  chmod 0644 "$tmp_file"
  mv -f "$tmp_file" "$destination_file"
  trap - RETURN
}

validate_json() {
  local source_file="$1"
  if command -v python3 >/dev/null 2>&1; then
    python3 -m json.tool "$source_file" >/dev/null
  fi
}

cmd="${1:-}"
case "$cmd" in
  list)
    found=0
    shopt -s nullglob
    for file in "$THEME_DIR"/*.css; do
      basename "$file" .css
      found=1
    done
    shopt -u nullglob
    if [[ "$found" -eq 0 ]]; then
      echo "No themes found in $THEME_DIR" >&2
      exit 1
    fi
    ;;

  off)
    rm -f "$ACTIVE_THEME" "$ACTIVE_GAUGES"
    echo "Theme override disabled. Base Espace UI and compiled gauge defaults remain active."
    ;;

  -h|--help|help|"")
    usage
    ;;

  *)
    source_css="$THEME_DIR/$cmd.css"
    source_gauges="$THEME_DIR/$cmd.json"

    if [[ ! -f "$source_css" ]]; then
      echo "Unknown theme: $cmd" >&2
      echo "Available themes:" >&2
      "$0" list >&2 || true
      exit 1
    fi

    if [[ -f "$source_gauges" ]]; then
      if ! validate_json "$source_gauges"; then
        echo "Invalid gauge JSON: $source_gauges" >&2
        exit 1
      fi
    fi

    copy_atomic "$source_css" "$ACTIVE_THEME"

    if [[ -f "$source_gauges" ]]; then
      copy_atomic "$source_gauges" "$ACTIVE_GAUGES"
      echo "Theme activated: $cmd (CSS + gauge renderer)"
    else
      rm -f "$ACTIVE_GAUGES"
      echo "Theme activated: $cmd (CSS only; gauge defaults restored)"
    fi
    ;;
esac
