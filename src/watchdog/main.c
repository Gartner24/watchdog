#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "scheduler/rr_scheduler.h"
#include "semaphore/log_sem.h"
#include "monitor/monitor.h"

static volatile int running = 1;

static void handle_sigterm(int sig) {
    (void)sig;
    running = 0;
}

int main(void) {
    signal(SIGTERM, handle_sigterm);
    signal(SIGINT, handle_sigterm);

    if (log_sem_open() != 0) {
        fprintf(stderr, "fatal: could not open log semaphore\n");
        return 1;
    }

    rr_scheduler_t sched;
    rr_scheduler_init(&sched);

    while (running) {
        rr_scheduler_tick(&sched);
    }

    rr_scheduler_destroy(&sched);
    log_sem_close();
    return 0;
}
