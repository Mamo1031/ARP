# ARP 1st Assignment

This project is a drone operation interactive simulator implemented with multiple processes communicating by exchanging data through shared memory or pipes, and they all operate as child processes of a central main process.


## Directory Structure
```
assignment/
├── bin/
│   ├── drone
│   ├── keyboard_manager
│   ├── main
│   ├── map_window
│   ├── obstacle
│   ├── server
│   ├── target
│   └── watchdog
├── config/
│   └── config.json
├── include/
│   ├── cJSON.h
│   └── utils.h
├── logs/
│   ├── debug.log
│   └── errors.log
├── src/
│   ├── drone.c
│   ├── keybrd_manager.c
│   ├── main.c
│   ├── map_window.c
│   ├── obstacles.c
│   ├── server.c
│   ├── target.c
│   └── watchdog.c
├── .gitignore
├── Makefile
└── README.md
```

- **`bin/`**: Compiled executables.
- **`config/config.json`**: Configuration parameters.
- **`include/`**: Header files 
- **`logs/`**: Log files.
- **`src/`**: Source code for each component.
- **`Makefile`**: Builds all components into `bin/`.


## Architecture Sketch
[Architecture Sketch PDF](images/assignment1.pdf)

- **main.c**: handles process management, inter-process communication, and overall system initialization.
   - Allows Inter-Process Communication. Creates multiple pipes for communication between processes that are the "server", "drone", "obstacles", and "targets". This allows the components to exchange information during execution.
   - Uses the fork() system call to create these processes and execvp() to execute their respective binaries. Those processes are the server, the drone, the obstacles, the targets and the keyboard manager.
   - A separate process that monitors the state of the child processes is created to ensure that the system is operating correctly and handles failures if any child process stops unexpectedly.
   - The component uses semaphores to synchronize process execution, ensuring that each process starts in the correct order.
   - Logs errors to debug.log and errors.log and exits gracefully if any errors occur during file or pipe creation, process launching, or execution.

   - PRIMITIVES: 
      - fork() is used to create new processes;
      - IPC is handled using pipes;
      - Semaphores are used to synchronize the process with sem_open(), sem_wait(), and sem_close();
      - fopen(), fclose(), and fread() are used to handle file input/output operations for configuration and logs;
      - wait() is used to wait for child processes to terminate;
   - ALGORITHMS:
      - JSON Parsing: to extract values from a configuration file for simulation parameters (e.g., obstacles, targets, initial drone positions).


- **server.c**:
   - server function - Main server loop that relays data between processes. This function monitors several file descriptors (using select) for incoming data from the map, input, obstacle, and target processes. When data is available, it forwards the information to the appropriate destination file descriptors.
   - get_konsole_child - Retrieves the PID of a child process running under a terminal. This function executes a "ps" command to list the child processes of the given terminal PID.
   - signal_handler - Handles signals received by the server process. For SIGUSR1, it updates the watchdog PID and forwards the signal back. For SIGUSR2, it performs a shutdown sequence including killing the map process, unlinking shared memory and semaphores, closing log files, and exiting.
   - create_drone_shared_memory - Creates and maps the shared memory for the drone. This function creates the shared memory segment for the Drone structure, sets its size, and maps it into the process's address space.
   - create_score_shared_memory - Creates and maps the shared memory for the score. This function creates the shared memory segment for the score (a float), sets its size, initializes it to 0, and maps it into the process's address space.
   - send_signal_generation_thread - Thread routine for periodically sending signals. This thread waits for 15 seconds, then sends a SIGTERM signal to the target and obstacle processes (if their PIDs are valid). It repeats this cycle indefinitely.
   - get_pid_by_command - Retrieves the PID of a process based on its command name. This function uses "ps aux" piped to grep to find a process by name, then parses the output to extract its PID.
   - main - Entry point for the server process. The server process is responsible for relaying data between various processes (drone, input, map, obstacle, and target processes) via pipes. It also creates and manages shared memory segments, semaphores, and spawns a thread to periodically send SIGTERM signals to the obstacle and target processes.

