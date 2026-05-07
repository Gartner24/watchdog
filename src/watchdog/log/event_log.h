#ifndef WD_EVENT_LOG_H
#define WD_EVENT_LOG_H

int  event_log_open(void);   /* call once at startup */
void event_log_close(void);  /* call on shutdown; flushes buffer */
void event_log_reopen(void); /* call on SIGHUP for logrotate */
void event_log_flush(void);  /* called by TASK_LOG_WRITER each tick */

/* Write an event. Thread-safe via semaphore. level: "INFO","ALERT","WARN","DEBUG" */
void log_event(const char *level, const char *task, const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));

#endif
