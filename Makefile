CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O3 -march=native -ffast-math
LDFLAGS = -lm
TARGET = matrix_mult
SOURCES = main.c matrix_multiplication.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean run debug profile

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS = -std=c11 -Wall -Wextra -g -O0 -DDEBUG
debug: clean $(TARGET)

profile: CFLAGS = -std=c11 -Wall -Wextra -O3 -march=native -ffast-math -pg
profile: LDFLAGS = -lm -pg
profile: clean $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) gmon.out

help:
	@echo "Available targets:"
	@echo "  all     - Build the program with optimization"
	@echo "  debug   - Build with debug symbols"
	@echo "  profile - Build with profiling support"
	@echo "  run     - Build and run the program"
	@echo "  clean   - Remove build artifacts"
	@echo "  help    - Show this help message"