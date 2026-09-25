#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "pipeline.h"
#include "pathsearch.h"
#include "redirect.h"
#include "jobs.h"
 
bool is_pipeline(tokenlist *tokens)
{
    for (size_t i = 0; i < tokens->size; i++)
        if (strcmp(tokens->items[i], "|") == 0)
            return true;
    return false;
}
 
// Splits tokens on "|" into an array of new tokenlists, one per pipeline stage.
// "|" tokens themselves are not copied into any stage. *out_count is set to
// the number of stages found. Each stage is a brand-new tokenlist with its
// own copies of the strings, so the caller's original tokens is untouched
// and can be freed independently. Supports any number of stages/pipes.
static tokenlist **split_stages(tokenlist *tokens, size_t *out_count)
{
    // worst case (no "|" at all) is 1 stage; each "|" adds one more
    size_t max_stages = 1;
    for (size_t i = 0; i < tokens->size; i++)
        if (strcmp(tokens->items[i], "|") == 0)
            max_stages++;
 
    tokenlist **stages = malloc(max_stages * sizeof(tokenlist *));
    size_t count = 0;
    stages[count] = new_tokenlist();
 
    for (size_t i = 0; i < tokens->size; i++)
    {
        if (strcmp(tokens->items[i], "|") == 0)
        {
            count++;
            stages[count] = new_tokenlist();
        }
        else
        {
            add_token(stages[count], tokens->items[i]);
        }
    }
 
    *out_count = count + 1;
    return stages;
}
 
static void free_stages(tokenlist **stages, size_t count)
{
    for (size_t i = 0; i < count; i++)
        free_tokens(stages[i]);
    free(stages);
}
 
bool execute_pipeline(tokenlist *tokens, bool background, const char *cmdline)
{
    size_t n;
    tokenlist **stages = split_stages(tokens, &n);
 
    // reject a malformed pipeline (e.g. "ls | | wc", or a trailing "|") up front,
    // before anything is forked
    for (size_t i = 0; i < n; i++)
    {
        if (stages[i]->size == 0)
        {
            fprintf(stderr, "syntax error: empty command in pipeline\n");
            free_stages(stages, n);
            return false;
        }
    }
 
    // resolve every stage's executable before forking anything, so a bad
    // command name is reported without leaving any partially-started pipeline
    char **paths = malloc(n * sizeof(char *));
    bool ok = true;
    for (size_t i = 0; i < n; i++)
    {
        paths[i] = find_path(stages[i]->items[0]);
        if (paths[i] == NULL)
        {
            fprintf(stderr, "Command not found: %s\n", stages[i]->items[0]);
            ok = false;
        }
    }
 
    if (!ok)
    {
        for (size_t i = 0; i < n; i++)
            free(paths[i]);
        free(paths);
        free_stages(stages, n);
        return false;
    }
 
    // one pipe between each adjacent pair of stages
    int (*pipes)[2] = malloc((n - 1) * sizeof(int[2]));
    for (size_t i = 0; i < n - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            for (size_t j = 0; j < i; j++) { close(pipes[j][0]); close(pipes[j][1]); }
            free(pipes);
            for (size_t k = 0; k < n; k++) free(paths[k]);
            free(paths);
            free_stages(stages, n);
            return false;
        }
    }
 
    pid_t *pids = malloc(n * sizeof(pid_t));
 
    for (size_t i = 0; i < n; i++)
    {
        pid_t pid = fork();
 
        if (pid == -1)
        {
            perror("fork");
            pids[i] = -1;
            continue; // best-effort: still wait for whatever did start
        }
 
        if (pid == 0) // child: stage i
        {
            if (i > 0)
                dup2(pipes[i - 1][0], STDIN_FILENO);
            if (i < n - 1)
                dup2(pipes[i][1], STDOUT_FILENO);
 
            // every pipe fd was inherited from the parent; close them all now
            // that the needed ends have been dup2'd where they belong
            for (size_t j = 0; j < n - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
 
            // explicit "<"/">" on this stage (e.g. first stage's input file,
            // last stage's output file) overrides the pipe end just set up
            handle_redirection(stages[i]);
 
            execv(paths[i], stages[i]->items);
 
            perror("execv");
            exit(1);
        }
 
        pids[i] = pid;
    }
 
    // parent: close every pipe fd now that all children have their copies
    for (size_t i = 0; i < n - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
 
    if (background)
    {
        // register the whole pipeline as ONE job. Only successfully-forked
        // pids are tracked; the pid shown to the user is the last one of
        // those, which per spec is normally cmd(last)'s pid.
        pid_t *valid_pids = malloc(n * sizeof(pid_t));
        size_t valid_count = 0;
        for (size_t i = 0; i < n; i++)
            if (pids[i] != -1)
                valid_pids[valid_count++] = pids[i];
 
        if (valid_count > 0)
            add_job(valid_pids, (int)valid_count, cmdline);
 
        free(valid_pids);
    }
    else
    {
        // wait for every stage to finish
        for (size_t i = 0; i < n; i++)
            if (pids[i] != -1)
                waitpid(pids[i], NULL, 0);
    }
 
    free(pids);
    free(pipes);
    for (size_t i = 0; i < n; i++)
        free(paths[i]);
    free(paths);
    free_stages(stages, n);
 
    return true;
}