CC     = gcc
CFLAGS = -Wall -Wextra -pedantic -g

TARGET = phantom

.PHONY: all clean

all: $(TARGET)

$(TARGET): fork_bomb.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
