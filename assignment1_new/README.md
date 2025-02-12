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
![Architecture Sketch](images/assignment1.pdf)

- **main.c**:
- **server.c**:
- **watchdog.c**:
- **drone.c**:
- **keyboard_manager.c**:
- **map_window.c**:
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
