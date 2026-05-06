#include <stdio.h>
#include <unistd.h>
#include "rr_scheduler.h"
#include "../monitor/monitor.h"
#include "../semaphore/log_sem.h"

static const char *task_names[TASK_COUNT] = {
    "monitor",
    "log-writer",
    "responder"
};

void rr_scheduler_init(rr_scheduler_t *s) {
    s->current = 0;
    printf("[scheduler] initialized Round Robin over %d tasks\n", TASK_COUNT);
}

void rr_scheduler_tick(rr_scheduler_t *s) {
    rr_task_id_t task = (rr_task_id_t)(s->current % TASK_COUNT);
    printf("[scheduler] running task=%s\n", task_names[task]);

    switch (task) {
        case TASK_MONITOR:
            monitor_run_once();
            break;
        case TASK_LOG_WRITER:
            /* TODO: flush in-memory event buffer to disk */
            break;
        case TASK_RESPONDER:
            /* TODO: check alert queue and send SIGKILL if needed */
            break;
        default:
            break;
    }

    s->current++;
    usleep(333000); /* ~333ms quantum for default 1000ms poll */
}

void rr_scheduler_destroy(rr_scheduler_t *s) {
    (void)s;
}
