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
#include "builtins.h"
#include "history.h"
 
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
 
/* Returns a malloc'd copy of `s` with trailing whitespace stripped. Used
 * both as the history entry for a command, and as the starting point for
 * stripping a trailing '&' off of it. */
static char *trim_trailing_ws(const char *s)
{
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        len--;
 
    char *out = malloc(len + 1);
    memcpy(out, s, len);
    out[len] = '\0';
    return out;
}
 
/* Given a whitespace-trimmed line that is known to end in '&', returns a
 * malloc'd copy with that '&' (and any whitespace before it) removed. This
 * is the text shown in a background job's "[n]+ done ..." message. */
static char *strip_trailing_amp(const char *trimmed)
{
    size_t len = strlen(trimmed);
    len--; // drop the '&' itself
 
    while (len > 0 && isspace((unsigned char)trimmed[len - 1]))
        len--;
 
    char *out = malloc(len + 1);
    memcpy(out, trimmed, len);
    out[len] = '\0';
    return out;
}
 
int main()
{
    // Unbuffer stdin. Without this, glibc's stdio pulls a large chunk of
    // stdin into an internal buffer on the first read, well beyond what
    // get_input() has actually consumed so far. That's invisible for
    // normal execution, but it breaks "shell-ception" (running this shell
    // recursively from within itself, sharing the same stdin): once a
    // command is forked+exec'd into a *new* copy of this shell, the child
    // starts with a fresh stdio buffer, but the kernel's read position on
    // stdin has already been advanced past data the parent silently
    // pre-buffered -- so the child finds nothing left to read, and the
    // parent ends up consuming the rest of the input itself instead. An
    // unbuffered stdin guarantees a read() only ever consumes exactly what
    // this process has actually processed, so a nested instance picks up
    // cleanly wherever the parent left off.
    setvbuf(stdin, NULL, _IONBF, 0);
 
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
 
        // the full line as typed (minus trailing whitespace), used for
        // this command's history entry
        char *trimmed = trim_trailing_ws(input);
 
        tokenlist *tokens = get_tokens(input, " \t");
 
        // "cmd ... &" -> run in the background. Strip the trailing "&"
        // token (it must not end up in argv) and remember the trimmed
        // command line for this job's eventual "done" message.
        bool background = false;
        char *cmdline = NULL;
        if (tokens->size > 0 && strcmp(tokens->items[tokens->size - 1], "&") == 0)
        {
            background = true;
            free(tokens->items[tokens->size - 1]);
            tokens->items[tokens->size - 1] = NULL;
            tokens->size--;
            cmdline = strip_trailing_amp(trimmed);
        }
 
        // tilde expand, then environment variable expand
        expand_tilde(tokens);
        expand_env(tokens);
 
        // if empty input (or input was just "&")
        if (tokens->size == 0) {
            free(cmdline);
            free(trimmed);
            free(input);
            free_tokens(tokens);
            continue;
        }
 
        if (is_builtin(tokens->items[0])) {
            // "exit" itself isn't logged as a valid command -- it reports
            // what ran *before* it, so recording it here would be circular
            if (strcmp(tokens->items[0], "exit") != 0)
                history_add(trimmed);
 
            bool should_exit = false;
            run_builtin(tokens, &should_exit);
 
            if (should_exit) {
                free(cmdline);
                free(trimmed);
                free(input);
                free_tokens(tokens);
                break;
            }
        }
        else if (is_pipeline(tokens)) {
            // one or more "|" separators: run every stage as a pipeline
            if (execute_pipeline(tokens, background, cmdline))
                history_add(trimmed);
        }
        else {
            // find command in PATH
            char *fname = find_path(tokens->items[0]);
 
            if (fname == NULL) {
                printf("Command not found: %s\n", tokens->items[0]);
            }
            else {
                history_add(trimmed);
 
                // tokens->items is already NULL-terminated,
                // so it can be used as argv for execv()
                if (!execute(fname, tokens, background, cmdline))
                    printf("Error: failed to run %s\n", tokens->items[0]);
 
                free(fname);
            }
        }
 
        // free memory
        free(cmdline);
        free(trimmed);
        free(input);
        free_tokens(tokens);
    }
 
    return 0;
}