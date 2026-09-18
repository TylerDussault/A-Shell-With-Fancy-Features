#include <stdio.h>
#include <stdlib.h>
#include "shell.h"

int main()
{
    // REPL (read eval print loop)
    while (1) {
        // print prompt
        printf("%s@%s:%s>", getenv("USER"), getenv("MACHINE"), getenv("PWD"));

        // get input
        char *input = get_input();

    }
}