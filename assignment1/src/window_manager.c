#include <ncurses.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define PIPE_DIR "pipes/"
#define PIPE_NAME PIPE_DIR "pipe_to_blackboard"

// Signal handler to clean up ncurses mode on Ctrl+C
void cleanup(int signo) {
    endwin(); // End ncurses mode
    exit(0);
}


void display_map(int pipe_fd) {
    char buffer[256];
    int drone_x = 0, drone_y = 0;
    int target_x = 0, target_y = 0;
    int obstacle_x = 0, obstacle_y = 0;

    const int MAP_W = 60;
    const int MAP_H = 30;

    while (1) {
        // Receive the latest data from Blackboard
        ssize_t bytes_read = read(pipe_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            break;
        }
        buffer[bytes_read] = '\0';

        sscanf(buffer, "Drone: (%d, %d), Target: (%d, %d), Obstacle: (%d, %d)",
               &drone_x, &drone_y, &target_x, &target_y, &obstacle_x, &obstacle_y);

        // ASCII map creation
        char map[MAP_H][MAP_W];
        for(int r=0; r<MAP_H; r++){
            for(int c=0; c<MAP_W; c++){
                map[r][c] = ' ';
            }
        }

        // Check coordinates and draw
        if (drone_x >= 0 && drone_x < MAP_W && drone_y >= 0 && drone_y < MAP_H) {
            map[drone_y][drone_x] = '+';
        }
        if (target_x >= 0 && target_x < MAP_W && target_y >= 0 && target_y < MAP_H) {
            map[target_y][target_x] = 'T';
        }
        if (obstacle_x >= 0 && obstacle_x < MAP_W && obstacle_y >= 0 && obstacle_y < MAP_H) {
            map[obstacle_y][obstacle_x] = 'O';
        }

        clear();
        mvprintw(0, 0, "MAP DISPLAY");
        // Draw the contents of map[][] to the terminal
        for(int r=0; r<MAP_H; r++){
            mvprintw(r+1, 0, "%.*s", MAP_W, map[r]);
        }

        refresh();
        usleep(50000);
    }
}



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
            printw("Pipe closed. Exiting...\n");
            refresh();
            sleep(2);
            break;
        }
        buffer[bytes_read] = '\0';

        mvprintw(0, 0, "INPUT DISPLAY");
        mvprintw(1, 0, "Dynamics Data: %s", buffer);

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
