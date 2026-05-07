#include <stdio.h>
#include <string.h>
#include "monitor.h"

#define CGROUP_MEM "/sys/fs/cgroup/system.slice/memory.current"
#define CGROUP_CPU "/sys/fs/cgroup/system.slice/cpu.stat"

long cgroup_read_mem_current(void) {
    FILE *f = fopen(CGROUP_MEM, "r");
    if (!f) return -1;
    long value;
    fscanf(f, "%ld", &value);
    fclose(f);
    return value;
}

/* Reads usage_usec from cgroup v2 cpu.stat. Returns microseconds. */
long cgroup_read_cpu_usec(void) {
    FILE *f = fopen(CGROUP_CPU, "r");
    if (!f) return -1;
    char key[64];
    long value;
    while (fscanf(f, "%63s %ld", key, &value) == 2) {
        if (strcmp(key, "usage_usec") == 0) {
            fclose(f);
            return value;
        }
    }
    fclose(f);
    return -1;
}
