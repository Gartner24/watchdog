#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "monitor.h"
#include "../semaphore/log_sem.h"

/* Declared in proc_reader.c */
extern double proc_read_loadavg(void);
extern long   proc_read_mem_free_mb(void);
extern long   proc_read_pgmajfault(void);

static long prev_pgmajfault = 0;

static void log_event(const char *level, const char *msg) {
    time_t now = time(NULL);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

    log_sem_acquire();
    printf("[%s] [%s] [monitor] %s\n", ts, level, msg);
    fflush(stdout);
    log_sem_release();
}

int monitor_get_metrics(wd_metrics_t *out) {
    out->load_avg_1min   = proc_read_loadavg();
    out->mem_free_mb     = proc_read_mem_free_mb();
    long cur_fault       = proc_read_pgmajfault();
    out->pgmajfault_delta = (prev_pgmajfault > 0) ? cur_fault - prev_pgmajfault : 0;
    prev_pgmajfault = cur_fault;
    return 0;
}

void monitor_run_once(void) {
    wd_metrics_t m;
    monitor_get_metrics(&m);

    char msg[128];
    snprintf(msg, sizeof(msg),
        "poll load=%.2f mem_free_mb=%ld pgmajfault_delta=%ld",
        m.load_avg_1min, m.mem_free_mb, m.pgmajfault_delta);
    log_event("INFO", msg);

    /* TODO: compare against thresholds and enqueue alert if exceeded */
}
