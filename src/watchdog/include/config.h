#ifndef WD_CONFIG_H
#define WD_CONFIG_H

typedef struct {
    int    poll_ms;           /* WD_POLL_MS,        default 1000  */
    char   log_path[256];     /* WD_LOG_PATH,        default /var/log/watchdog/events.log */
    double load_threshold;    /* WD_LOAD_THRESHOLD,  default 4.0  */
    long   mem_free_mb_min;   /* WD_MEM_FREE_MB,     default 200  */
    int    quantum_ms;        /* derived: poll_ms / 3              */
    int    debug;             /* WD_LOG_DEBUG=1 enables debug lines */
} wd_config_t;

extern wd_config_t g_config;

void config_load(void);

#endif
