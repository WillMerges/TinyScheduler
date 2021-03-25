#include "ts.h"
#include <string.h>

// this is in bss and should be initialized to zero
// problems if it's not
tiny_task_t tasks[TS_MAX_TASKS];

// mark a ttid, used = 1, unused = 0
unsigned char ttid_map[TS_MAX_TASKS];


int ts_add(tiny_task_t* task) {
    for(int i = 0; i < TS_MAX_TASKS; i++) {
        if(ttid_map[i] == 0) {
            ttid_map[i] = 1;
            tasks[i] = *task;
            return i;
        }
    }
    return TS_ERR_TTID; // no space
}

void ts_rem(int ttid) {
    // bounds check
    if(ttid < 0 || ttid >= TS_MAX_TASKS) {
        return;
    }
    // clear and mark unused
    memset(tasks + ttid, 0, sizeof(tiny_task_t));
    ttid_map[ttid] = 0;
}

// not all that great of time complexity,  ¯\_(ツ)_/¯
// for a small number of tasks, a dynamically allocated queue seems like a lot of overhead
void ts_schedule() {
    long int curr_time = ts_systime();
    long int best_ttid = -1;

    for(size_t i = 0; i < TS_MAX_TASKS; i++) {
        if(ttid_map[i] == 1) {
            if(tasks[i].priority == SLEEP_PRIORITY) {
                continue; // skip sleeping tasks
            }

            // run the highest priority task that needs to run
            if(tasks[i].start_time < curr_time) {
                if(best_ttid == -1) {
                    best_ttid = i;
                // no longer find the one waiting the longest, the priority level should reflect wait times now
                // } else if(tasks[i].start_time < tasks[best_ttid].start_time) {
                } else {
                    if(tasks[i].priority > tasks[best_ttid].priority) {
                        tasks[best_ttid].priority++; // bump the skipped things priority
                        best_ttid = i;
                    } else {
                        tasks[i].priority++; // bump whatever we didnt get to run up a priority
                    }
                }
            }
        }
    }

    if(best_ttid != -1) {
        tasks[best_ttid].start_time = curr_time - 1;
        tasks[best_ttid].task(&tasks[best_ttid]);
    }
}
