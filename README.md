# Concurrent Bounded FIFO Queue in C

A thread-safe, bounded FIFO (First-In-First-Out) queue implementation in C using pthreads.

## Features

- **Thread-safe**: Multiple producers and consumers can safely access the queue concurrently
- **Bounded capacity**: Queue has a fixed maximum size to prevent unbounded memory growth
- **Blocking operations**: Producers block when queue is full, consumers block when empty
- **Non-blocking operations**: Try-enqueue and try-dequeue for immediate return
- **Timeout operations**: Operations with configurable timeout periods
- **Statistics tracking**: Monitor queue usage and performance metrics
- **Generic data storage**: Store pointers to any data type

## API Reference

### Creation and Destruction
```c
concurrent_queue_t* queue_create(size_t capacity);
queue_error_t queue_destroy(concurrent_queue_t* queue);
```

### Blocking Operations
```c
queue_error_t queue_enqueue(concurrent_queue_t* queue, void* data);
queue_error_t queue_dequeue(concurrent_queue_t* queue, void** data);
```

### Non-blocking Operations
```c
queue_error_t queue_try_enqueue(concurrent_queue_t* queue, void* data);
queue_error_t queue_try_dequeue(concurrent_queue_t* queue, void** data);
```

### Timeout Operations
```c
queue_error_t queue_enqueue_timeout(concurrent_queue_t* queue, void* data, const struct timespec* timeout);
queue_error_t queue_dequeue_timeout(concurrent_queue_t* queue, void** data, const struct timespec* timeout);
```

### Query Functions
```c
size_t queue_size(concurrent_queue_t* queue);
size_t queue_capacity(concurrent_queue_t* queue);
bool queue_is_empty(concurrent_queue_t* queue);
bool queue_is_full(concurrent_queue_t* queue);
```

### Statistics
```c
queue_error_t queue_get_stats(concurrent_queue_t* queue, queue_stats_t* stats);
const char* queue_error_string(queue_error_t error);
```

## Building

```bash
make all        # Build all programs
make test       # Build and run tests
make example    # Build the example program
make clean      # Clean build artifacts
```

## Usage Example

```c
#include "concurrent_queue.h"

// Create a queue with capacity 10
concurrent_queue_t* queue = queue_create(10);

// Producer thread
int* data = malloc(sizeof(int));
*data = 42;
queue_enqueue(queue, data);

// Consumer thread
int* retrieved;
queue_dequeue(queue, (void**)&retrieved);
printf("Got: %d\n", *retrieved);
free(retrieved);

// Cleanup
queue_destroy(queue);
```

## Implementation Details

- Uses a circular buffer for O(1) enqueue/dequeue operations
- Single mutex protects all queue state
- Condition variables for efficient blocking without busy-waiting
- Supports graceful shutdown with queue_destroy()

## Error Codes

- `QUEUE_SUCCESS`: Operation completed successfully
- `QUEUE_ERROR_NULL_PARAM`: NULL parameter passed
- `QUEUE_ERROR_NO_MEMORY`: Memory allocation failed
- `QUEUE_ERROR_FULL`: Queue is full (non-blocking operations)
- `QUEUE_ERROR_EMPTY`: Queue is empty (non-blocking operations)
- `QUEUE_ERROR_DESTROYED`: Queue has been destroyed
- `QUEUE_ERROR_TIMEOUT`: Operation timed out
- `QUEUE_ERROR_SYSTEM`: System error occurred

## Thread Safety

All queue operations are thread-safe. Multiple threads can safely:
- Enqueue items concurrently
- Dequeue items concurrently
- Mix enqueue and dequeue operations
- Query queue state

## Memory Management

- The queue owns its internal structure and buffer
- Users own the data pointed to by enqueued items
- Users must free dequeued data when no longer needed
- queue_destroy() does not free user data