#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#define DEFAULT_MAX_CLIENTS 5
#define DEFAULT_BUFFER_SIZE 1024

#define PIPE_DIR "pipes/"
#define BLACKBOARD_PIPE_NAME PIPE_DIR "pipe_to_blackboard"
#define WATCHDOG_PIPE_NAME PIPE_DIR "pipe_to_watchdog"
#define KEYBOARD_PIPE_NAME PIPE_DIR "pipe_to_keyboard"
#define DRONE_PIPE_NAME PIPE_DIR "pipe_to_drone"
#define OBSTACLES_PIPE_NAME PIPE_DIR "pipe_to_obstacles"
#define TARGETS_PIPE_NAME PIPE_DIR "pipe_to_targets"

// Define FIFO capacities for targets and obstacles
#define MAX_TARGETS 50
#define MAX_OBSTACLES 50

// Blackboard structure holds drone data and FIFO queues for targets and obstacles.
typedef struct {
    int drone_x;
    int drone_y;
    // Each target: [0]: x, [1]: y
    int targets[MAX_TARGETS][2];
    int target_head;  // index of the oldest target
    int target_tail;  // index where next target will be enqueued
    // Each obstacle: [0]: x, [1]: y
    int obstacles[MAX_OBSTACLES][2];
    int obstacle_head;
    int obstacle_tail;

    int target_x;
    int target_y;
    int obstacle_x;
    int obstacle_y;
} Blackboard;

// Function to read parameters from a file.
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

// Enqueue a target into the FIFO queue.
void enqueue_target(Blackboard *bb, int tx, int ty) {
    bb->targets[bb->target_tail][0] = tx;
    bb->targets[bb->target_tail][1] = ty;
    bb->target_tail = (bb->target_tail + 1) % MAX_TARGETS;
    // If the queue is full, advance head to overwrite oldest element.
    if (bb->target_tail == bb->target_head) {
        bb->target_head = (bb->target_head + 1) % MAX_TARGETS;
    }
}

// Enqueue an obstacle into the FIFO queue.
void enqueue_obstacle(Blackboard *bb, int ox, int oy) {
    bb->obstacles[bb->obstacle_tail][0] = ox;
    bb->obstacles[bb->obstacle_tail][1] = oy;
    bb->obstacle_tail = (bb->obstacle_tail + 1) % MAX_OBSTACLES;
    if (bb->obstacle_tail == bb->obstacle_head) {
        bb->obstacle_head = (bb->obstacle_head + 1) % MAX_OBSTACLES;
    }
}

// handle_client processes incoming messages and updates the Blackboard accordingly.
void handle_client(int fd, char *buffer, Blackboard *bb) {
    ssize_t bytes_read = read(fd, buffer, 1023);
    if (bytes_read <= 0) return; // error or EOF
    buffer[bytes_read] = '\0';

    if (strncmp(buffer, "OBSTACLE,", 9) == 0) {
        int ox, oy;
        sscanf(buffer, "OBSTACLE,%d,%d", &ox, &oy);
        enqueue_obstacle(bb, ox, oy);
    } else if (strncmp(buffer, "TARGET,", 7) == 0) {
        int tx, ty;
        sscanf(buffer, "TARGET,%d,%d", &tx, &ty);
        enqueue_target(bb, tx, ty);
    } else if (strncmp(buffer, "DRONE:", 6) == 0) {
        int x, y;
        // Expected format: "DRONE: (x,y)"
        if (sscanf(buffer, "DRONE: (%d,%d)", &x, &y) == 2) {
            bb->drone_x = x;
            bb->drone_y = y;
        }
    } else if (strcmp(buffer, "UPDATE_DRONE") == 0) {
        bb->drone_x += 1;
        bb->drone_y += 1;
    }
}

