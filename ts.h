/**
//  Tiny Scheduler
/   A low memory usage non-preemptive scheduler
/
/   Not very good but works for a small number of tasks
/
/   Will Merges 2021
**/
#ifndef TINY_SCHED_H
#define TINY_SCHED_H

// maximum number of tasks for the scheduler to track at a time
#define TS_MAX_TASKS 20

// the ttid returned when an error occurs (no more room to store task)
#define TS_ERR_TTID -1

// number of milliseconds to 'look' into the future for a higher priority task
// in theory it should be approximately the average task time
#define TS_RESOLUTION 5

// task killed value, NULL is default
#define TS_KILLED 1

// this needs to be defined by the system!
// the extern is unnecessary, just a reminder to define it!
extern long int ts_systime();

// priority level
typedef enum {
    SLEEP_PRIORITY, IDLE_PRIORITY,
    LOW_PRIORITY, MEDIUM_PRIORITY, HIGH_PRIORITY
} priority_t;

// task structure
typedef struct tiny_task tiny_task_t;
struct tiny_task {
    int ttid; // tiny task id, set by the scheduler
    long int start_time; // requested start time, set by user, units are what ts_systime uses
    priority_t priority; // priority level, set by user
    void (*task) (tiny_task_t*); // task function, defined by programmer
    unsigned char free; // user sets this if it was malloced and needs to freed when the scheduler kills it
    unsigned char killed;
};

// add a task to the scheduler
// returns ttid
int ts_add(tiny_task_t* task);

// remove a task from the scheduler
void ts_rem(int ttid);

// runs the scheduler
void ts_schedule();

#endif
