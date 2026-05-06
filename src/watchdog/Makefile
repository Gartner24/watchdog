CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -g -I.
LDFLAGS = -lpthread -lrt

SRCS = main.c \
       scheduler/rr_scheduler.c \
       semaphore/log_sem.c \
       monitor/monitor.c \
       monitor/proc_reader.c \
       monitor/cgroup_reader.c

TARGET = watchdog

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
