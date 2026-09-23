#include <stdlib.h>
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "redirect.h"

void handle_redirection(tokenlist *tokens){ 
    // loop through tokens for < or >
    for (int i = 0; i < tokens->size; i++)
    {
        // cmd < input.txt
        if (strcmp(tokens->items[i], "<") == 0)
        {
            // open, dup2, close
            int fd = open(tokens->items[i + 1], O_RDONLY);
            if (fd == -1)
            {
                perror("open");
                exit(1);
            }

            dup2(fd, STDIN_FILENO);
            close(fd);

            free(tokens->items[i]);
            free(tokens->items[i + 1]);

            for (int j = i; j < tokens->size - 1; j++)
                tokens->items[j] = tokens->items[j + 2];

            tokens->size -= 2;
            tokens->items[tokens->size] = NULL;

            i--;
        }

        // cmd > output.txt
        else if (strcmp(tokens->items[i], ">") == 0)
        {
            // open, dup2, close
            int fd = open(tokens->items[i + 1], O_WRONLY | O_CREAT | O_TRUNC,0644); // open with --rw------- priviledges

            if (fd == -1)
            {
                perror("open");
                exit(1);
            }

            dup2(fd, STDOUT_FILENO);
            close(fd);

            free(tokens->items[i]);
            free(tokens->items[i + 1]);

            for (int j = i; j < tokens->size - 1; j++)
                tokens->items[j] = tokens->items[j + 2];

            tokens->size -= 2;
            tokens->items[tokens->size] = NULL;

            i--;
        }
    }
}