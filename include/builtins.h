#pragma once
 
#include <stdbool.h>
#include "lexer.h"
 
// True if `command` names one of the shell's built-in functions
// (exit, cd, jobs). These are handled entirely inside the shell process --
// from scratch, with no fork()/execv() involved -- per the assignment's
// restrictions.
bool is_builtin(const char *command);
 
// Runs the built-in named by tokens->items[0]. Only valid to call when
// is_builtin(tokens->items[0]) is true.
// Sets *should_exit to true if the shell's main loop should terminate
// afterward (i.e. this was "exit"); leaves it untouched otherwise, so the
// caller should initialize it to false first.
void run_builtin(tokenlist *tokens, bool *should_exit);