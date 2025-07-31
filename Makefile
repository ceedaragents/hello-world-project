CC = gcc
CFLAGS = -Wall -Wextra -g -pthread -std=c11
LDFLAGS = -pthread

# Source files
SRC_DIR = src
TEST_DIR = tests
EXAMPLE_DIR = examples

# Object files
QUEUE_OBJ = $(SRC_DIR)/queue.o

# Targets
TEST_TARGET = test_queue
EXAMPLE_TARGET = queue_example

# Default target
all: $(TEST_TARGET) $(EXAMPLE_TARGET)

# Build the queue object file
$(QUEUE_OBJ): $(SRC_DIR)/queue.c $(SRC_DIR)/queue.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build the test executable
$(TEST_TARGET): $(TEST_DIR)/test_queue.c $(QUEUE_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build the example executable
$(EXAMPLE_TARGET): $(EXAMPLE_DIR)/queue_example.c $(QUEUE_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Run example
example: $(EXAMPLE_TARGET)
	./$(EXAMPLE_TARGET)

# Clean build artifacts
clean:
	rm -f $(QUEUE_OBJ) $(TEST_TARGET) $(EXAMPLE_TARGET)

# Phony targets
.PHONY: all test example clean