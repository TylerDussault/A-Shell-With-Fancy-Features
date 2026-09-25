#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"
#include "lexer.h"
#include "expand.h"
#include "redirect.h"
#include "env.h"
#include "execute.h"
#include "pathsearch.h"
#include "pipeline.h"
#include "jobs.h"
 
/* returns the env var's value, or a placeholder if it is unset */
static const char *env_or(const char *name, const char *fallback)
{
    const char *val = getenv(name);
    return (val != NULL) ? val : fallback;
}
 
/* prints USER@MACHINE:PWD> */
static void print_prompt(void)
{
    printf("%s@%s:%s>", env_or("USER", "user"), env_or("MACHINE", "machine"),
           env_or("PWD", "?"));
    fflush(stdout); /* no newline in the prompt, so force it out before reading input */
}
 
/* Returns a malloc'd copy of `input` with trailing whitespace, and (if
 * present) a trailing '&' plus any whitespace before it, stripped off.
 * This is the text shown in a background job's "[n]+ done ..." message. */
static char *strip_trailing_amp(const char *input)
{
    size_t len = strlen(input);
 
    while (len > 0 && isspace((unsigned char)input[len - 1]))
        len--;
 
    if (len > 0 && input[len - 1] == '&')
    {
        len--;
        while (len > 0 && isspace((unsigned char)input[len - 1]))
            len--;
    }
 
    char *out = malloc(len + 1);
    memcpy(out, input, len);
    out[len] = '\0';
    return out;
}
 
int main()
{
    // REPL (read eval print loop)
    while (1) {
        // reap any background jobs that finished since the last prompt
        check_finished_jobs();
 
        // print prompt
        print_prompt();
 
        // get input and tokenize
        char *input = get_input();
        if (input == NULL) {   /* EOF: leave the loop like "exit" */
            printf("\n");
            break;
        }
        tokenlist *tokens = get_tokens(input, " \t");
 
        // "cmd ... &" -> run in the background. Strip the trailing "&"
        // token (it must not end up in argv) and remember the trimmed
        // original command line for this job's eventual "done" message.
        bool background = false;
        char *cmdline = NULL;
        if (tokens->size > 0 && strcmp(tokens->items[tokens->size - 1], "&") == 0)
        {
            background = true;
            free(tokens->items[tokens->size - 1]);
            tokens->items[tokens->size - 1] = NULL;
            tokens->size--;
            cmdline = strip_trailing_amp(input);
        }
 
        // tilde expand, then environment variable expand
        expand_tilde(tokens);
        expand_env(tokens);
 
        // if empty input (or input was just "&")
        if (tokens->size == 0) {
            free(cmdline);
            free(input);
            free_tokens(tokens);
            continue;
        }
 
        if (is_pipeline(tokens)) {
            // one or more "|" separators: run every stage as a pipeline
            execute_pipeline(tokens, background, cmdline);
        }
        else {
            // find command in PATH
            char *fname = find_path(tokens->items[0]);
 
            if (fname == NULL) {
                printf("Command not found: %s\n", tokens->items[0]);
            }
            else {
                // tokens->items is already NULL-terminated,
                // so it can be used as argv for execv()
                if (!execute(fname, tokens, background, cmdline))
                    printf("Error: failed to run %s\n", tokens->items[0]);
 
                free(fname);
            }
        }
 
 
        // free memory
        free(cmdline);
        free(input);
        free_tokens(tokens);
    }
 
    return 0;
}