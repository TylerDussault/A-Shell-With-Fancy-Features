#include <stdlib.h>
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>
#include "redirect.h"

void handle_redirection(tokenlist *tokens){    
    // loop through tokens for < or >
    for(int i = 0; i < tokens->size; i++){
        char *token = tokens->items[i];
        
        // cmd < file_in
        if (strcmp(tokens->items[i], "<") == 0)
        {
            int fd = open(tokens->items[i + 1], O_RDONLY);

            // error opening file
            if (fd == -1)
            {
                perror("open");
                exit(1);
            }

            dup2(fd, STDIN_FILENO);
            close(fd);

            // remove < and filename
            free(tokens->items[i]);
            free(tokens->items[i + 1]);

            for (int j = i; j < tokens->size - 2; j++){
                tokens->items[j] = tokens->items[j + 2];
            }

            tokens->size -= 2;
            i--;
        }

        // cmd > file_out
        else if(strcmp(token,">") == 0){
            int fd = open(tokens->items[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            
            // error opening file
            if (fd == -1)
            {
                perror("open");
                exit(1);
            }

            dup2(fd, STDOUT_FILENO);
            close(fd);

            // remove > and filename
            free(tokens->items[i]);
            free(tokens->items[i + 1]);

            for (int j = i; j < tokens->size - 2; j++){
                tokens->items[j] = tokens->items[j + 2];
            }
                
            tokens->size -= 2;
            i--;
        }

        // null terminate token for execv()
        tokens->items[tokens->size] = NULL;
    }

}