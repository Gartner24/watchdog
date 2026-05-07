#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "alert_queue.h"

#define SEM_NAME  "/watchdog_alerts"
#define QUEUE_CAP 16

static alert_t  queue[QUEUE_CAP];
static int      q_head = 0;
static int      q_tail = 0;
static sem_t   *q_sem  = NULL;

static int q_full(void)  { return ((q_head + 1) % QUEUE_CAP) == q_tail; }
static int q_empty(void) { return q_head == q_tail; }

int alert_queue_init(void) {
    q_sem = sem_open(SEM_NAME, O_CREAT, 0600, 1);
    return (q_sem == SEM_FAILED) ? -1 : 0;
}

void alert_queue_destroy(void) {
    if (q_sem) { sem_close(q_sem); sem_unlink(SEM_NAME); }
}

int alert_queue_push(alert_t a) {
    sem_wait(q_sem);
    int rc = -1;
    if (!q_full()) {
        queue[q_head] = a;
        q_head = (q_head + 1) % QUEUE_CAP;
        rc = 0;
    }
    sem_post(q_sem);
    return rc;
}

int alert_queue_pop(alert_t *out) {
    sem_wait(q_sem);
    int rc = -1;
    if (!q_empty()) {
        *out = queue[q_tail];
        q_tail = (q_tail + 1) % QUEUE_CAP;
        rc = 0;
    }
    sem_post(q_sem);
    return rc;
}
