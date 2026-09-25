#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "execute.h"
#include "redirect.h"
#include "jobs.h"
 
bool execute(char *fname, tokenlist *tokens, bool background, const char *cmdline)
{
    pid_t pid = fork(); // new process
 
    if (pid == -1) // can't fork
        return false;
 
    else if (pid == 0) // child process
    {
        handle_redirection(tokens);
        execv(fname, tokens->items);
 
        perror("execv");
        exit(1);
    }
 
    else // parent
    {
        if (background)
            add_job(&pid, 1, cmdline);
        else
            waitpid(pid, NULL, 0);
    }
 
    return true;
}
