#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

// Default values
#define DEFAULT_BUFFER_SIZE 256
#define DEFAULT_MASS 1.0
#define DEFAULT_VISCOSITY 1.0
#define DEFAULT_TIMESTEP 50 // in milliseconds
#define DEFAULT_BOUNDARY 100

// Structure to hold drone state
typedef struct {
    double x, y;       // Position
    double vx, vy;     // Velocity
    double fx, fy;     // Forces
} DroneState;

// Function to read parameters from file
void read_parameters(const char *filename, int *buffer_size, double *mass, double *viscosity, double *boundary, int *timestep) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open parameters file. Using default values.");
        *buffer_size = DEFAULT_BUFFER_SIZE;
        *mass = DEFAULT_MASS;
        *viscosity = DEFAULT_VISCOSITY;
        *boundary = DEFAULT_BOUNDARY;
        *timestep = DEFAULT_TIMESTEP;
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char key[128];
        double value;
        if (sscanf(line, "%[^=]=%lf", key, &value) == 2) {
            if (strcmp(key, "BUFFER_SIZE") == 0) *buffer_size = (int)value;
            else if (strcmp(key, "DRONE_MASS") == 0) *mass = value;
            else if (strcmp(key, "VISCOUS_COEFF") == 0) *viscosity = value;
            else if (strcmp(key, "GEOMETRIC_BOUNDARY") == 0) *boundary = value;
            else if (strcmp(key, "TIME_STEP_MS") == 0) *timestep = (int)value;
        }
    }
    fclose(file);
}

// Function to update drone dynamics
void update_dynamics(DroneState *drone, double mass, double viscosity, double timestep) {
    // Convert timestep to seconds
    double dt = timestep / 1000.0;

    // Update velocities using Euler's method
    drone->vx += (drone->fx - viscosity * drone->vx) * dt / mass;
    drone->vy += (drone->fy - viscosity * drone->vy) * dt / mass;

    // Update positions
    drone->x += drone->vx * dt;
    drone->y += drone->vy * dt;

    // Reset forces
    drone->fx = 0;
    drone->fy = 0;
}

// Main function
int main(int argc, char *argv[]) {
    int pipe_fd;
    int buffer_size;
    double mass, viscosity, boundary;
    int timestep;
    char *parameters_file = "./config/parameters.txt";
    char buffer[DEFAULT_BUFFER_SIZE];
    DroneState drone = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    // Read parameters from file
    read_parameters(parameters_file, &buffer_size, &mass, &viscosity, &boundary, &timestep);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pipe_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the named pipe
    pipe_fd = open(argv[1], O_RDONLY);
    if (pipe_fd == -1) {
        perror("Failed to open pipe");
        return EXIT_FAILURE;
    }

    printf("Drone Dynamics started. Waiting for commands...\n");

    while (1) {
        // Read commands from the pipe
        ssize_t bytes_read = read(pipe_fd, buffer, buffer_size);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // Null-terminate the string
            if (strcmp(buffer, "UP") == 0) drone.fy += 1.0;
            else if (strcmp(buffer, "UP-RIGHT") == 0) {drone.fy += 1.0/sqrt(2.0); drone.fx += 1.0/sqrt(2.0);}
            else if (strcmp(buffer, "RIGHT") == 0) drone.fx += 1.0;
            else if (strcmp(buffer, "DOWN-RIGHT") == 0) {drone.fy -= 1.0/sqrt(2.0); drone.fx += 1.0/sqrt(2.0);}
            else if (strcmp(buffer, "DOWN") == 0) drone.fy -= 1.0;
            else if (strcmp(buffer, "DOWN-LEFT") == 0) {drone.fy -= 1.0/sqrt(2.0); drone.fx -= 1.0/sqrt(2.0);}
            else if (strcmp(buffer, "LEFT") == 0) drone.fx -= 1.0;
            else if (strcmp(buffer, "UP-LEFT") == 0) {drone.fy += 1.0/sqrt(2.0); drone.fx += 1.0/sqrt(2.0);}
            

            printf("Command received: %s\n", buffer);
        } else if (bytes_read == -1) {
            perror("Error reading from pipe");
            break;
        }

        // Update drone dynamics
        update_dynamics(&drone, mass, viscosity, timestep);

        // Print drone state
        printf("Drone Position: (%.2f, %.2f), Velocity: (%.2f, %.2f)\n",
               drone.x, drone.y, drone.vx, drone.vy);

        // Simulate timestep delay
        usleep(timestep * 1000);
    }

    close(pipe_fd);
    return EXIT_SUCCESS;
}
