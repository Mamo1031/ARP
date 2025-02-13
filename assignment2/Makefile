# Compiler, source, include, binary, and logs directory settings
CC   = gcc
CXX  = g++
SRC  = src
INC  = include
BIN  = bin

FAST_INCLUDE = /usr/local/include
GEN  = include/Generated

LIB_DIR = /usr/local/lib
LIBS = -lfastdds -lfastcdr

GEN_SRC = $(GEN)/ObjectsPubSubTypes.cxx

.PHONY: all clean logs

# The 'all' target compiles all executables
all: logs $(BIN) main drone watchdog keyboard_manager map_window obstacle_pub server_sub target_pub

# Create the bin directory if it does not exist
$(BIN):
	mkdir -p $(BIN)

logs:
	mkdir -p logs

main: $(SRC)/main.c
	$(CC) -I$(INC) -o $(BIN)/main $(SRC)/main.c -lcjson -lrt -pthread && \
		echo "Compilation of main.c completed successfully" || \
		echo "Error compiling main.c"

drone: $(SRC)/drone.c
	$(CC) -I$(INC) -o $(BIN)/drone $(SRC)/drone.c -lm -lrt -pthread && \
		echo "Compilation of drone.c completed successfully" || \
		echo "Error compiling drone.c"

watchdog: $(SRC)/watchdog.c
	$(CC) -I$(INC) -o $(BIN)/watchdog $(SRC)/watchdog.c -lrt -pthread && \
		echo "Compilation of watchdog.c completed successfully" || \
		echo "Error compiling watchdog.c"

keyboard_manager: $(SRC)/keyboard_manager.c
	$(CC) -I$(INC) -o $(BIN)/keyboard_manager $(SRC)/keyboard_manager.c -lncurses -lrt -pthread && \
		echo "Compilation of keyboard_manager.c completed successfully" || \
		echo "Error compiling keyboard_manager.c"

map_window: $(SRC)/map_window.c
	$(CC) -I$(INC) -o $(BIN)/map_window $(SRC)/map_window.c -lncurses -lrt -pthread && \
		echo "Compilation of map_window.c completed successfully" || \
		echo "Error compiling map_window.c"

obstacle_pub: $(SRC)/ObstaclePub.cpp $(GEN_SRC)
	$(CXX) -std=c++11 -I$(INC) -I$(FAST_INCLUDE) -L$(LIB_DIR) $(LIBS) -o $(BIN)/ObstaclePub $(SRC)/ObstaclePub.cpp $(GEN_SRC) -lrt -pthread && \
		echo "Compilation of ObstaclePub.cpp completed successfully" || \
		echo "Error compiling ObstaclePub.cpp"

server_sub: $(SRC)/ServerSub.cpp $(GEN_SRC)
	$(CXX) -std=c++11 -I$(INC) -I$(FAST_INCLUDE) -L$(LIB_DIR) $(LIBS) -o $(BIN)/ServerSub $(SRC)/ServerSub.cpp $(GEN_SRC) -lrt -pthread && \
		echo "Compilation of ServerSub.cpp completed successfully" || \
		echo "Error compiling ServerSub.cpp"

target_pub: $(SRC)/TargetPub.cpp $(GEN_SRC)
	$(CXX) -std=c++11 -I$(INC) -I$(FAST_INCLUDE) -L$(LIB_DIR) $(LIBS) -o $(BIN)/TargetPub $(SRC)/TargetPub.cpp $(GEN_SRC) -lrt -pthread && \
		echo "Compilation of TargetPub.cpp completed successfully" || \
		echo "Error compiling TargetPub.cpp"

# Cleanup target to remove generated binaries and logs
clean:
	rm -rf $(BIN) logs
