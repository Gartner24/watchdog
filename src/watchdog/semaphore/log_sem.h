#ifndef LOG_SEM_H
#define LOG_SEM_H

int  log_sem_open(void);
void log_sem_acquire(void);
void log_sem_release(void);
void log_sem_close(void);

#endif
