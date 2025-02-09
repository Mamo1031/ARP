#include <ncurses.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#define PIPE_DIR "pipes/"
#define PIPE_NAME PIPE_DIR "pipe_to_blackboard"

#define MAX_TARGETS 50
#define MAX_OBSTACLES 50

// Signal handler to clean up ncurses mode on Ctrl+C
void cleanup(int signo) {
    endwin(); // End ncurses mode
    exit(0);
}

// Function to display the map with multiple targets and obstacles
void display_map(int pipe_fd) {
    char buffer[1024];  // Increased buffer size to handle multiple entries
    int drone_x = 0, drone_y = 0;
    int targets[MAX_TARGETS][2];
    int num_targets = 0;
    int obstacles[MAX_OBSTACLES][2];
    int num_obstacles = 0;

    const int MAP_W = 60;
    const int MAP_H = 30;

    while (1) {
        // Receive the latest data from Blackboard
        ssize_t bytes_read = read(pipe_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            usleep(50000);
            continue;
        }
        buffer[bytes_read] = '\0';

        // Reset target and obstacle counts
        num_targets = 0;
        num_obstacles = 0;

        // Parse the received data (assuming the format: "Drone: (x, y), Targets: [(x1, y1), (x2, y2), ...], Obstacles: [(x1, y1), (x2, y2), ...]")
        char targets_str[512] = "", obstacles_str[512] = "";
        sscanf(buffer, "Drone: (%d, %d), Targets: [%511[^]]], Obstacles: [%511[^]]]",
               &drone_x, &drone_y, targets_str, obstacles_str);

        // Parse the targets list
        char *token = strtok(targets_str, "(),");
        while (token != NULL && num_targets < MAX_TARGETS) {
            targets[num_targets][0] = atoi(token);
            token = strtok(NULL, "(),");
            if (token == NULL) break;
            targets[num_targets][1] = atoi(token);
            token = strtok(NULL, "(),");
            num_targets++;
        }

        // Parse the obstacles list
        token = strtok(obstacles_str, "(),");
        while (token != NULL && num_obstacles < MAX_OBSTACLES) {
            obstacles[num_obstacles][0] = atoi(token);
            token = strtok(NULL, "(),");
            if (token == NULL) break;
            obstacles[num_obstacles][1] = atoi(token);
            token = strtok(NULL, "(),");
            num_obstacles++;
        }

        // ASCII map creation
        char map[MAP_H][MAP_W];
        for (int r = 0; r < MAP_H; r++) {
            for (int c = 0; c < MAP_W; c++) {
                map[r][c] = ' ';
            }
        }

        // Place drone on the map
        if (drone_x >= 0 && drone_x < MAP_W && drone_y >= 0 && drone_y < MAP_H) {
            map[drone_y][drone_x] = '+';
        }

        // Place targets on the map
        for (int i = 0; i < num_targets; i++) {
            int tx = targets[i][0];
            int ty = targets[i][1];
            if (tx >= 0 && tx < MAP_W && ty >= 0 && ty < MAP_H) {
                map[ty][tx] = 'T';
            }
        }

        // Place obstacles on the map
        for (int i = 0; i < num_obstacles; i++) {
            int ox = obstacles[i][0];
            int oy = obstacles[i][1];
            if (ox >= 0 && ox < MAP_W && oy >= 0 && oy < MAP_H) {
                map[oy][ox] = 'O';
            }
        }

        clear();
        mvprintw(0, 0, "MAP DISPLAY");
        // Draw the contents of map[][] to the terminal
        for (int r = 0; r < MAP_H; r++) {
            mvprintw(r + 1, 0, "%.*s", MAP_W, map[r]);
        }

        refresh();
        usleep(50000);
    }
}

// Function to display input dynamics
void display_input_dynamics(int pipe_fd) {
    char buffer[256];

    while (1) {
        clear();

        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_read = read(pipe_fd, buffer, sizeof(buffer) - 1);

        if (bytes_read == -1) {
            perror("Error reading from pipe");
            continue;
        } else if (bytes_read == 0) {
            printw("Pipe closed. Waiting for data...\n");
            refresh();
            usleep(50000);
            continue;
        }
        buffer[bytes_read] = '\0';

        mvprintw(0, 0, "INPUT DISPLAY");
        
        for (int i=2;i<14;i++)
		{
			mvprintw(2, i, "_");
			mvprintw(5, i, "_");
			mvprintw(8, i, "_");
			mvprintw(11, i, "_");
		}
		mvprintw(2, 2, ".");
		mvprintw(2, 6, ".");
		mvprintw(2, 10, ".");
		mvprintw(2, 14, ".");
		for (int j=3; j<12; j++)
		{
			mvprintw(j, 2, "|");
			mvprintw(j, 6, "|");
			mvprintw(j, 10, "|");
			mvprintw(j, 14, "|");
		}
		mvprintw(3, 4, "_"); // Top Left
		mvprintw(4, 3, "'");
		mvprintw(4, 4, "\\");
		mvprintw(3, 8, "A"); // Top
		mvprintw(4, 8, "|");
		mvprintw(3, 12, "_"); // Top Right
		mvprintw(4, 12, "/");
		mvprintw(4, 13, "'");
		mvprintw(7, 12, "-"); // Right
		mvprintw(7, 13, ">");
		mvprintw(10, 12, "\\"); // Bottom Right
		mvprintw(10, 13, "|");
		mvprintw(11, 12, "'");
		mvprintw(10, 8, "|"); // Bottom
		mvprintw(11, 8, "V");
		mvprintw(10, 4, "/"); // Bottom Left
		mvprintw(10, 3, "|");
		mvprintw(11, 4, "'");
		mvprintw(7, 4, "-"); // Left
		mvprintw(7, 3, "<");
		mvprintw(7, 8, "@"); 
		
	for (int i=2;i<14;i++)
		{
			mvprintw(13, i, "_");
			mvprintw(16, i, "_");
			mvprintw(19, i, "_");
			mvprintw(22, i, "_");
		}
		mvprintw(13, 2, ".");
		mvprintw(13, 6, ".");
		mvprintw(13, 10, ".");
		mvprintw(13, 14, ".");
		for (int j=14; j<23; j++)
		{
			mvprintw(j, 2, "|");
			mvprintw(j, 6, "|");
			mvprintw(j, 10, "|");
			mvprintw(j, 14, "|");
		}
		mvprintw(15, 4, "E"); // Top Left
		mvprintw(15, 8, "R"); // Top
		mvprintw(15, 12, "T"); // Top Right
		mvprintw(18, 12, "G"); // Right
		mvprintw(21, 12, "B"); // Bottom Right
		mvprintw(21, 8, "V"); // Bottom
		mvprintw(21, 4, "C"); // Bottom Left
		mvprintw(18, 4, "D"); // Left
		mvprintw(18, 8, "F"); // On/Off

        refresh();
        usleep(50000);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [map|input]\n", argv[0]);
        return 1;
    }

    int pipe_fd = open(PIPE_NAME, O_RDONLY);
    if (pipe_fd == -1) {
        perror("Failed to open pipe");
        return 1;
    }

    signal(SIGINT, cleanup);
    initscr();
    noecho();
    curs_set(FALSE);

    if (strcmp(argv[1], "map") == 0) {
        display_map(pipe_fd);
    } else if (strcmp(argv[1], "input") == 0) {
        display_input_dynamics(pipe_fd);
    } else {
        fprintf(stderr, "Unknown mode: %s\n", argv[1]);
        endwin();
        return 1;
    }

    close(pipe_fd);
    endwin();
    return 0;
}
