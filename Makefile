CC = gcc
CFLAGS = -O3 -Wall -Wextra -std=c11 -march=native -ffast-math
LDFLAGS = -lm

TARGET = matrix_multiply
SRCS = main.c matrix_multiply.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Debug build with less optimization and debug symbols
debug: CFLAGS = -O0 -g -Wall -Wextra -std=c11
debug: clean $(TARGET)

# Profile build with profiling enabled
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: clean $(TARGET)

.PHONY: all clean run debug profile