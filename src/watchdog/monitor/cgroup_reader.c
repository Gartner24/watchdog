#include <stdio.h>
#include "monitor.h"

#define CGROUP_MEM "/sys/fs/cgroup/system.slice/memory.current"

/* Reads current memory usage of system.slice from cgroup v2. Returns bytes. */
long cgroup_read_mem_current(void) {
    FILE *f = fopen(CGROUP_MEM, "r");
    if (!f) return -1;
    long value;
    fscanf(f, "%ld", &value);
    fclose(f);
    return value;
}
