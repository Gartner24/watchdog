#ifndef RR_SCHEDULER_H
#define RR_SCHEDULER_H

typedef enum {
    TASK_MONITOR,
    TASK_LOG_WRITER,
    TASK_RESPONDER,
    TASK_COUNT
} rr_task_id_t;

typedef struct {
    int current;
} rr_scheduler_t;

void rr_scheduler_init(rr_scheduler_t *s);
void rr_scheduler_tick(rr_scheduler_t *s);
void rr_scheduler_destroy(rr_scheduler_t *s);

#endif
