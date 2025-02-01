# ARP 1st Assignment


## Overview
This project is a drone operation interactive simulator implemented with multiple processes communicating via FIFOs (named pipes). It consists of:
- **Blackboard (Server)**: Manages a "blackboard" data structure of the world state (drone, targets, obstacles).
- **Drone Dynamics**: Simulates the drone's motion under forces (keyboard input, obstacle repulsion, etc.).
- **Keyboard Manager**: Captures keyboard input and sends corresponding commands (e.g., UP, DOWN, LEFT, RIGHT) to the drone or blackboard.
- **Obstacles Generator**: Periodically generates obstacles.
- **Targets Generator**: Periodically generates targets.
- **Watchdog**: Monitors other processes via heartbeat messages.

The UI is implemented with NCurses in a `window_manager` process, showing:
1. A **MAP DISPLAY** window with ASCII symbols for obstacles (O), targets (T), and the drone (+).
2. An **INPUT DISPLAY** and **DYNAMICS DISPLAY** window, showing keyboard commands and real-time drone state (position, velocity, force), respectively.


## Directory Structure
```
assignment/
├── bin/
│   ├── blackboard
│   ├── drone_dynamics
│   ├── keyboard_manager
│   ├── obstacles_generator
│   ├── targets_generator
│   ├── watchdog
│   └── window_manager
├── config/
│   └── parameters.txt
├── logs/
│   ├── blackboard.log
│   ├── drone_dynamics.log
│   ├── keyboard_manager.log
│   ├── obstacles_generator.log
│   ├── targets_generator.log
│   └── watchdog.log
├── pipes/
│   ├── pipe_to_blackboard
│   ├── pipe_to_drone
│   ├── pipe_to_keyboard
│   ├── pipe_to_obstacles
│   ├── pipe_to_targets
│   └── pipe_to_watchdog
├── src/
│   ├── blackboard.c
│   ├── drone_dynamics.c
│   ├── keyboard_manager.c
│   ├── obstacles_generator.c
│   ├── targets_generator.c
│   ├── watchdog.c
│   └── window_manager.c
├── Makefile
├── README.md
└── run.sh
```

- **`bin/`**: Compiled executables.
- **`config/parameters.txt`**: Configuration parameters (e.g., boundary size, time step, mass, etc.).
- **`logs/`**: Logs for each process.
- **`pipes/`**: Named pipes (FIFOs) for inter-process communication.
- **`src/`**: Source code for each component.
- **`Makefile`**: Builds all components into `bin/`.
- **`run.sh`**: Shell script to create FIFOs, kill any existing processes, run each component, and open the NCurses windows.


## Building
1. Ensure you have `gcc` and `make` installed.
2. In the `assignment/` directory, run:
   ```bash
   make
   ```
    This will compile all .c files under src/ into executables under bin/.


## Running
1. Make sure `run.sh` is executable:
    ```bash
   chmod +x run.sh
   ```
2. Run the script:
   ```bash
   ./run.sh
   ```
3. The script will:
- Kill any existing processes named like `blackboard`, `watchdog`, etc.
- Create the named pipes in the `pipes/` directory.
- Launch each component in the background, redirecting their logs to `./logs/*.log`.
- Open two Konsole windows for `window_manager`:
    - One with `map` mode (MAP DISPLAY).
    - One with `input` mode (INPUT and DYNAMICS DISPLAY).


## Usage
- Keyboard Manager: Press `w/a/s/d` keys to control the drone. Press `q` to quit.
- Watchdog: Automatically monitors other processes via heartbeats. If a process fails to send a heartbeat, the watchdog logs an error and exits the system.
- Drone Dynamics: Receives commands, applies forces, and updates the drone position and velocity.
- Obstacles/Targets Generators: Periodically send obstacle or target data to the blackboard.
- Blackboard: Collects all data and broadcasts the latest world state to `window_manager`.
- Window Manager:
    - MAP DISPLAY: Shows ASCII map with drone ( `+` ), obstacles ( `O` ), targets ( `T` ).
    - INPUT DISPLAY: Shows command layout or pressed keys.
    - DYNAMICS DISPLAY: Shows numeric values of position, velocity, and force.


## Logging
Each process writes its runtime logs to the `logs/` directory, e.g.:

- `blackboard.log`
- `drone_dynamics.log`
- `keyboard_manager.log`
- `obstacles_generator.log`
- `targets_generator.log`
- `watchdog.log`

Check these logs for debug information or errors.