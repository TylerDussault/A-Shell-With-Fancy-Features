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
        if (strcmp(tokens->items[i], "<") == 0)
        {
            // open, dup2, close

            free(tokens->items[i]);
            free(tokens->items[i + 1]);

            for (int j = i; j < tokens->size - 1; j++)
                tokens->items[j] = tokens->items[j + 2];

            tokens->size -= 2;
            tokens->items[tokens->size] = NULL;

            i--;
        }
        else if (strcmp(tokens->items[i], ">") == 0)
        {
            // open, dup2, close

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