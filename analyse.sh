#!/bin/bash

# === Script: analyse.sh ===
# Description: Analyses memory usage of a given process ID (PID)
# Usage: ./analyse.sh <PID>

# Check if PID argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <PID>"
  exit 1
fi

PID=$1

# Check if the process exists
if [ ! -d "/proc/$PID" ]; then
  echo "Process with PID $PID does not exist."
  exit 1
fi

echo "=== Memory Analysis for PID: $PID ==="
echo

# Show memory usage with ps
echo "[1] VSZ and RSS from ps:"
ps -o pid,vsz,rss,comm -p $PID
echo

# Show memory mappings from /proc/<PID>/maps
echo "[2] Memory map from /proc/$PID/maps:"
cat /proc/$PID/maps

echo
echo "=== End of analysis ==="
