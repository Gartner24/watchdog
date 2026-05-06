#include <stdio.h>
#include <stdlib.h>
#include "monitor.h"

/* Reads /proc/loadavg and returns the 1-minute load average. */
double proc_read_loadavg(void) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) return -1.0;
    double load1;
    fscanf(f, "%lf", &load1);
    fclose(f);
    return load1;
}

/* Reads MemFree from /proc/meminfo and returns it in MB. */
long proc_read_mem_free_mb(void) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) return -1;
    char key[64];
    long value;
    char unit[16];
    while (fscanf(f, "%63s %ld %15s", key, &value, unit) == 3) {
        if (__builtin_strcmp(key, "MemFree:") == 0) {
            fclose(f);
            return value / 1024; /* kB -> MB */
        }
    }
    fclose(f);
    return -1;
}

/* Reads pgmajfault from /proc/vmstat. */
long proc_read_pgmajfault(void) {
    FILE *f = fopen("/proc/vmstat", "r");
    if (!f) return -1;
    char key[64];
    long value;
    while (fscanf(f, "%63s %ld", &key, &value) == 2) {
        if (__builtin_strcmp(key, "pgmajfault") == 0) {
            fclose(f);
            return value;
        }
    }
    fclose(f);
    return -1;
}
