#!/usr/bin/env bash
set -euo pipefail

CUSTOM_DIR="${LIVI_CUSTOM_DIR:-$HOME/.config/LIVI/custom}"
THEME_DIR="${LIVI_THEME_DIR:-$CUSTOM_DIR/themes}"
ACTIVE_THEME="$CUSTOM_DIR/espace-theme.css"

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
  theme-switch.sh off

The selected CSS is copied atomically to:
  ~/.config/LIVI/custom/espace-theme.css

With the Espace hot-reload patch installed, LIVI applies the change in about one second without restart.
EOF
}

mkdir -p "$CUSTOM_DIR" "$THEME_DIR"

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
    rm -f "$ACTIVE_THEME"
    echo "Theme override disabled. Base espace-ui.css remains active."
    ;;

  -h|--help|help|"")
    usage
    ;;

  *)
    source_file="$THEME_DIR/$cmd.css"
    if [[ ! -f "$source_file" ]]; then
      echo "Unknown theme: $cmd" >&2
      echo "Available themes:" >&2
      "$0" list >&2 || true
      exit 1
    fi

    tmp_file="$(mktemp "$CUSTOM_DIR/.espace-theme.css.XXXXXX")"
    trap 'rm -f "$tmp_file"' EXIT
    cat "$source_file" > "$tmp_file"
    chmod 0644 "$tmp_file"
    mv -f "$tmp_file" "$ACTIVE_THEME"
    trap - EXIT

    echo "Theme activated: $cmd"
    ;;
esac
