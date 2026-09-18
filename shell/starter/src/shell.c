#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "lexer.h"
#include "expand.h"

int main()
{
    // REPL (read eval print loop)
    while (1) {
        // print prompt
        printf("%s@%s:%s>", getenv("USER"), getenv("MACHINE"), getenv("PWD"));

        // get input and tokenize
        char *input = get_input();
        tokenlist *tokens = get_tokens(input, " ");

        // tilde expand
        expand_tilde(tokens);
        
        // free memory
        free(input);
        free_tokens(tokens);
    }
}