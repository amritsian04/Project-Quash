#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 64

char** parse_input(char* input) {
    char** args = malloc(MAX_TOKENS * sizeof(char*));
    if (!args) {
        perror("malloc");
        return NULL;
    }

    char* token;
    int index = 0;

    // Tokenize the input
    token = strtok(input, " ");
    while (token != NULL && index < MAX_TOKENS) {
        args[index++] = token;
        token = strtok(NULL, " ");
    }
    args[index] = NULL; // Null-terminate the argument array

    return args;
}