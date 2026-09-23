#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "lexer.h"
#include "expand.h"
#include "redirect.h"
#include "env.h"
#include "execute.h"
#include "pathsearch.h"
 
int main()
{
    // REPL (read eval print loop)
    while (1) {
        // print prompt
        printf("%s@%s:%s>", getenv("USER"), getenv("MACHINE"), getenv("PWD"));
 
        // get input and tokenize
        char *input = get_input();
        if (input == NULL) {   /* EOF: leave the loop like "exit" */
            printf("\n");
            break;
        }
        tokenlist *tokens = get_tokens(input, " \t");
 
        // tilde expand
        expand_tilde(tokens);
 
        // if empty input
        if (tokens->size == 0) {
            free(input);
            free_tokens(tokens);
            continue;
        }
        
        // find command in PATH
        char *fname = find_path(tokens->items[0]);
 
        if (fname == NULL) {
            printf("Command not found: %s\n", tokens->items[0]);
        }
        else {
            // tokens->items is already NULL-terminated,
            // so it can be used as argv for execv()
            execute(fname, tokens);
 
            free(fname);
        }
 
 
        // free memory
        free(input);
        free_tokens(tokens);
    }
 
    return 0;
}