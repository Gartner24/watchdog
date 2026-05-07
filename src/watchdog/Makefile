CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -g -I.
LDFLAGS = -lpthread -lrt

SRCS = main.c \
       include/config.c \
       include/alert_queue.c \
       scheduler/rr_scheduler.c \
       semaphore/log_sem.c \
       log/event_log.c \
       monitor/monitor.c \
       monitor/proc_reader.c \
       monitor/cgroup_reader.c \
       responder/responder.c

TARGET = watchdog

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
