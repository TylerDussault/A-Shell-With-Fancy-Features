#pragma once
 
// Tracks the last few "valid" commands the shell has run, so "exit" can
// display them per spec.
 
// exit only ever needs to show the last 3.
#define HISTORY_SIZE 3
 
// Records `cmdline` as the most recently run valid command. Call this for
// any command the shell actually recognized and attempted to run: a
// built-in (other than "exit" itself), an external command found on
// $PATH, or a pipeline that started successfully. Do NOT call it for
// "command not found" or a pipeline syntax error -- those aren't valid
// commands -- and do NOT call it for "exit" itself, since exit's job is to
// report what ran *before* it, not to log itself.
void history_add(const char *cmdline);
 
// Prints the last (up to) HISTORY_SIZE valid commands recorded via
// history_add(), oldest to newest. If none have been recorded, says so
// instead. Used by the "exit" built-in.
void history_print_last3(void);