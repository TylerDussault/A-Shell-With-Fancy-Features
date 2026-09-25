#pragma once
 
#include <stdbool.h>
#include <sys/types.h>
 
// Spec: assume no more than 10 background processes running concurrently.
#define MAX_JOBS 10
 
// Registers a new background job made up of `pid_count` child pids (in
// left-to-right stage order for a pipeline; just one pid for a single
// external command). The pid shown to the user -- both immediately on
// start, and later in "jobs" -- is pids[pid_count - 1], i.e. the LAST
// stage of a pipeline, per spec ("cmd1 | cmd2 &" prints cmd2's pid).
// Every pid in the array is still tracked internally so each one gets
// reaped, even though only the last one is displayed.
// Immediately prints "[job_num] pid".
// Returns the assigned job number, or -1 if the job table is full.
int add_job(pid_t *pids, int pid_count, const char *cmdline);
 
// Non-blocking: polls every pid belonging to every active job with
// waitpid(..., WNOHANG) and reaps any that have finished. Once every pid in
// a job has been reaped, prints "[job_num]+ done cmdline" and frees that
// job's slot. Call this once per main-loop iteration (e.g. right before
// printing the next prompt).
void check_finished_jobs(void);
 
// Prints "[job_num]+ pid cmdline" for every currently active job, oldest
// first. If there are none, says so instead. Used by the "jobs" builtin.
void print_jobs(void);
 
// Blocking: waits for every pid of every active job to finish, printing the
// usual "[job_num]+ done cmdline" message for each job as it completes.
// Used by "exit" so the shell doesn't quit while background work is still
// running.
void wait_all_jobs(void);
 
// True if there is at least one active background job.
bool has_active_jobs(void);