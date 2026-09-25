#define _POSIX_C_SOURCE 200809L // exposes strdup() under -std=c11
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "jobs.h"
 
typedef struct {
    int job_num;
    pid_t *pids;       // every child process that belongs to this job
    bool *reaped;       // parallel array: has pids[i] already been waited on?
    int pid_count;
    int remaining;      // how many pids in this job still haven't been reaped
    pid_t display_pid;  // the pid shown to the user (last stage, or the pid)
    char *cmdline;
    bool active;
} Job;
 
static Job jobs[MAX_JOBS];
static int next_job_num = 1; // job numbers increment forever, never reused
 
static int find_free_slot(void)
{
    for (int i = 0; i < MAX_JOBS; i++)
        if (!jobs[i].active)
            return i;
    return -1;
}
 
int add_job(pid_t *pids, int pid_count, const char *cmdline)
{
    int slot = find_free_slot();
    if (slot == -1)
    {
        // spec says we can assume this never happens (max 10 concurrent),
        // but fail safe instead of corrupting the table
        fprintf(stderr, "jobs: too many background jobs\n");
        return -1;
    }
 
    Job *j = &jobs[slot];
 
    j->pids = malloc(pid_count * sizeof(pid_t));
    j->reaped = calloc(pid_count, sizeof(bool));
    memcpy(j->pids, pids, pid_count * sizeof(pid_t));
 
    j->job_num = next_job_num++;
    j->pid_count = pid_count;
    j->remaining = pid_count;
    j->display_pid = pids[pid_count - 1];
    j->cmdline = strdup(cmdline);
    j->active = true;
 
    printf("[%d] %d\n", j->job_num, j->display_pid);
 
    return j->job_num;
}
 
static void free_job(int slot)
{
    Job *j = &jobs[slot];
    free(j->pids);
    free(j->reaped);
    free(j->cmdline);
    j->active = false;
}
 
void check_finished_jobs(void)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (!jobs[i].active)
            continue;
 
        Job *j = &jobs[i];
 
        for (int p = 0; p < j->pid_count; p++)
        {
            if (j->reaped[p])
                continue;
 
            int status;
            pid_t r = waitpid(j->pids[p], &status, WNOHANG);
            if (r > 0)
            {
                j->reaped[p] = true;
                j->remaining--;
            }
        }
 
        if (j->remaining == 0)
        {
            printf("[%d]+ done %s\n", j->job_num, j->cmdline);
            free_job(i);
        }
    }
}
 
void print_jobs(void)
{
    bool any = false;
 
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (!jobs[i].active)
            continue;
 
        any = true;
        printf("[%d]+ %d %s\n", jobs[i].job_num, jobs[i].display_pid, jobs[i].cmdline);
    }
 
    if (!any)
        printf("No active background jobs\n");
}
 
void wait_all_jobs(void)
{
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (!jobs[i].active)
            continue;
 
        Job *j = &jobs[i];
 
        for (int p = 0; p < j->pid_count; p++)
        {
            if (j->reaped[p])
                continue;
            int status;
            waitpid(j->pids[p], &status, 0);
            j->reaped[p] = true;
        }
 
        printf("[%d]+ done %s\n", j->job_num, j->cmdline);
        free_job(i);
    }
}
 
bool has_active_jobs(void)
{
    for (int i = 0; i < MAX_JOBS; i++)
        if (jobs[i].active)
            return true;
    return false;
}