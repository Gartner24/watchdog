#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include "log_sem.h"

#define SEM_NAME "/watchdog_log"

static sem_t *log_sem = NULL;

int log_sem_open(void) {
    log_sem = sem_open(SEM_NAME, O_CREAT, 0600, 1);
    if (log_sem == SEM_FAILED) {
        perror("sem_open");
        return -1;
    }
    return 0;
}

void log_sem_acquire(void) {
    sem_wait(log_sem);
}

void log_sem_release(void) {
    sem_post(log_sem);
}

void log_sem_close(void) {
    if (log_sem) {
        sem_close(log_sem);
        sem_unlink(SEM_NAME);
    }
}
