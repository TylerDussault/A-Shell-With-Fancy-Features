#pragma once
 
#include <stdbool.h>
#include "lexer.h"
 
// Executes the executable at `fname` with `tokens` as argv.
// If `background` is false, the parent waits for it to finish (unchanged
// behavior from parts 1-7).
// If `background` is true, the parent does NOT wait; instead the child is
// registered as a background job (see jobs.h), printed as "[n] pid"
// immediately, and reaped later by check_finished_jobs(). `cmdline` is the
// text shown in that job's eventual "done" message -- only used (and
// required to be non-NULL) when background is true.
bool execute(char *fname, tokenlist *tokens, bool background, const char *cmdline);