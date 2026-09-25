#pragma once
 
#include "lexer.h"
 
// True if tokens contains a "|" token, i.e. this is a pipeline and not a single command.
bool is_pipeline(tokenlist *tokens);
 
// Runs tokens as a pipeline of two or more commands separated by "|" tokens
// (any number of pipes is supported), waits for all of them to finish, and
// returns once the whole pipeline is done. Each stage may also have its own
// "<" / ">" redirection (e.g. the first stage's input, the last stage's output).
// Returns false only if the pipeline could not be started at all (bad command,
// fork failure); true if it ran, whether or not the piped commands succeeded.
bool execute_pipeline(tokenlist *tokens);
 