#ifndef BUILTIN_H
#define BUILTIN_H
#include <stdio.h>

// Prototypes for built-in functions
void execute_ls(char **args);
void execute_echo(char **args);
void execute_pwd();
void execute_cd(char **args);    // New function for `cd`
void execute_export(char **args); // New function for `export`

#endif // BUILTIN_H
