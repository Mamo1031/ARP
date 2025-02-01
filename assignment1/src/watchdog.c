#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define DEFAULT_TIMEOUT_SEC 5
#define BUFFER_SIZE 256

typedef struct {
    int process_id;
    time_t last_heartbeat;
} ProcessInfo;

ProcessInfo processes[BUFFER_SIZE];

// Read parameters from file
int read_parameters(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open parameters file");
        return DEFAULT_TIMEOUT_SEC;
    }

    char key[128];
    int value;
    int timeout_sec = DEFAULT_TIMEOUT_SEC;

    while (fscanf(file, "%[^=]=%d", key, &value) == 2) {
        if (strcmp(key, "TIMEOUT_SEC") == 0) {
            timeout_sec = value;
        }
    }

    fclose(file);
    return timeout_sec;
}

// Check if processes are alive
void check_processes(int timeout_sec) {
    time_t current_time = time(NULL);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (processes[i].process_id != -1 && current_time - processes[i].last_heartbeat > timeout_sec) {
            fprintf(stderr, "Warning: Process %d timed out.\n", processes[i].process_id);
            fprintf(stderr, "Stopping the system.\n");
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pipe_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *pipe_name = argv[1];
    int pipe_fd = open(pipe_name, O_RDONLY);
    if (pipe_fd == -1) {
        perror("Failed to open pipe");
        return EXIT_FAILURE;
    }

    int timeout_sec = read_parameters("./config/parameters.txt");
    fd_set read_fds;
    char buffer[BUFFER_SIZE];
    time_t current_time;

    printf("Watchdog started. Monitoring processes...\n");

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(pipe_fd, &read_fds);

        struct timeval timeout = {1, 0};
        int activity = select(pipe_fd + 1, &read_fds, NULL, NULL, &timeout);

        if (activity < 0) {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        current_time = time(NULL);

        if (FD_ISSET(pipe_fd, &read_fds)) {
            if (read(pipe_fd, buffer, BUFFER_SIZE) > 0) {
                int process_id;
                sscanf(buffer, "HEARTBEAT,%d", &process_id);
                processes[process_id].process_id = process_id;
                processes[process_id].last_heartbeat = current_time;
                printf("Received heartbeat from Process %d at %ld\n", process_id, current_time);
            }
        }

        check_processes(timeout_sec);
    }

    close(pipe_fd);
    return EXIT_SUCCESS;
}
