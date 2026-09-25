#pragma once
 
#include "lexer.h"
 
// Returns the full path of the executable to run for "command", or NULL if there isn't one.
//  - a command containing a '/' is used as the path itself (no search)
//  - otherwise the directories in $PATH are searched in order and the first match wins
// The returned string is malloc'd, so the caller must free it.
char *find_path(char *command);