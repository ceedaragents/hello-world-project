CC = cc
CFLAGS = -O3 -Wall -Wextra -std=c99
LDFLAGS = -lm

# Source files
SRCS = main.c matrix.c matrix_multiply_impl.c matrix_test.c matrix_benchmark.c
OBJS = $(SRCS:.c=.o)

# Header files
HEADERS = matrix.h matrix_multiply.h matrix_test.h matrix_benchmark.h

# Target executable
TARGET = matrix_multiply

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile source files to object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run tests only
test: $(TARGET)
	./$(TARGET) --test

# Run benchmarks only
benchmark: $(TARGET)
	./$(TARGET) --benchmark

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJS)

# Debug build
debug: CFLAGS = -g -O0 -Wall -Wextra -std=c99
debug: clean $(TARGET)

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Build the matrix multiplication program (default)"
	@echo "  run       - Build and run the program"
	@echo "  test      - Build and run tests only"
	@echo "  benchmark - Build and run benchmarks only"
	@echo "  clean     - Remove build artifacts"
	@echo "  debug     - Build with debug symbols"
	@echo "  help      - Show this help message"

.PHONY: all run test benchmark clean debug help