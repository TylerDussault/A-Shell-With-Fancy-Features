 #include <stdlib.h>
#include <string.h>
#include "expand.h"
 
void expand_tilde(tokenlist *tokens)
{
    char *home = getenv("HOME");
    if (home == NULL) // nothing to expand to
        return;
 
    for (size_t i = 0; i < tokens->size; i++)
    {
        char *token = tokens->items[i];
 
        // only "~" by itself or "~/..." gets expanded
        if (strcmp(token, "~") == 0 || strncmp(token, "~/", 2) == 0)
        {
            // $HOME followed by everything after the '~' (nothing, for a lone "~")
            char *new_str = malloc(strlen(home) + strlen(token));
            strcpy(new_str, home);
            strcat(new_str, token + 1);
 
            free(token);
            tokens->items[i] = new_str;
        }
    }
}