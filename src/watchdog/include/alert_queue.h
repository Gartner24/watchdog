#ifndef WD_ALERT_QUEUE_H
#define WD_ALERT_QUEUE_H

#include <sys/types.h>

#define WD_BREACH_LOAD    (1 << 0)
#define WD_BREACH_MEM     (1 << 1)
#define WD_BREACH_PGFAULT (1 << 2)

typedef struct {
    int    breach_mask;
    pid_t  suspect_pid;
    long   suspect_ticks;
    double load;
    long   mem_free_mb;
} alert_t;

int  alert_queue_init(void);
void alert_queue_destroy(void);
int  alert_queue_push(alert_t a);   /* returns -1 if full */
int  alert_queue_pop(alert_t *out); /* returns -1 if empty */

#endif
