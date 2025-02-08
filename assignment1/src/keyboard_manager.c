#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#define DEFAULT_BUFFER_SIZE 256

#define WATCHDOG_PIPE_NAME "pipes/pipe_to_watchdog" 

int read_buffer_size(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open parameters file. Using default buffer size.");
        return DEFAULT_BUFFER_SIZE;
    }

    char line[256];
    int buffer_size = DEFAULT_BUFFER_SIZE;
    while (fgets(line, sizeof(line), file)) {
        char key[128];
        int value;
        if (sscanf(line, "%[^=]=%d", key, &value) == 2) {
            if (strcmp(key, "BUFFER_SIZE") == 0) {
                buffer_size = value;
            }
        }
    }
    fclose(file);
    return buffer_size;
}

// Configure terminal for non-blocking input
void configure_terminal(struct termios *old) {
    struct termios new;
    tcgetattr(STDIN_FILENO, old);
    new = *old;
    new.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

// Restore terminal settings
void restore_terminal(const struct termios *old) {
    tcsetattr(STDIN_FILENO, TCSANOW, old);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pipe_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int buffer_size = read_buffer_size("./config/parameters.txt");
    char buffer[buffer_size];

    int wd_fd = open(WATCHDOG_PIPE_NAME, O_WRONLY);
    if (wd_fd == -1) {
        perror("Failed to open watchdog pipe in keyboard_manager");
    }

    int pipe_fd = open(argv[1], O_WRONLY);
    if (pipe_fd == -1) {
        perror("Failed to open pipe");
        return EXIT_FAILURE;
    }

    struct termios old_termios;
    configure_terminal(&old_termios);
    printf("Keyboard Manager started. Use 'w/a/s/d' for movement, 'q' to quit.\n");

    char command;
    while (1) {
        command = getchar();
        if (command == 'q') {
            printf("Exiting Keyboard Manager.\n");
            break;
        }

        // Map commands to actions
        switch (command) {
            case 'r': strcpy(buffer, "UP"); break;
            case 't': strcpy(buffer, "UP-RIGHT"); break;
            case 'g': strcpy(buffer, "RIGHT"); break;
            case 'b': strcpy(buffer, "DOWN-RIGHT"); break;
            case 'v': strcpy(buffer, "DOWN"); break;
            case 'c': strcpy(buffer, "DOWN-LEFT"); break;
            case 'd': strcpy(buffer, "LEFT"); break;
            case 'e': strcpy(buffer, "UP-LEFT"); break;
            default: continue;
        }

        ssize_t bytes_written = write(pipe_fd, buffer, strlen(buffer) + 1);
        if (bytes_written == -1) {
            perror("Failed to write to pipe");
            break;
        }

        // Notify Watchdog
        if (wd_fd != -1) {
            char msg[64];
            snprintf(msg, sizeof(msg), "HEARTBEAT,drone_dynamics,%d", getpid());

            ssize_t ret = write(wd_fd, msg, strlen(msg)+1);
            if(ret < 0) {
                perror("Failed to write");
            } else if(ret < strlen(msg)+1) {
                fprintf(stderr, "Partial write: %ld/%ld bytes\n", (long)ret, (long)strlen(msg)+1);
            }
        }

        printf("Command sent: %s\n", buffer);
    }

    restore_terminal(&old_termios);
    close(pipe_fd);
    return EXIT_SUCCESS;
}
