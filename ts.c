#include "tiny_sched.h"
#include <string.h>

// this is in bss and should be initialized to zero
// problems if it's not
tiny_task_t tasks[TS_MAX_TASKS];

// mark a ttid, used = 1, unused = 0
unsigned char ttid_map[TS_MAX_TASKS];


int ts_add(tiny_task_t task) {
    for(int i = 0; i < TS_MAX_TASKS; i++) {
        if(ttid_map[i] == 0) {
            ttid_map[i] = 1;
            tasks[i] = task;
            return i;
        }
    }
    return -1; // no space
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
void schedule(int* stop_var, int stop_cond) {
    long int curr_time;
    long int best_ttid = -1;
    while(*stop_var != stop_cond) {
        curr_time = ts_systime();
        for(size_t i = 0; i < TS_MAX_TASKS; i++) {
            if(ttid_map[i] == 1) {
                if(tasks[i].start_time > curr_time) {
                    if(tasks[i].start_time < tasks[best_ttid].start_time + TS_PEEK_LENGTH) {
                        if(tasks[i].priority > tasks[best_ttid].priority) {
                            best_ttid = i;
                        }
                    }
                }
            }
        }
        if(best_ttid != -1) {
            best_ttid = -1;
            tasks[best_ttid].task(&tasks[best_ttid]);
        }
    }
}
