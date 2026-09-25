#define _POSIX_C_SOURCE 200809L // exposes strdup()
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"
 
// small ring buffer of the last HISTORY_SIZE command lines
static char *entries[HISTORY_SIZE];
static int count = 0;      // how many slots are filled so far (caps at HISTORY_SIZE)
static int next_slot = 0;  // next index to write to (wraps around)
 
void history_add(const char *cmdline)
{
    free(entries[next_slot]);
    entries[next_slot] = strdup(cmdline);
 
    next_slot = (next_slot + 1) % HISTORY_SIZE;
    if (count < HISTORY_SIZE)
        count++;
}
 
void history_print_last3(void)
{
    if (count == 0)
    {
        printf("No valid commands have been entered\n");
        return;
    }
 
    // entries were written starting at slot 0 and wrap around; the oldest
    // of the currently-kept entries is `count` slots behind next_slot
    int start = (next_slot - count + HISTORY_SIZE) % HISTORY_SIZE;
 
    printf("Last %d valid command%s:\n", count, count == 1 ? "" : "s");
    for (int i = 0; i < count; i++)
    {
        int idx = (start + i) % HISTORY_SIZE;
        printf("  %s\n", entries[idx]);
    }
}