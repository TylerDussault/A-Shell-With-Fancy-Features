#pragma once
 
#include "lexer.h"
 
//replaces every token of the form $NAME with the value of that env var.
//an unset variable expands to nothing, so its token is removed (like bash)
void expand_env(tokenlist *tokens);
 