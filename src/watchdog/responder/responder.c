#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "responder.h"
#include "../include/alert_queue.h"
#include "../log/event_log.h"

/* Processes that must never be killed regardless of load */
static const char *whitelist[] = {
    "watchdog", "sshd", "systemd", "init", "bash", "zsh", "sh", NULL
};

static int is_whitelisted(pid_t pid) {
    char path[64], comm[64];
    snprintf(path, sizeof(path), "/proc/%d/comm", (int)pid);
    FILE *f = fopen(path, "r");
    if (!f) return 1; /* can't read: don't kill */
    comm[0] = '\0';
    fscanf(f, "%63s", comm);
    fclose(f);
    for (int i = 0; whitelist[i]; i++) {
        if (strcmp(comm, whitelist[i]) == 0) return 1;
    }
    return 0;
}

void responder_run_once(void) {
    alert_t a;
    if (alert_queue_pop(&a) < 0) return; /* nothing to do */

    log_event("INFO", "responder",
        "processing alert breach_mask=0x%x suspect_pid=%d load=%.2f mem_free_mb=%ld",
        a.breach_mask, (int)a.suspect_pid, a.load, a.mem_free_mb);

    if (!(a.breach_mask & WD_BREACH_LOAD) && !(a.breach_mask & WD_BREACH_MEM))
        return; /* pgfault only: log but don't kill */

    if (a.suspect_pid <= 0) {
        log_event("WARN", "responder", "no suspect PID to kill");
        return;
    }

    if (a.suspect_pid < 1000) {
        log_event("WARN", "responder",
            "suspect pid=%d is system-range — skipping", (int)a.suspect_pid);
        return;
    }

    if (is_whitelisted(a.suspect_pid)) {
        log_event("WARN", "responder",
            "suspect pid=%d is whitelisted — skipping", (int)a.suspect_pid);
        return;
    }

    if (kill(a.suspect_pid, SIGKILL) == 0) {
        log_event("WARN", "responder",
            "SIGKILL pid=%d ticks=%ld load=%.2f",
            (int)a.suspect_pid, a.suspect_ticks, a.load);
    } else {
        log_event("WARN", "responder",
            "SIGKILL pid=%d failed (already exited?)", (int)a.suspect_pid);
    }
}
