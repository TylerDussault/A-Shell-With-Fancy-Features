#define _POSIX_C_SOURCE 200809L // exposes getcwd() under -std=c99/c11
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include "builtins.h"
#include "jobs.h"
#include "history.h"
 
bool is_builtin(const char *command)
{
    return strcmp(command, "exit") == 0 ||
           strcmp(command, "cd") == 0 ||
           strcmp(command, "jobs") == 0;
}
 
// cd [path]
//  - no args: go to $HOME
//  - more than one arg: error
//  - target missing, or not a directory: error
//  - on success, updates $PWD so the prompt (which reads $PWD) stays correct
static void builtin_cd(tokenlist *tokens)
{
    if (tokens->size > 2)
    {
        fprintf(stderr, "cd: too many arguments\n");
        return;
    }
 
    const char *target;
    if (tokens->size == 1) // just "cd", no path given
    {
        target = getenv("HOME");
        if (target == NULL)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return;
        }
    }
    else
    {
        target = tokens->items[1];
    }
 
    struct stat st;
    if (stat(target, &st) == -1)
    {
        fprintf(stderr, "cd: %s: No such file or directory\n", target);
        return;
    }
    if (!S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "cd: %s: Not a directory\n", target);
        return;
    }
 
    if (chdir(target) == -1)
    {
        perror("cd");
        return;
    }
 
    // keep $PWD in sync with the real working directory (this is what the
    // prompt in shell.c reads), using the canonical absolute path
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        setenv("PWD", cwd, 1);
}
 
// exit
//  - waits for any still-running background jobs to finish first
//  - then prints the last (up to) three valid commands entered
static void builtin_exit(void)
{
    wait_all_jobs();
    history_print_last3();
}
 
void run_builtin(tokenlist *tokens, bool *should_exit)
{
    const char *cmd = tokens->items[0];
 
    if (strcmp(cmd, "cd") == 0)
    {
        builtin_cd(tokens);
    }
    else if (strcmp(cmd, "jobs") == 0)
    {
        print_jobs();
    }
    else if (strcmp(cmd, "exit") == 0)
    {
        builtin_exit();
        *should_exit = true;
    }
}