- **watchdog.c**: This program implements a watchdog process that monitors multiple child processes, ensuring they are responsive within a specified timeout period with SIGUSR1. If any monitored process fails to respond, the watchdog takes action by logging the issue in debug.log and errors.log and terminating all processes with SIGUSR2 or SIGTERM.
   - PRIMITIVES: 
      - For signals: SIGUSR1, SIGUSR2, SIGTERM;
      - For signal handling: sigaction(), sigemptyset(), sigpromask();
      - For process management: kill(), getpid();
      - For semaphores: sem_t, sem_open(), sem_post(), sem_close();
      - For file operations: fopen(), fgets(), fclose();
      For time management: time(), difftime(), strftime();
   - ALGORITHMS:
      - Timeout Handling algorithm
      - Signal Handling Algorithm
      - Watchdog Loop

- **drone.c**: defines the functions and structures related to the drone's movement and physics interactions (forces like attraction, repulsion and friction). It operates in a loop, responding to changes in the environment, such as map size, obstacle positions, and target positions, while also processing keyboard inputs to control the drone. It handles object collisions (obstacles and targets), updating the drone’s position, and responding to key presses.
   - PRIMITIVES:
      - Shared memory for the droen state and the score (shm_open, mmap);
      - Named pipes to receive map size, obstacles and targets from external processes (select());
      - Signals to change the behaviour of the drone (sigaction);
      - Threads to update the drone's position position (pthread_create() and update_drone_position_thread());
      - Memory allocation for obstacles and targets (malloc() and free());
      - Logging errors.
   - ALGORITHMS:
      - Main loop using select() for multiplexing;
      - Parsing incoming data for obstacles and targets and updating the simulation state;
      - Update of the drone position in a separate thread;
      - Handling asynchronous events with signal handlers;
      - Managing dynamic memory for obstacles and targets.
- **keyboard_manager.c**: The Keyboard Manager captures user input, updates UI elements, and communicates with other processes via shared memory, pipes, and signals. 
   - PRIMITIVES:
      - ncurses for UI;
      - pthread for multithreading;
      - shm for IPC;
      - signals for control.
   - ALGORITHMS:
      - Reading keyboard input;
      - Creating and filling UI windows for visual feedback;
      - Update the drone position;
      - Signal handling.

- **map_window.c**: visualizes the simulation environment with targets, obstacles and the drone in the terminal using ncurses, while handling dynamic window resizing and real-time updates of the positions. It retrieves the drone's position and current score from shared memory and handles synchronization and shutdown with semaphores.
   - PRIMITIVES:
      - "game" structure (map dimensions, shared memory pointer for drone state and score, number of obstacles and targets);
      - Display functions;
      - IPC communication;
      - Signal handling.
   - ALGORITHMS:
      - Initialzation of log files, ncurses, shared memory and semaphores;
      - Update the data of drone, obstacles and targets;
      - Resizing of display windows and cleanup.

- **obstacle.c**:
- **target.c**:


## Prerequisites
- **Konsole**: Ensure that Konsole is installed on your system.
   ```bash
   sudo apt install konsole
   ```
- **NCurses**: Ensure that NCurses is installed on your system.
   ```bash
   sudo apt install libncurses-dev
   ```
- **cJSON library**: Ensure that cJSON is installed on your system.
   ```bash
   sudo apt install libcjson-dev
   ```
- **libbsd library**: Ensure that libbsd is installed on your system.
   ```bash
   sudo apt install libbsd-dev
   ```


## Building
1. Ensure you have `gcc` and `make` installed.
2. In the `assignment1_new/` directory, run:
   ```bash
   make
   ```
    This will compile all .c files under src/ into executables under bin/.


## Running
Run :
```bash
./bin/main
```

## Operational instructions
- After running, press `s` to start the simulation or `q` to quit.
- When starting the simulation, be sure to click on the window of `Input Display`, then command the drone with your keyboard.
```
\  ^  /                                       E  R  T
<  F  >          with the following keys       D  F  G
/  v  \                                         C  V  B
```
- Press the keys around the letter `F` to increase the force of command of the drone in this direction (`e/r/t/g/b/v/c/d`). _Compatible for keyboards of type QWERTY, AZERTY and QWERTZ._ The drone will be represented by an orange cross (`"+"`).
- Press `F` to remove command forces.
- Press `Q` to close.

- Obstacles (red `"O"`) and geofences on the borders will repulse the drone.
- Targets (green `"T"`) will attract the drone.
