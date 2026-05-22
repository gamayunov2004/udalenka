cat > collect_mem_map.sh <<'EOF'
#!/usr/bin/env bash
set -euo pipefail

PID=${1:?"PID is required"}
MODE=${2:-unknown}
OUT_DIR="/var/log/my_mem_maps"
DATE_TIME=$(date '+%Y-%m-%d_%H:%M:%S')
OUT_FILE="$OUT_DIR/map_${PID}_${DATE_TIME}_${MODE}.txt"

sudo mkdir -p "$OUT_DIR"
pmap -x "$PID" | sudo tee "$OUT_FILE" > /dev/null
EOF
