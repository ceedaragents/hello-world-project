CC = gcc
CFLAGS = -Wall -Wextra -pthread -g -O2
LDFLAGS = -pthread

all: test_concurrent_queue example

test_concurrent_queue: test_concurrent_queue.o concurrent_queue.o
	$(CC) $(LDFLAGS) -o $@ $^

example: example.o concurrent_queue.o
	$(CC) $(LDFLAGS) -o $@ $^

concurrent_queue.o: concurrent_queue.c concurrent_queue.h
	$(CC) $(CFLAGS) -c concurrent_queue.c

test_concurrent_queue.o: test_concurrent_queue.c concurrent_queue.h
	$(CC) $(CFLAGS) -c test_concurrent_queue.c

example.o: example.c concurrent_queue.h
	$(CC) $(CFLAGS) -c example.c

test: test_concurrent_queue
	./test_concurrent_queue

run: example
	./example

clean:
	rm -f *.o test_concurrent_queue example

.PHONY: all test run clean