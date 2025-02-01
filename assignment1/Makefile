# Variables
CC = gcc
CFLAGS = -Wall -O2
BIN_DIR = bin
SRC_DIR = src

# Targets
all: $(BIN_DIR)/blackboard $(BIN_DIR)/drone_dynamics $(BIN_DIR)/keyboard_manager \
     $(BIN_DIR)/obstacles_generator $(BIN_DIR)/targets_generator $(BIN_DIR)/watchdog \
     $(BIN_DIR)/window_manager

$(BIN_DIR)/blackboard: $(SRC_DIR)/blackboard.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/drone_dynamics: $(SRC_DIR)/drone_dynamics.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/keyboard_manager: $(SRC_DIR)/keyboard_manager.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/obstacles_generator: $(SRC_DIR)/obstacles_generator.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/targets_generator: $(SRC_DIR)/targets_generator.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/watchdog: $(SRC_DIR)/watchdog.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/window_manager: $(SRC_DIR)/window_manager.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -lncurses -o $@

clean:
	rm -rf $(BIN_DIR)
