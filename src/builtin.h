#ifndef BUILTIN_H
#define BUILTIN_H
#include <stdio.h>

// Prototypes for built-in functions
void execute_echo(char **args);
void execute_pwd();
void execute_cd(char **args);    // New function for `cd`
void execute_export(char **args); // New function for `export`
void remove_job(int index);
void check_background_jobs();
void display_jobs();
void add_job(pid_t pid, char **args);

#endif // BUILTIN_H
