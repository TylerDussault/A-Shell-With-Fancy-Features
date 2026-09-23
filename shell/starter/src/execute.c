#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "execute.h"
#include <sys/wait.h>

bool execute(char *fname, tokenlist *tokens)
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
        waitpid(pid, NULL, 0);
    }

    return true;
}
