#ifndef MONITOR_H
#define MONITOR_H

typedef struct {
    double load_avg_1min;
    long   mem_free_mb;
    long   pgmajfault_delta;
} wd_metrics_t;

void monitor_run_once(void);
int  monitor_get_metrics(wd_metrics_t *out);

#endif
