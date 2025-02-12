# Compiler, source, include, binary, and logs directory settings
CC   = gcc
SRC  = src
INC  = include
BIN  = bin

.PHONY: all clean

# The 'all' target compiles all executables
all: $(BIN) main server drone watchdog keyboard_manager map_window obstacle target

# Create the bin directory if it does not exist
$(BIN):
	mkdir -p $(BIN)

main: $(SRC)/main.c
	$(CC) -I$(INC) -o $(BIN)/main $(SRC)/main.c -lcjson && \
		echo "Compilation of main.c completed successfully" || \
		echo "Error compiling main.c"

server: $(SRC)/server.c
	$(CC) -I$(INC) -o $(BIN)/server $(SRC)/server.c && \
		echo "Compilation of server.c completed successfully" || \
		echo "Error compiling server.c"

drone: $(SRC)/drone.c
	$(CC) -I$(INC) -o $(BIN)/drone $(SRC)/drone.c -lm && \
		echo "Compilation of drone.c completed successfully" || \
		echo "Error compiling drone.c"

watchdog: $(SRC)/watchdog.c
	$(CC) -I$(INC) -o $(BIN)/watchdog $(SRC)/watchdog.c && \
		echo "Compilation of watchdog.c completed successfully" || \
		echo "Error compiling watchdog.c"

keyboard_manager: $(SRC)/keyboard_manager.c
	$(CC) -I$(INC) -o $(BIN)/keyboard_manager $(SRC)/keyboard_manager.c -lncurses && \
		echo "Compilation of keyboard_manager.c completed successfully" || \
		echo "Error compiling keyboard_manager.c"

map_window: $(SRC)/map_window.c
	$(CC) -I$(INC) -o $(BIN)/map_window $(SRC)/map_window.c -lncurses && \
		echo "Compilation of map_window.c completed successfully" || \
		echo "Error compiling map_window.c"

obstacle: $(SRC)/obstacle.c
	$(CC) -I$(INC) -o $(BIN)/obstacle $(SRC)/obstacle.c -lbsd && \
		echo "Compilation of obstacle.c completed successfully" || \
		echo "Error compiling obstacle.c"

target: $(SRC)/target.c
	$(CC) -I$(INC) -o $(BIN)/target $(SRC)/target.c -lbsd && \
		echo "Compilation of target.c completed successfully" || \
		echo "Error compiling target.c"

# Cleanup target to remove generated binaries and logs
clean:
	rm -rf $(BIN)
