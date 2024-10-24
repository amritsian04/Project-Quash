#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> // For getenv and setenv
#include <sys/types.h> // For fork()
#include <sys/wait.h> // For waitpid()
#include "builtin.h"
#define MAX_JOBS 100
#define MAX_INPUT_SIZE 1024

// Structure to hold job information
struct Job {
    int job_id;
    pid_t pid;
    char command[MAX_INPUT_SIZE];  // Stores the full command string for the job
};

struct Job jobs[MAX_JOBS]; // Array to hold background jobs
int num_jobs = 0;  // To track the number of background jobs

// Function to remove a job from the jobs list
void remove_job(int index) {
    if (index < 0 || index >= num_jobs) {
        return; // Index out of bounds
    }
    // Shift jobs left to fill the gap created by the removal
    for (int i = index; i < num_jobs - 1; i++) {
        jobs[i] = jobs[i + 1];
    }
    num_jobs--; // Decrease the job count
}

// Function to add a background job
void add_job(pid_t pid, char **args) {
    if (num_jobs < MAX_JOBS) {
        jobs[num_jobs].job_id = num_jobs + 1; // Assign job ID
        jobs[num_jobs].pid = pid; // Store the process ID
        snprintf(jobs[num_jobs].command, sizeof(jobs[num_jobs].command), "%s", args[0]); // Store the command
        num_jobs++; // Increment the job count
        printf("Background job started: [%d] %d %s\n", jobs[num_jobs - 1].job_id, pid, args[0]); // Print job info
    } else {
        printf("Too many background jobs running.\n");
    }
}

// Function to display all running background jobs
void display_jobs() {
    printf("ID     PID     Command\n");
    for (int i = 0; i < num_jobs; i++) {
        printf("[%d]    %d    %s\n", jobs[i].job_id, jobs[i].pid, jobs[i].command);
    }
}

// Function to check for background jobs that have completed
void check_background_jobs() {
    int status;
    pid_t pid;
    for (int i = 0; i < num_jobs; i++) {
        pid = waitpid(jobs[i].pid, &status, WNOHANG);
        if (pid > 0) {
            printf("Completed: [%d] %d %s\n", jobs[i].job_id, jobs[i].pid, jobs[i].command);
            // Remove the completed job from the list
            remove_job(i);
            i--;  // Adjust index to check the next job correctly
        }
    }
}

void execute_echo(char **args) {
    for (int i = 1; args[i] != NULL; i++) {
        if (args[i][0] == '$') {
            // Handle environment variables by checking for the '$' character
            char *env_var = getenv(args[i] + 1); // Skip the '$' character
            if (env_var != NULL) {
                printf("%s ", env_var); // Print the environment variable value
            } else {
                printf("%s ", args[i]); // If the environment variable doesn't exist, print as is
            }
        } else {
            // Only print the argument as is if it's not an environment variable (don't expand . or ..)
            printf("%s ", args[i]);
        }
    }
    printf("\n"); // Print a newline after all arguments
}

// Function to execute the 'pwd' command
void execute_pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd); // Print the current working directory
    } else {
        perror("getcwd"); // Print an error if getting the current directory fails
    }
}

void execute_cd(char **args) {
    // If no argument is provided, change to home directory
    if (args[1] == NULL) {
        char *home = getenv("HOME"); // Get home directory from environment variable
        if (home != NULL) {
            if (chdir(home) != 0) {
                perror("chdir"); // Print error if changing directory fails
            }
        } else {
            fprintf(stderr, "HOME environment variable not set.\n");
        }
    } else {
        // Change to the specified directory
        if (chdir(args[1]) != 0) {
            perror("chdir"); // Print error if changing directory fails
        }
    }
}

// Function to execute the 'export' command
void execute_export(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "export: missing argument\n"); // Print an error if no argument is provided
    } else {
        // Split the argument into name and value using '=' as the delimiter
        char *name = strtok(args[1], "=");
        char *value = strtok(NULL, "=");
        if (name != NULL && value != NULL) {
            if (setenv(name, value, 1) != 0) {
                perror("setenv"); // Print an error if setting the environment variable fails
            }
        } else {
            fprintf(stderr, "export: invalid format, use NAME=VALUE\n"); // Print an error for invalid format
        }
    }
}