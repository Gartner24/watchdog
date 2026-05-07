#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

wd_config_t g_config;

void config_load(void) {
    const char *v;

    v = getenv("WD_POLL_MS");
    g_config.poll_ms = v ? atoi(v) : 1000;

    v = getenv("WD_LOG_PATH");
    strncpy(g_config.log_path,
            v ? v : "/var/log/watchdog/events.log",
            sizeof(g_config.log_path) - 1);

    v = getenv("WD_LOAD_THRESHOLD");
    g_config.load_threshold = v ? atof(v) : 4.0;

    v = getenv("WD_MEM_FREE_MB");
    g_config.mem_free_mb_min = v ? atol(v) : 200;

    v = getenv("WD_LOG_DEBUG");
    g_config.debug = (v && atoi(v) == 1);

    g_config.quantum_ms = g_config.poll_ms / 3;
    if (g_config.quantum_ms < 1) g_config.quantum_ms = 1;
}
