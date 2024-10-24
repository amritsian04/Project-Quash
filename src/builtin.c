#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "builtin.h"

void execute_ls() {
    struct dirent *entry; // Declare a pointer to struct dirent
    DIR *dp = opendir("."); // Open the current directory

    if (dp == NULL) { // Check if the directory was opened successfully
        perror("opendir"); // Print error message if it failed
        return;
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

// Function to execute the 'echo' command
void execute_echo(char **args) {
    // Print each argument
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
}

// Function to execute the 'pwd' command
void execute_pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd");
    }
}
