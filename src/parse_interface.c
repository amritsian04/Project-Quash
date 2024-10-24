#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 64

// Function to parse the input string into command and arguments
char** parse_input(char* input) {
    char** args = malloc(MAX_TOKENS * sizeof(char*));  // Allocate memory for argument array
    if (!args) {
        perror("malloc");  // Error if memory allocation fails
        return NULL;
    }

    char* token;
    int index = 0;

    // Tokenize the input string using space as delimiter
    token = strtok(input, " ");
    while (token != NULL && index < MAX_TOKENS) {
        args[index++] = token;  // Add the token to the args array
        token = strtok(NULL, " ");
    }
    args[index] = NULL;  // Null-terminate the argument array

    return args;
}
