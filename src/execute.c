#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h" // Include your built-in commands header
#include "parse_interface.h" // Include your parsing header

#define MAX_INPUT_SIZE 1024

// Function prototype for execute_command
void execute_command(char *input) {
    // Use the parsing function pto get arguments
    char **args = parse_input(input);

    // Check if parsing failed
    if (args == NULL) {
        return; // Exit if there was an error
    }

    // Check for built-in commands
    if (strcmp(args[0], "ls") == 0) {
        execute_ls();
    } else if (strcmp(args[0], "echo") == 0) {
        execute_echo(args);
    } else if (strcmp(args[0], "pwd") == 0) {
        execute_pwd();
    } else {
        // Handle external commands (you can add more logic here)
        printf("Command not found: %s\n", args[0]);
    }

    // Free the allocated memory for args
    free(args);
}

int main() {
    char input[MAX_INPUT_SIZE];

    // Main shell loop
    while (1) {
        printf("[QUASH]$ "); // Shell prompt

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
