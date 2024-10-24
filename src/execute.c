#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h> // For PATH_MAX
#include "builtin.h" // Include your built-in commands header
#include "parse_interface.h" // Include your parsing header

#define MAX_INPUT_SIZE 1024

// Declare num_jobs as an external variable to share with builtin.c
extern int num_jobs;

#ifndef PATH_MAX
#define PATH_MAX 4096  // A common default value; adjust if necessary
#endif

// Function to expand environment variables in the input arguments
void expand_environment_variables(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (args[i][0] == '$') {
            // Strip the '$' and get the environment variable's value
            char *env_var = getenv(args[i] + 1);
            if (env_var != NULL) {
                // Allocate a new string to hold the expanded value
                char *expanded_arg = malloc(strlen(env_var) + 1);
                if (expanded_arg == NULL) {
                    perror("malloc");
                    exit(EXIT_FAILURE);
                }
                strcpy(expanded_arg, env_var);
                args[i] = expanded_arg; // Replace argument with expanded value
            }
        }
    }
}

// Function to resolve the full path of directories (e.g., handling .. and .)
char* resolve_full_path(char *path) {
    char *resolved_path = malloc(PATH_MAX);
    if (resolved_path == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    // Use realpath to resolve relative paths like ../ and ./
    if (realpath(path, resolved_path) == NULL) {
        perror("realpath");
        free(resolved_path);
        return NULL; // Return NULL if path resolution fails
    }
    return resolved_path;
}

// Function to execute the given command
void execute_command(char *input) {
    // Parse the input to get command and arguments
    char **args = parse_input(input);

    // Check if parsing failed
    if (args == NULL) {
        return; // Exit if there was an error
    }

    // Expand environment variables in the arguments
    expand_environment_variables(args);

    // Check if the command ends with '&' indicating background execution
    int is_background = 0;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "&") == 0) {
            is_background = 1;
            args[i] = NULL;  // Remove '&' from the arguments
            break;
        }
    }

    // Built-in commands handling
    if (strcmp(args[0], "echo") == 0) {
        execute_echo(args);
    } else if (strcmp(args[0], "pwd") == 0) {
        execute_pwd();
    } else if (strcmp(args[0], "cd") == 0) {
        execute_cd(args);
    } else if (strcmp(args[0], "export") == 0) {
        execute_export(args);
    } else if (strcmp(args[0], "jobs") == 0) {
        display_jobs();  // Display all running background jobs
    } else {
        // Fork a new process for external commands with arguments (including environment variables)
        pid_t pid = fork();

        if (pid == -1) {
            // Fork failed
            perror("fork");
            return;
        } else if (pid == 0) {
            // In child process, execute the command with arguments
            // If the command is not an absolute or relative path, search in PATH
            if (args[0][0] != '/' && args[0][0] != '.') {
                // Let execvp handle the PATH search
                if (execvp(args[0], args) == -1) {
                    perror("execvp");
                }
            } else {
                // If it's an absolute or relative path, execute it directly
                if (execvp(args[0], args) == -1) {
                    perror("execvp");
                }
            }
            exit(EXIT_FAILURE); // Exit child if exec fails
        } else {
            if (is_background) {
                // Add job to the job list for background execution
                add_job(pid, args);
                // Don't wait for the background process
            } else {
                // Foreground execution: wait for the child process to finish
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }

    // Free the allocated memory for args
    free(args);

    // Check for any background jobs that have completed
    check_background_jobs();
}

int main() {
    char input[MAX_INPUT_SIZE];

    // Main shell loop
    while (1) {
        printf("[QUASH]$ "); // Shell prompt

        // Get input from user
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break; // Exit if EOF (Ctrl+D)
        }

        // Remove trailing newline character
        input[strlen(input) - 1] = '\0';

        // If the user types 'exit' or 'quit', exit the shell
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            break;
        }

        // Send the input to the command executor
        execute_command(input);
    }

    return 0;
}
