#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include "monitor.h"

double proc_read_loadavg(void) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) return -1.0;
    double load1;
    fscanf(f, "%lf", &load1);
    fclose(f);
    return load1;
}

long proc_read_mem_free_mb(void) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) return -1;
    char key[64];
    long value;
    char unit[16];
    while (fscanf(f, "%63s %ld %15s", key, &value, unit) == 3) {
        if (strcmp(key, "MemFree:") == 0) {
            fclose(f);
            return value / 1024;
        }
    }
    fclose(f);
    return -1;
}

long proc_read_pgmajfault(void) {
    FILE *f = fopen("/proc/vmstat", "r");
    if (!f) return -1;
    char key[64];
    long value;
    while (fscanf(f, "%63s %ld", key, &value) == 2) {
        if (strcmp(key, "pgmajfault") == 0) {
            fclose(f);
            return value;
        }
    }
    fclose(f);
    return -1;
}

static long read_pid_cputicks(pid_t pid) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    int ipid, ppid, pgrp, session, tty, tpgid;
    unsigned int flags;
    long minflt, cminflt, majflt, cmajflt;
    long utime, stime;
    char comm[256], state;

    fscanf(f, "%d %255s %c %d %d %d %d %d %u %ld %ld %ld %ld %ld %ld",
           &ipid, comm, &state, &ppid, &pgrp, &session, &tty, &tpgid,
           &flags, &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime);
    fclose(f);
    return utime + stime;
}

/* Per-pid tick state for delta computation across polls */
#define MAX_TRACKED 4096
typedef struct { pid_t pid; long ticks; } pid_tick_t;
static pid_tick_t tick_table[MAX_TRACKED];
static int        tick_count = 0;

static long lookup_prev(pid_t pid) {
    for (int i = 0; i < tick_count; i++)
        if (tick_table[i].pid == pid) return tick_table[i].ticks;
    return 0;
}

static void upsert_prev(pid_t pid, long ticks) {
    for (int i = 0; i < tick_count; i++) {
        if (tick_table[i].pid == pid) { tick_table[i].ticks = ticks; return; }
    }
    if (tick_count < MAX_TRACKED) {
        tick_table[tick_count].pid   = pid;
        tick_table[tick_count].ticks = ticks;
        tick_count++;
    }
}

/*
 * Scans /proc for the PID with the highest CPU tick delta since the last call.
 * Using deltas instead of cumulative ticks prevents long-running processes
 * (e.g. mysqld, node) from always winning over freshly-spawned attackers.
 */
int proc_read_top_pid(pid_t *out_pid, long *out_ticks) {
    DIR *d = opendir("/proc");
    if (!d) return -1;

    pid_t best_pid   = 0;
    long  best_delta = 0;
    struct dirent *ent;

    while ((ent = readdir(d)) != NULL) {
        if (!isdigit((unsigned char)ent->d_name[0])) continue;
        pid_t pid = (pid_t)atoi(ent->d_name);
        if (pid < 1000) continue;
        long cur = read_pid_cputicks(pid);
        if (cur < 0) continue;
        long delta = cur - lookup_prev(pid);
        upsert_prev(pid, cur);
        if (delta > best_delta) {
            best_delta = delta;
            best_pid   = pid;
        }
    }
    closedir(d);

    *out_pid   = best_pid;
    *out_ticks = best_delta;
    return 0;
}
