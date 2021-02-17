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

// how far to peek into the future to look for a higher priority task to run
#define TS_PEEK_LENGTH 10

// this needs to be defined by the system!
// the extern is unnecessary, just a reminder to define it!
extern long int ts_systime();

// priority level
typedef enum {
    LOW, MEDIUM, HIGH
} priority_t;

// task structure
typedef struct tiny_task tiny_task_t;
struct tiny_task {
    int ttid; // tiny task id, set by the scheduler
    long int start_time; // requested start time, set by user
    priority_t priority; // priority level, set by user
    void (*task) (tiny_task_t*); // task function, defined by programmer
};

// add a task to the scheduler
// returns ttid
int ts_add(tiny_task_t task);

// remove a task from the scheduler
void ts_rem(int ttid);

// runs the scheduler
// will not exit unless *stop_var == stop_cond
void schedule(int* stop_var, int stop_cond);

#endif