#define _POSIX_C_SOURCE 200809L // exposes fchmod() under -std=c99
 
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include "redirect.h"
 
// removes tokens[i] and tokens[i+1] (an operator and its filename) from the list
static void remove_two_tokens(tokenlist *tokens, int i)
{
    free(tokens->items[i]);
    free(tokens->items[i + 1]);
 
    for (int j = i; j < tokens->size - 1; j++)
        tokens->items[j] = tokens->items[j + 2];
 
    tokens->size -= 2;
    tokens->items[tokens->size] = NULL;
}
 
void handle_redirection(tokenlist *tokens)
{
    for (int i = 0; i < tokens->size; i++)
    {
        // cmd < input.txt
        if (strcmp(tokens->items[i], "<") == 0)
        {
            char *filename = tokens->items[i + 1];
            struct stat st;
 
            // the file must exist and be a regular file (not a directory, device, etc.)
            if (stat(filename, &st) == -1 || !S_ISREG(st.st_mode))
            {
                fprintf(stderr, "%s: No such file or not a regular file\n", filename);
                exit(1);
            }
 
            int fd = open(filename, O_RDONLY);
            if (fd == -1)
            {
                perror("open");
                exit(1);
            }
 
            dup2(fd, STDIN_FILENO);
            close(fd);
 
            remove_two_tokens(tokens, i);
            i--;
        }
 
        // cmd > output.txt
        else if (strcmp(tokens->items[i], ">") == 0)
        {
            // -rw------- : owner read/write only, per spec
            int fd = open(tokens->items[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
 
            if (fd == -1)
            {
                perror("open");
                exit(1);
            }
 
            // open()'s mode is only applied when the file is newly created, so
            // force the permissions here in case an existing file is being overwritten
            fchmod(fd, 0600);
 
            dup2(fd, STDOUT_FILENO);
            close(fd);
 
            remove_two_tokens(tokens, i);
            i--;
        }
    }
}