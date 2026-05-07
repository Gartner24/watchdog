#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

static int depth_limit = 6;
static int sleep_ms    = 50;
static int duration_s  = 30;

static volatile int stop = 0;

static void handle_signal(int sig) {
    (void)sig;
    stop = 1;
}

static void bomb(int depth) {
    if (stop) return;
    if (depth <= 0) {
        /* Leaf: sleep then exit, checking for abort every 100ms */
        for (int i = 0; i < duration_s * 10 && !stop; i++)
            usleep(100000);
        return;
    }
    usleep(sleep_ms * 1000UL);
    pid_t pid = fork();
    if (pid < 0) return;
    if (pid == 0) {
        bomb(depth - 1);
        _exit(0);
    }
    bomb(depth - 1);
    waitpid(pid, NULL, 0);
}

static void usage(const char *prog) {
    fprintf(stderr,
        "usage: %s [--depth N] [--sleep-ms N] [--duration N]\n"
        "  --depth      max fork depth (default 6, max safe: 7)\n"
        "  --sleep-ms   ms between fork waves (default 50)\n"
        "  --duration   seconds leaf processes sleep (default 30)\n",
        prog);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (geteuid() == 0) {
        fprintf(stderr, "[phantom] error: refusing to run as root\n");
        return 1;
    }

    signal(SIGTERM, handle_signal);
    signal(SIGINT,  handle_signal);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--depth") == 0 && i + 1 < argc)
            depth_limit = atoi(argv[++i]);
        else if (strcmp(argv[i], "--sleep-ms") == 0 && i + 1 < argc)
            sleep_ms = atoi(argv[++i]);
        else if (strcmp(argv[i], "--duration") == 0 && i + 1 < argc)
            duration_s = atoi(argv[++i]);
        else
            usage(argv[0]);
    }

    printf("[phantom] pid=%d depth=%d sleep_ms=%d duration_s=%d\n",
           (int)getpid(), depth_limit, sleep_ms, duration_s);
    printf("[phantom] launching fork bomb — kill -TERM %d to abort\n", (int)getpid());
    fflush(stdout);

    bomb(depth_limit);

    printf("[phantom] terminating\n");
    return 0;
}