// write_to_pipe sends the current state of the Blackboard via the BLACKBOARD_PIPE_NAME.
// It outputs the drone position, followed by the FIFO contents for targets and obstacles.
void write_to_pipe(int pipe_fd, Blackboard *bb) {
    char buffer[1024];
    char target_str[512] = "";
    char obstacle_str[512] = "";
    int i = bb->target_head;
    while (i != bb->target_tail) {
        char temp[32];
        snprintf(temp, sizeof(temp), "(%d,%d)", bb->targets[i][0], bb->targets[i][1]);
        strcat(target_str, temp);
        i = (i + 1) % MAX_TARGETS;
        if (i != bb->target_tail) strcat(target_str, ",");
    }
    i = bb->obstacle_head;
    while (i != bb->obstacle_tail) {
        char temp[32];
        snprintf(temp, sizeof(temp), "(%d,%d)", bb->obstacles[i][0], bb->obstacles[i][1]);
        strcat(obstacle_str, temp);
        i = (i + 1) % MAX_OBSTACLES;
        if (i != bb->obstacle_tail) strcat(obstacle_str, ",");
    }
    snprintf(buffer, sizeof(buffer),
             "Drone: (%d, %d), Targets: [%s], Obstacles: [%s]",
             bb->drone_x, bb->drone_y, target_str, obstacle_str);

    ssize_t bytes_written = write(pipe_fd, buffer, strlen(buffer) + 1);
    if (bytes_written == -1) {
        perror("Failed to write to pipe");
    } else if (bytes_written != (ssize_t)(strlen(buffer) + 1)) {
        fprintf(stderr, "Warning: Not all data written to pipe (%ld/%lu bytes)\n",
                bytes_written, strlen(buffer) + 1);
    }
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <parameters_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int wd_fd = open(WATCHDOG_PIPE_NAME, O_WRONLY | O_NONBLOCK);
    if (wd_fd == -1) {
        perror("Failed to open watchdog pipe in blackboard");
    }

    int max_clients, buffer_size;
    double drone_mass, viscous_coeff;
    read_parameters(argv[1], &max_clients, &buffer_size, &drone_mass, &viscous_coeff);

    // Initialize Blackboard.
    Blackboard bb;
    bb.drone_x = 0;
    bb.drone_y = 0;
    bb.target_x = 0;    // (Not used in FIFO implementation)
    bb.target_y = 0;
    bb.obstacle_x = 0;  // (Not used in FIFO implementation)
    bb.obstacle_y = 0;
    bb.target_head = 0;
    bb.target_tail = 0;
    bb.obstacle_head = 0;
    bb.obstacle_tail = 0;

    // List of named pipes to read from.
    const char *pipe_names[] = {
        KEYBOARD_PIPE_NAME,
        DRONE_PIPE_NAME,
        OBSTACLES_PIPE_NAME,
        TARGETS_PIPE_NAME
    };
    int num_pipes = sizeof(pipe_names) / sizeof(pipe_names[0]);

    // Open named pipes for reading.
    int pipe_fds[num_pipes];
    for (int i = 0; i < num_pipes; i++) {
        pipe_fds[i] = open(pipe_names[i], O_RDONLY | O_NONBLOCK);
        if (pipe_fds[i] == -1) {
            perror("Failed to open one of the named pipes for reading");
            // Depending on design, you might choose to exit here.
        }
    }

    // Open dummy writer for TARGETS_PIPE_NAME.
    int dummy_targets_fd = open(TARGETS_PIPE_NAME, O_WRONLY | O_NONBLOCK);
    if (dummy_targets_fd == -1) {
        perror("Failed to open dummy writer for TARGETS_PIPE_NAME");
    }
    // Open dummy writer for OBSTACLES_PIPE_NAME.
    int dummy_obstacles_fd = open(OBSTACLES_PIPE_NAME, O_WRONLY | O_NONBLOCK);
    if (dummy_obstacles_fd == -1) {
        perror("Failed to open dummy writer for OBSTACLES_PIPE_NAME");
    }

    // Open the blackboard pipe for writing (to send updates).
    int write_pipe_fd = open(BLACKBOARD_PIPE_NAME, O_WRONLY);
    if (write_pipe_fd == -1) {
        perror("Failed to open pipe_to_blackboard for writing");
        exit(EXIT_FAILURE);
    }

    fd_set read_fds_set;
    char buffer[buffer_size];

    while (1) {
        FD_ZERO(&read_fds_set);
        int max_fd = 0;

        // Add all reading pipe file descriptors to the set.
        for (int i = 0; i < num_pipes; i++) {
            if (pipe_fds[i] != -1) {
                FD_SET(pipe_fds[i], &read_fds_set);
                if (pipe_fds[i] > max_fd) {
                    max_fd = pipe_fds[i];
                }
            }
        }

        // Wait indefinitely for data.
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

        // Write the current blackboard state to the named pipe.
        write_to_pipe(write_pipe_fd, &bb);

        // Notify Watchdog.
        if (wd_fd != -1 && fcntl(wd_fd, F_GETFL) != -1) {
            char msg[64];
            snprintf(msg, sizeof(msg), "HEARTBEAT,blackboard,%d", getpid());
            ssize_t ret = write(wd_fd, msg, strlen(msg) + 1);
            if (ret < 0) {
                perror("Failed to write to watchdog pipe");
                close(wd_fd);
                wd_fd = open(WATCHDOG_PIPE_NAME, O_WRONLY | O_NONBLOCK);
                if (wd_fd == -1) {
                    perror("Failed to reopen watchdog pipe");
                }
            }
        }
    }

    // Close all file descriptors.
    for (int i = 0; i < num_pipes; i++) {
        if (pipe_fds[i] != -1) {
            close(pipe_fds[i]);
        }
    }
    close(write_pipe_fd);
    if (dummy_targets_fd != -1) close(dummy_targets_fd);
    if (dummy_obstacles_fd != -1) close(dummy_obstacles_fd);
    if (wd_fd != -1) close(wd_fd);

    return 0;
}
