#include "ts.h"
#include "queue.h"
#include <string.h>
#include <stdlib.h>

unsigned char ttid_map[TS_MAX_TASKS];
queue_t* queue;

// positive/zero if fst should be scheduled first, negative otherwise
int ts_queue_sort(void* fst, void* snd) {
    tiny_task_t* t1 = (tiny_task_t*)fst;
    tiny_task_t* t2 = (tiny_task_t*)snd;

    if(t1->priority = SLEEP_PRIORITY) {
        return -1; // t1 is sleeping, it should go after t2
    }

    if(t1->priority < t2->priority) {
        if(t1->start_time + TS_RESOLUTION < t2->start_time) {
            return 1; // t1 is further than resolution away from t2
        } else {
            return -1;
        }
    } else { // t1 is higher priority
        if(t2->start_time + TS_RESOLUTION < t1->start_time) {
            return -1; // t2 is further than resolution away from t1
        } else {
            return 1;
        }
    }
}

int ts_add(tiny_task_t* task) {
    // init the queue if we haven't already (it's okay if this slow since we shouldn't be adding a lot)
    if(!queue) {
        queue = q_mkqueue(&ts_queue_sort);
        if(!queue) {
            return -1;
        }
    }

    // find the next available ttid and queue it up
    for(int i = 0; i < TS_MAX_TASKS; i++) {
        if(ttid_map[i] == 0) {
            ttid_map[i] = 1;
            if(q_enqueue(queue, (void*)task) < 0) {
                return TS_ERR_TTID;
            } else {
                task->ttid = i;
                task->killed = 0;
                return i;
            }
        }
    }
}

// marked the task as killed, the scheduler will take care of it later
// it's priority will be changed
void ts_rem(int ttid) {
    q_node_t* node = queue->head;
    tiny_task_t* task = NULL;
    while(node) {
        task = (tiny_task_t*)(node->data);
        if(!task->killed && task->ttid == ttid) {
            task->priority = MEDIUM_PRIORITY; // kill it somewhat fast (don't want it hanging around long)
            task->killed = TS_KILLED; // mark it as killed

            // reclaim the ttid, this is okay since all overlapping ttids have one who's killed
            ttid_map[ttid] = 0;
            return;
        }
        node = node->next;
    }

    // reclaim the memory from the queue if it's empty
    // in theory this doesn't really happen since there should be an idle task
    if(QUEUE_EMPTY(queue)) {
        q_rmqueue(queue);
        queue = NULL;
    }
}

// called and executes ONCE (schedules AND dispatches)
// does not loop
// this should be called from the clock ISR or whatever is changing the system time
void ts_schedule() {
    if(!queue) {
        return;
    }

    tiny_task_t* task = q_peek(queue);

    if(!task) { // nothing on the queue or error
        return;
    }

    // check if it's a 'zombie' task we need to take care of
    if(task->killed == TS_KILLED) {
        if(task->free) { // task was malloced so we need to free it
            free((void*)task);
        }
        return;
    }

    if(task->priority > SLEEP_PRIORITY && task->start_time <= ts_systime()) {

        task = q_dequeue(queue);
        if(!task) { // sanity check that dequeue worked
            return;
        }

        task->task(task); // call the task function

        // requeue the task
        q_enqueue(queue, task);
    }
}

// called if a task is changed externally
// resorts the scheduler queue
void ts_update() {
    queue_t* q2 = q_mkqueue(&ts_queue_sort);

    tiny_task_t* task = q_dequeue(queue);
    while(task) {
        q_enqueue(q2, task);
    }

    q_rmqueue(queue);
    queue = q2;
}
