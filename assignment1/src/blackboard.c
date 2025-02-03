#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEFAULT_MAX_CLIENTS 5
#define DEFAULT_BUFFER_SIZE 1024

#define PIPE_DIR "pipes/"
#define BLACKBOARD_PIPE_NAME PIPE_DIR "pipe_to_blackboard"
#define WATCHDOG_PIPE_NAME PIPE_DIR "pipe_to_watchdog"
#define KEYBOARD_PIPE_NAME PIPE_DIR "pipe_to_keyboard"
#define DRONE_PIPE_NAME PIPE_DIR "pipe_to_drone"
#define OBSTACLES_PIPE_NAME PIPE_DIR "pipe_to_obstacles"
#define TARGETS_PIPE_NAME PIPE_DIR "pipe_to_targets"

typedef struct {
    int drone_x;
    int drone_y;
    int target_x;
    int target_y;
    int obstacle_x;
    int obstacle_y;
} Blackboard;

// Function to read parameters from a file
void read_parameters(const char *filename, int *max_clients, int *buffer_size, double *drone_mass, double *viscous_coeff) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open parameters file. Using default values.");
        *max_clients = DEFAULT_MAX_CLIENTS;
        *buffer_size = DEFAULT_BUFFER_SIZE;
        *drone_mass = 1.0;
        *viscous_coeff = 1.0;
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char key[128];
        double value;
        if (sscanf(line, "%[^=]=%lf", key, &value) == 2) {
            if (strcmp(key, "MAX_CLIENTS") == 0) {
                *max_clients = (int)value;
            } else if (strcmp(key, "BUFFER_SIZE") == 0) {
                *buffer_size = (int)value;
            } else if (strcmp(key, "DRONE_MASS") == 0) {
                *drone_mass = value;
            } else if (strcmp(key, "VISCOUS_COEFF") == 0) {
                *viscous_coeff = value;
            }
        }
    }
    fclose(file);
}


void handle_client(int fd, char *buffer, Blackboard *bb) {
    ssize_t bytes_read = read(fd, buffer, 1023);
    if (bytes_read <= 0) return; // error or EOF
    buffer[bytes_read] = '\0';

    if (strncmp(buffer, "OBSTACLE,", 9) == 0) {
        int ox, oy;
        sscanf(buffer, "OBSTACLE,%d,%d", &ox, &oy);
        bb->obstacle_x = ox;
        bb->obstacle_y = oy;
    } else if (strncmp(buffer, "TARGET,", 7) == 0) {
        int tx, ty;
        sscanf(buffer, "TARGET,%d,%d", &tx, &ty);
        bb->target_x = tx;
        bb->target_y = ty;
    } else if (strcmp(buffer, "UPDATE_DRONE") == 0) {
        bb->drone_x += 1;
        bb->drone_y += 1;
    }
}


void write_to_pipe(int pipe_fd, Blackboard *bb) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer),
             "Drone: (%d, %d), Target: (%d, %d), Obstacle: (%d, %d)",
             bb->drone_x, bb->drone_y,
             bb->target_x, bb->target_y,
             bb->obstacle_x, bb->obstacle_y
    );

    // write(pipe_fd, buffer, strlen(buffer) + 1);
    ssize_t bytes_written = write(pipe_fd, buffer, strlen(buffer) + 1);
    if (bytes_written == -1) {
        perror("Failed to write to pipe");
    } else if (bytes_written != (ssize_t)(strlen(buffer) + 1)) {
        fprintf(stderr, "Warning: Not all data written to pipe (%ld/%lu bytes)\n",
                bytes_written, strlen(buffer) + 1);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <parameters_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int wd_fd = open(WATCHDOG_PIPE_NAME, O_WRONLY);
    if (wd_fd == -1) {
        perror("Failed to open watchdog pipe in blackboard");
    }

    int max_clients, buffer_size;
    double drone_mass, viscous_coeff;
    read_parameters(argv[1], &max_clients, &buffer_size, &drone_mass, &viscous_coeff);

    // List of named pipes to read from
    const char *pipe_names[] = {
        KEYBOARD_PIPE_NAME,
        DRONE_PIPE_NAME,
        OBSTACLES_PIPE_NAME,
        TARGETS_PIPE_NAME
    };
    int num_pipes = sizeof(pipe_names) / sizeof(pipe_names[0]);

    // Open named pipes for reading
    int pipe_fds[num_pipes];
    for (int i = 0; i < num_pipes; i++) {
        pipe_fds[i] = open(pipe_names[i], O_RDONLY | O_NONBLOCK);
        if (pipe_fds[i] == -1) {
            perror("Failed to open one of the named pipes for reading");
            // Proceeding without exiting might not be ideal, but depends on design
        }
    }

    // Open the named pipe for writing (to send updates)
    int write_pipe_fd = open(BLACKBOARD_PIPE_NAME, O_WRONLY);
    if (write_pipe_fd == -1) {
        perror("Failed to open pipe_to_blackboard for writing");
        // Exit if unable to open the pipe for writing
        exit(EXIT_FAILURE);
    }

    fd_set read_fds_set;
    Blackboard bb = {0, 0, 0, 0, 0, 0};
    char buffer[buffer_size];

    while (1) {
        FD_ZERO(&read_fds_set);
        int max_fd = 0;

        // Add all pipe_fds to the set
        for (int i = 0; i < num_pipes; i++) {
            if (pipe_fds[i] != -1) {
                FD_SET(pipe_fds[i], &read_fds_set);
                if (pipe_fds[i] > max_fd) {
                    max_fd = pipe_fds[i];
                }
            }
        }

        // Set timeout (optional, here it's NULL meaning wait indefinitely)
        int activity = select(max_fd + 1, &read_fds_set, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Select error");
            break;
        }

        if (activity > 0) {
            for (int i = 0; i < num_pipes; i++) {
                if (pipe_fds[i] != -1 && FD_ISSET(pipe_fds[i], &read_fds_set)) {
                    handle_client(pipe_fds[i], buffer, &bb);
                }
            }
        }

        // Write the current blackboard state to the named pipe
        write_to_pipe(write_pipe_fd, &bb);

        // Notify Watchdog
        if (wd_fd != -1) {
            char msg[64];
            snprintf(msg, sizeof(msg), "HEARTBEAT,blackboard,%d", getpid());

            ssize_t ret = write(wd_fd, msg, strlen(msg)+1);
            if(ret < 0) {
                perror("Failed to write");
            } else if(ret < strlen(msg)+1) {
                fprintf(stderr, "Partial write: %ld/%ld bytes\n", (long)ret, (long)strlen(msg)+1);
            }
        }
    }

    // Close all pipe file descriptors
    for (int i = 0; i < num_pipes; i++) {
        if (pipe_fds[i] != -1) {
            close(pipe_fds[i]);
        }
    }
    close(write_pipe_fd);

    return 0;
}
