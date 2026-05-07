#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include "event_log.h"
#include "../include/config.h"
#include "../semaphore/log_sem.h"

#define RING_CAP   64
#define LINE_MAX  256

static char  ring[RING_CAP][LINE_MAX];
static int   ring_head = 0; /* next write slot */
static int   ring_tail = 0; /* next flush slot */
static int   log_fd    = -1;

static int ring_full(void)  { return ((ring_head + 1) % RING_CAP) == ring_tail; }
static int ring_empty(void) { return ring_head == ring_tail; }

int event_log_open(void) {
    log_fd = open(g_config.log_path,
                  O_WRONLY | O_CREAT | O_APPEND, 0640);
    return (log_fd < 0) ? -1 : 0;
}

void event_log_close(void) {
    event_log_flush();
    if (log_fd >= 0) { close(log_fd); log_fd = -1; }
}

void event_log_reopen(void) {
    event_log_flush();
    if (log_fd >= 0) close(log_fd);
    log_fd = open(g_config.log_path,
                  O_WRONLY | O_CREAT | O_APPEND, 0640);
}

void log_event(const char *level, const char *task, const char *fmt, ...) {
    if (g_config.debug == 0 && strcmp(level, "DEBUG") == 0) return;

    char ts[32];
    time_t now = time(NULL);
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

    char body[LINE_MAX];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(body, sizeof(body), fmt, ap);
    va_end(ap);

    char line[LINE_MAX];
    int n = snprintf(line, sizeof(line),
                     "[%s] [%-5s] [%-10s] %s\n",
                     ts, level, task, body);
    if (n < 0 || n >= (int)sizeof(line)) n = (int)sizeof(line) - 1;

    /* stdout immediately (journald picks this up) */
    fputs(line, stdout);
    fflush(stdout);

    log_sem_acquire();
    if (!ring_full()) {
        memcpy(ring[ring_head], line, (size_t)n + 1);
        ring_head = (ring_head + 1) % RING_CAP;
    }
    log_sem_release();
}

void event_log_flush(void) {
    if (log_fd < 0 || ring_empty()) return;

    log_sem_acquire();

    /* Collect all pending lines into iovec */
    struct iovec iov[RING_CAP];
    int cnt = 0;
    while (!ring_empty() && cnt < RING_CAP) {
        iov[cnt].iov_base = ring[ring_tail];
        iov[cnt].iov_len  = strlen(ring[ring_tail]);
        ring_tail = (ring_tail + 1) % RING_CAP;
        cnt++;
    }

    log_sem_release();

    if (cnt > 0)
        writev(log_fd, iov, cnt);
}
