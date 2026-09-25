#pragma once
 
#include <stdbool.h>
#include "lexer.h"
 
// True if tokens contains a "|" token, i.e. this is a pipeline and not a single command.
bool is_pipeline(tokenlist *tokens);
 
// Runs tokens as a pipeline of two or more commands separated by "|" tokens
// (any number of pipes is supported). Each stage may also have its own
// "<" / ">" redirection (e.g. the first stage's input, the last stage's
// output).
//
// If `background` is false, waits for every stage to finish before
// returning (unchanged behavior from parts 1-7).
// If `background` is true, does NOT wait: the whole pipeline is registered
// as a single background job (the pid displayed to the user is the LAST
// stage's, per spec) and this returns as soon as every stage has been
// started. `cmdline` is the text shown in that job's "done" message --
// only used (and required to be non-NULL) when background is true.
//
// Returns false only if the pipeline could not be started at all (bad
// command, fork failure); true if it ran, whether or not the piped
// commands succeeded.
bool execute_pipeline(tokenlist *tokens, bool background, const char *cmdline);