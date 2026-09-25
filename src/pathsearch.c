#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "pathsearch.h"
 
// is "path" an existing regular file that we are allowed to execute?
static bool is_executable(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode) && access(path, X_OK) == 0;
}
 
char *find_path(char *command)
{
    // a command with a slash already is a path, so there is nothing to search
    if (strchr(command, '/') != NULL)
    {
        if (!is_executable(command))
            return NULL;
 
        char *copy = malloc(strlen(command) + 1);
        strcpy(copy, command);
        return copy;
    }
 
    char *path_env = getenv("PATH");
    if (path_env == NULL)
        return NULL;
 
    // split $PATH into its directories (delimited by colons)
    tokenlist *dirs = get_tokens(path_env, ":");
    char *result = NULL;
 
    // try each directory in order and stop at the first executable match
    for (size_t i = 0; i < dirs->size && result == NULL; i++)
    {
        // room for directory + "/" + command + '\0'
        char *candidate = malloc(strlen(dirs->items[i]) + strlen(command) + 2);
        strcpy(candidate, dirs->items[i]);
        strcat(candidate, "/");
        strcat(candidate, command);
 
        if (is_executable(candidate))
            result = candidate;
        else
            free(candidate);
    }
 
    free_tokens(dirs);
    return result;
}



