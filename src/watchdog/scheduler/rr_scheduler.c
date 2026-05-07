#include <stdio.h>
#include <unistd.h>
#include "rr_scheduler.h"
#include "../include/config.h"
#include "../monitor/monitor.h"
#include "../log/event_log.h"
#include "../responder/responder.h"

static const char *task_names[TASK_COUNT] = {
    "monitor",
    "log-writer",
    "responder"
};

void rr_scheduler_init(rr_scheduler_t *s) {
    s->current = 0;
    s->tick    = 0;
    log_event("INFO", "scheduler",
        "Round Robin initialized tasks=%d quantum_ms=%d poll_ms=%d",
        TASK_COUNT, g_config.quantum_ms, g_config.poll_ms);
}

void rr_scheduler_tick(rr_scheduler_t *s) {
    rr_task_id_t task = (rr_task_id_t)(s->current % TASK_COUNT);

    log_event("DEBUG", "scheduler", "task=%s tick=%d",
              task_names[task], s->tick);

    switch (task) {
        case TASK_MONITOR:
            monitor_run_once();
            break;
        case TASK_LOG_WRITER:
            event_log_flush();
            break;
        case TASK_RESPONDER:
            responder_run_once();
            break;
        default:
            break;
    }

    s->current++;
    s->tick++;
    usleep((unsigned int)g_config.quantum_ms * 1000U);
}

void rr_scheduler_destroy(rr_scheduler_t *s) {
    (void)s;
}
