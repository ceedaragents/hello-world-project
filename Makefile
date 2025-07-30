CC = cc
CFLAGS = -O3 -Wall -Wextra -std=c99
LDFLAGS = -lm

all: matrix_multiply

matrix_multiply: matrix_multiply.c
	$(CC) $(CFLAGS) -o matrix_multiply matrix_multiply.c $(LDFLAGS)

run: matrix_multiply
	./matrix_multiply

clean:
	rm -f matrix_multiply

.PHONY: all run clean