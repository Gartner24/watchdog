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

/*
 * Read CPU ticks (utime+stime) for one pid from /proc/[pid]/stat.
 * Returns -1 on failure.
 */
static long read_pid_cputicks(pid_t pid) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    /* Fields in /proc/[pid]/stat: pid (comm) state ppid ... utime stime */
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

/*
 * Scans /proc for the highest CPU-consuming non-system PID.
 * Stores result in out_pid / out_ticks (raw ticks, not percent).
 * Returns 0 on success, -1 if /proc is unreadable.
 */
int proc_read_top_pid(pid_t *out_pid, long *out_ticks) {
    DIR *d = opendir("/proc");
    if (!d) return -1;

    pid_t best_pid = 0;
    long  best_ticks = 0;
    struct dirent *ent;

    while ((ent = readdir(d)) != NULL) {
        if (!isdigit((unsigned char)ent->d_name[0])) continue;
        pid_t pid = (pid_t)atoi(ent->d_name);
        if (pid < 1000) continue; /* skip system/kernel pids */
        long ticks = read_pid_cputicks(pid);
        if (ticks > best_ticks) {
            best_ticks = ticks;
            best_pid   = pid;
        }
    }
    closedir(d);

    *out_pid   = best_pid;
    *out_ticks = best_ticks;
    return 0;
}
