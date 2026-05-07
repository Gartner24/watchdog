#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "include/config.h"
#include "include/alert_queue.h"
#include "scheduler/rr_scheduler.h"
#include "semaphore/log_sem.h"
#include "log/event_log.h"

static volatile int running  = 1;
static volatile int reopen_log = 0;

static void handle_sigterm(int sig) { (void)sig; running = 0; }
static void handle_sighup(int sig)  { (void)sig; reopen_log = 1; }

int main(void) {
    config_load();

    signal(SIGTERM, handle_sigterm);
    signal(SIGINT,  handle_sigterm);
    signal(SIGHUP,  handle_sighup);

    if (log_sem_open() != 0) {
        fprintf(stderr, "fatal: could not open log semaphore\n");
        return 1;
    }

    if (alert_queue_init() != 0) {
        fprintf(stderr, "fatal: could not init alert queue\n");
        log_sem_close();
        return 1;
    }

    if (event_log_open() != 0) {
        fprintf(stderr, "warning: could not open log file %s (stdout only)\n",
                g_config.log_path);
    }

    log_event("INFO", "watchdog",
        "starting poll_ms=%d load_threshold=%.2f mem_free_mb_min=%ld log=%s",
        g_config.poll_ms, g_config.load_threshold,
        g_config.mem_free_mb_min, g_config.log_path);

    rr_scheduler_t sched;
    rr_scheduler_init(&sched);

    while (running) {
        if (reopen_log) {
            reopen_log = 0;
            event_log_reopen();
        }
        rr_scheduler_tick(&sched);
    }

    log_event("INFO", "watchdog", "shutting down");
    rr_scheduler_destroy(&sched);
    event_log_close();
    alert_queue_destroy();
    log_sem_close();
    return 0;
}
