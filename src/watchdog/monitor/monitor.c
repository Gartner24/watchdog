#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "monitor.h"
#include "../include/config.h"
#include "../include/alert_queue.h"
#include "../log/event_log.h"

extern double proc_read_loadavg(void);
extern long   proc_read_mem_free_mb(void);
extern long   proc_read_pgmajfault(void);
extern long   cgroup_read_mem_current(void);
extern long   cgroup_read_cpu_usec(void);
extern int    proc_read_top_pid(pid_t *out_pid, long *out_ticks);

static long prev_pgmajfault = 0;
static long prev_cpu_usec   = 0;

int monitor_get_metrics(wd_metrics_t *out) {
    out->load_avg_1min    = proc_read_loadavg();
    out->mem_free_mb      = proc_read_mem_free_mb();
    out->mem_current_bytes= cgroup_read_mem_current();

    long cur_fault = proc_read_pgmajfault();
    out->pgmajfault_delta = (prev_pgmajfault > 0) ? cur_fault - prev_pgmajfault : 0;
    prev_pgmajfault = cur_fault;

    long cur_cpu = cgroup_read_cpu_usec();
    out->cpu_usec_delta = (prev_cpu_usec > 0) ? cur_cpu - prev_cpu_usec : 0;
    prev_cpu_usec = cur_cpu;

    out->top_pid = 0; out->top_pid_ticks = 0;
    proc_read_top_pid(&out->top_pid, &out->top_pid_ticks);

    return 0;
}

static int threshold_breached(const wd_metrics_t *m) {
    int mask = 0;
    if (m->load_avg_1min  >= g_config.load_threshold)    mask |= WD_BREACH_LOAD;
    if (m->mem_free_mb    >= 0 &&
        m->mem_free_mb    <  g_config.mem_free_mb_min)   mask |= WD_BREACH_MEM;
    if (m->pgmajfault_delta > 100)                        mask |= WD_BREACH_PGFAULT;
    return mask;
}

void monitor_run_once(void) {
    wd_metrics_t m;
    monitor_get_metrics(&m);

    log_event("INFO", "monitor",
        "poll load=%.2f mem_free_mb=%ld pgmajfault_delta=%ld cpu_usec_delta=%ld",
        m.load_avg_1min, m.mem_free_mb, m.pgmajfault_delta, m.cpu_usec_delta);

    int breach = threshold_breached(&m);
    if (breach) {
        log_event("ALERT", "monitor",
            "threshold exceeded load=%.2f threshold=%.2f mem_free_mb=%ld min=%ld breach_mask=0x%x",
            m.load_avg_1min, g_config.load_threshold,
            m.mem_free_mb, g_config.mem_free_mb_min, breach);

        alert_t a = {
            .breach_mask  = breach,
            .suspect_pid  = m.top_pid,
            .suspect_ticks= m.top_pid_ticks,
            .load         = m.load_avg_1min,
            .mem_free_mb  = m.mem_free_mb,
        };
        if (alert_queue_push(a) < 0)
            log_event("WARN", "monitor", "alert queue full — dropped");
    }
}
