#!/bin/bash

# LOGFILE="logs/debug.log" ## Uncomment echo for feedback
# exec >> "$LOGFILE" 2>&1

# Read the PID of the watchdog
if [ ! -f pids.txt ]; then
    echo "Error: pids.txt not found !"
    exit 1
fi

WATCHDOG_PID=$(head -n 1 pids.txt)

# Send SIGUSR2 to the watchdog to kill all the processes
# echo "[RESTART] Killing watchdog and all processes..."
kill -SIGUSR2 $WATCHDOG_PID

# Wait for all processes to exit
# echo "[RESTART] Waiting for all processes to exit..."
while pgrep -x "watchdog" || pgrep -x "server" || pgrep -x "drone" || \
      pgrep -x "obstacle" || pgrep -x "target" || pgrep -x "input"; do
    sleep 0.5
done

# echo "[RESTART] All processes terminated."

# Start over the main
# echo "[START OVER] Starting over..."
wmctrl -c "Konsole"
konsole -e bash -c "./bin/main; exec bash"

# Attendre un peu pour que les PIDs soient écrits
sleep 2
