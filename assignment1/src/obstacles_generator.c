#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Default parameters
#define DEFAULT_GEOMETRIC_BOUNDARY 100
#define DEFAULT_OBSTACLE_RADIUS 5
#define DEFAULT_OBSTACLE_REPULSION 0.1
#define DEFAULT_TIME_STEP_MS 3000

typedef struct {
    int geometric_boundary;
    int obstacle_radius;
    float obstacle_repulsion;
    int time_step_ms;
} Parameters;

void read_parameters(const char *filename, Parameters *params) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open parameters file. Using default values.");
        params->geometric_boundary = DEFAULT_GEOMETRIC_BOUNDARY;
        params->obstacle_radius = DEFAULT_OBSTACLE_RADIUS;
        params->obstacle_repulsion = DEFAULT_OBSTACLE_REPULSION;
        params->time_step_ms = DEFAULT_TIME_STEP_MS;
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char key[128];
        float value;
        if (sscanf(line, "%[^=]=%f", key, &value) == 2) {
            if (strcmp(key, "GEOMETRIC_BOUNDARY") == 0) {
                params->geometric_boundary = (int)value;
            } else if (strcmp(key, "OBSTACLE_RADIUS") == 0) {
                params->obstacle_radius = (int)value;
            } else if (strcmp(key, "OBSTACLE_REPULSION") == 0) {
                params->obstacle_repulsion = value;
            } else if (strcmp(key, "TIME_STEP_MS") == 0) {
                params->time_step_ms = (int)value;
            }
        }
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    int pipe_fd;
    char buffer[256];
    int obstacle_x, obstacle_y;

    Parameters params;
    read_parameters("./config/parameters.txt", &params);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pipe_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the named pipe for writing
    pipe_fd = open(argv[1], O_WRONLY);
    if (pipe_fd == -1) {
        perror("Failed to open pipe");
        return EXIT_FAILURE;
    }

    srand(time(NULL)); // Seed for random number generation

    printf("Obstacles Generator started. Sending random obstacle positions...\n");

    while (1) {
        // Generate random positions for the obstacle within geometric boundaries
        obstacle_x = rand() % params.geometric_boundary;
        obstacle_y = rand() % params.geometric_boundary;

        // Format the obstacle position
        snprintf(buffer, sizeof(buffer), "OBSTACLE,%d,%d", obstacle_x, obstacle_y);

        // Send the obstacle position to the blackboard
        ssize_t bytes_written = write(pipe_fd, buffer, strlen(buffer) + 1);
        if (bytes_written == -1) {
            perror("Failed to write to pipe");
            break; // Exit loop on error
        }

        printf("Obstacle generated at (%d, %d)\n", obstacle_x, obstacle_y);

        // Sleep for the specified time step
        usleep(params.time_step_ms * 1000);
    }

    close(pipe_fd);
    return EXIT_SUCCESS;
}
