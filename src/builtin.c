#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h> // For getenv and setenv
#include <sys/types.h> // For fork()
#include <sys/wait.h> // For waitpid()
#include "builtin.h"

// Function to execute the 'ls' command, handling arguments
void execute_ls(char **args) {
    pid_t pid = fork(); // Create a new process

    if (pid == -1) {
        // Fork failed
        perror("fork");
        return;
    } else if (pid == 0) {
        // Child process: execute the command
        if (args[1] != NULL) {
            // Execute command with arguments using execvp
            if (execvp(args[0], args) == -1) {
                perror("execvp");
            }
        } else {
            // Default behavior if no arguments are passed
            struct dirent *entry; // Declare a pointer to struct dirent
            DIR *dp = opendir("."); // Open the current directory

            if (dp == NULL) { // Check if the directory was opened successfully
                perror("opendir"); // Print error message if it failed
                exit(EXIT_FAILURE); // Exit child process on failure
            }

            // Print each entry on the same line
            while ((entry = readdir(dp)) != NULL) {
                // Check if the entry is not "." or ".."
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    printf("%s ", entry->d_name); // Print the name followed by a space
                }
            }

            printf("\n"); // Print a newline after all entries
            closedir(dp); // Close the directory
        }
        exit(EXIT_SUCCESS); // Exit child process successfully
    } else {
        // Parent process: wait for the child process to finish
        int status;
        waitpid(pid, &status, 0); // Wait for child process
    }
}

// Function to execute the 'echo' command with support for environment variables
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
            printf("%s ", args[i]); // Print each argument
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

// Function to execute the 'cd' command
void execute_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "cd: missing argument\n"); // Print an error if no directory is provided
    } else {
        if (chdir(args[1]) != 0) {
            perror("chdir"); // Print an error if changing the directory fails
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
