#include <stdlib.h>
#include <string.h>
#include "env.h"
 
void expand_env(tokenlist *tokens)
{
    for (size_t i = 0; i < tokens->size; i++)
    {
        char *token = tokens->items[i];
 
        // only whole-argument variables: starts with '$' and has a name ("$" alone is literal)
        if (token[0] != '$' || token[1] == '\0')
            continue;
 
        char *value = getenv(token + 1);
 
        if (value != NULL)
        {
            // replace the token with a copy of the value
            char *new_str = malloc(strlen(value) + 1);
            strcpy(new_str, value);
 
            free(token);
            tokens->items[i] = new_str;
        }
        else
        {
            // unset variable: remove the token and shift the rest left
            free(token);
            for (size_t j = i; j < tokens->size; j++)
                tokens->items[j] = tokens->items[j + 1]; // last one copies the NULL terminator
 
            tokens->size--;
            i--; // recheck this index, it now holds the next token (size_t wraps back to 0)
        }
    }
}
