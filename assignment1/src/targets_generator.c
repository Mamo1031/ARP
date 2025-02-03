#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define WATCHDOG_PIPE_NAME "pipes/pipe_to_watchdog" 

// Function to read parameters
void read_parameters(const char *filename, int *boundary) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open parameters file. Using default values.");
        *boundary = 100; // Default value for GEOMETRIC_BOUNDARY
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char key[128];
        int value;
        if (sscanf(line, "%[^=]=%d", key, &value) == 2) {
            if (strcmp(key, "GEOMETRIC_BOUNDARY") == 0) {
                *boundary = value;
            }
        }
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    int pipe_fd;
    char buffer[256];
    int target_x, target_y;
    int boundary;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pipe_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Read parameters from parameters.txt
    read_parameters("./config/parameters.txt", &boundary);

    int wd_fd = open(WATCHDOG_PIPE_NAME, O_WRONLY);
    if (wd_fd == -1) {
        perror("Failed to open watchdog pipe in targets_generator");
    }

    // Open the named pipe for writing
    pipe_fd = open(argv[1], O_WRONLY);
    if (pipe_fd == -1) {
        perror("Failed to open pipe");
        return EXIT_FAILURE;
    }

    srand(time(NULL)); // Seed for random number generation

    printf("Targets Generator started. Sending random target positions within boundary %d...\n", boundary);

    while (1) {
        // Generate random positions for the target within boundary
        target_x = rand() % boundary;
        target_y = rand() % boundary;

        // Format the target position
        snprintf(buffer, sizeof(buffer), "TARGET,%d,%d", target_x, target_y);

        // Send the target position to the blackboard
        ssize_t bytes_written = write(pipe_fd, buffer, strlen(buffer) + 1);
        if (bytes_written == -1) {
            perror("Failed to write to pipe");
            break; // Exit loop on error
        }

        // Notify Watchdog
        if (wd_fd != -1) {
            char msg[64];
            snprintf(msg, sizeof(msg), "HEARTBEAT,targets_generator,%d", getpid());

            ssize_t ret = write(wd_fd, msg, strlen(msg)+1);
            if(ret < 0) {
                perror("Failed to write");
            } else if(ret < strlen(msg)+1) {
                fprintf(stderr, "Partial write: %ld/%ld bytes\n", (long)ret, (long)strlen(msg)+1);
            }
        }

        printf("Target generated at (%d, %d)\n", target_x, target_y);

        usleep(1 * 1e6); // Generate a new target every 1 seconds
    }

    close(pipe_fd);
    return EXIT_SUCCESS;
}
