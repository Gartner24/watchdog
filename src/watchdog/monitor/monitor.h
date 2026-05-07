#ifndef WD_MONITOR_H
#define WD_MONITOR_H

#include <sys/types.h>

typedef struct {
    double load_avg_1min;
    long   mem_free_mb;
    long   pgmajfault_delta;
    long   cpu_usec_delta;   /* cgroup v2 cpu.stat usage_usec delta */
    long   mem_current_bytes;/* cgroup v2 memory.current            */
    pid_t  top_pid;          /* highest CPU ticks PID (non-system)  */
    long   top_pid_ticks;    /* raw tick count for top_pid          */
} wd_metrics_t;

void monitor_run_once(void);
int  monitor_get_metrics(wd_metrics_t *out);

#endif
