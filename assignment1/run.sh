#!/bin/bash

# Function to kill existing processes
kill_existing_processes() {
    pkill -f blackboard || true
    pkill -f watchdog || true
    pkill -f keyboard_manager || true
    pkill -f drone_dynamics || true
    pkill -f obstacles_generator || true
    pkill -f targets_generator || true
    pkill -f "./bin/window_manager map"
    pkill -f "./bin/window_manager input"
}

# Kill any existing related processes
kill_existing_processes

# Ensure pipes directory exists
PIPE_DIR="./pipes"
mkdir -p "$PIPE_DIR"

# Remove any existing named pipes
rm -f "$PIPE_DIR"/pipe_to_*

# Create named pipes
mkfifo "$PIPE_DIR"/pipe_to_blackboard
mkfifo "$PIPE_DIR"/pipe_to_watchdog
mkfifo "$PIPE_DIR"/pipe_to_keyboard
mkfifo "$PIPE_DIR"/pipe_to_drone
mkfifo "$PIPE_DIR"/pipe_to_obstacles
mkfifo "$PIPE_DIR"/pipe_to_targets

# Path to parameters file
PARAMETERS_FILE="./config/parameters.txt"

# Verify the parameters file exists
if [ ! -f "$PARAMETERS_FILE" ]; then
  echo "Error: Parameters file not found at $PARAMETERS_FILE"
  exit 1
fi

# Ensure logs directory exists
LOG_DIR="./logs"
mkdir -p "$LOG_DIR"

# Start other processes in the background with log redirection
./bin/blackboard "$PARAMETERS_FILE" > "$LOG_DIR/blackboard.log" 2>&1 &
BLACKBOARD_PID=$!

./bin/watchdog ./pipes/pipe_to_watchdog > "$LOG_DIR/watchdog.log" 2>&1 &
WATCHDOG_PID=$!

./bin/keyboard_manager ./pipes/pipe_to_keyboard > "$LOG_DIR/keyboard_manager.log" 2>&1 &
KEYBOARD_MANAGER_PID=$!

./bin/drone_dynamics ./pipes/pipe_to_drone > "$LOG_DIR/drone_dynamics.log" 2>&1 &
DRONE_DYNAMICS_PID=$!

./bin/obstacles_generator ./pipes/pipe_to_obstacles > "$LOG_DIR/obstacles_generator.log" 2>&1 &
OBSTACLES_GENERATOR_PID=$!

./bin/targets_generator ./pipes/pipe_to_targets > "$LOG_DIR/targets_generator.log" 2>&1 &
TARGETS_GENERATOR_PID=$!

# Start two instances of window_manager for different views in separate konsole windows
konsole -e ./bin/window_manager map & 
WINDOW_MANAGER_MAP_PID=$!

konsole -e ./bin/window_manager input & 
WINDOW_MANAGER_INPUT_PID=$!

# Wait for all processes
wait $BLACKBOARD_PID $WATCHDOG_PID $KEYBOARD_MANAGER_PID $DRONE_DYNAMICS_PID $OBSTACLES_GENERATOR_PID $TARGETS_GENERATOR_PID $WINDOW_MANAGER_MAP_PID $WINDOW_MANAGER_INPUT_PID

echo "All processes terminated."

# Cleanup named pipes
rm -f ./pipes/pipe_to_blackboard ./pipes/pipe_to_watchdog ./pipes/pipe_to_keyboard ./pipes/pipe_to_drone ./pipes/pipe_to_obstacles ./pipes/pipe_to_targets
