    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "expand.h"

    void expand_tilde(tokenlist *tokens)
    {
        char *home = getenv("HOME");

        for(int i = 0; i < tokens->size; i++){
            // case 1: "~" expansion
            char *token = tokens->items[i];
            if(strcmp(token, "~") == 0){
                char *new_str = malloc(strlen(home) + 1);
                strcpy(new_str,home);

                free(tokens->items[i]);
                tokens->items[i] = new_str;
            }
            // case 2: "~/..." expansion
            else if(token[0] == '~' && token[1] == '/'){
                int size = strlen(getenv("HOME"))+ strlen(token);
                
                char *new_str = malloc(size+1);
                strcpy(new_str,getenv("HOME"));
                strcat(new_str, token+1);
                
                free(tokens->items[i]);
                tokens->items[i] = new_str;
            }
        };
    }