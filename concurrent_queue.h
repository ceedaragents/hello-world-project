#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <stddef.h>
#include <stdbool.h>
#include <time.h>

typedef struct concurrent_queue concurrent_queue_t;

typedef enum {
    QUEUE_SUCCESS = 0,
    QUEUE_ERROR_NULL_PARAM = -1,
    QUEUE_ERROR_NO_MEMORY = -2,
    QUEUE_ERROR_FULL = -3,
    QUEUE_ERROR_EMPTY = -4,
    QUEUE_ERROR_DESTROYED = -5,
    QUEUE_ERROR_TIMEOUT = -6,
    QUEUE_ERROR_SYSTEM = -7
} queue_error_t;

typedef struct {
    size_t total_enqueued;
    size_t total_dequeued;
    size_t max_size_reached;
    size_t current_size;
    size_t capacity;
} queue_stats_t;

concurrent_queue_t* queue_create(size_t capacity);
queue_error_t queue_destroy(concurrent_queue_t* queue);

queue_error_t queue_enqueue(concurrent_queue_t* queue, void* data);
queue_error_t queue_dequeue(concurrent_queue_t* queue, void** data);

queue_error_t queue_try_enqueue(concurrent_queue_t* queue, void* data);
queue_error_t queue_try_dequeue(concurrent_queue_t* queue, void** data);

queue_error_t queue_enqueue_timeout(concurrent_queue_t* queue, void* data, const struct timespec* timeout);
queue_error_t queue_dequeue_timeout(concurrent_queue_t* queue, void** data, const struct timespec* timeout);

size_t queue_size(concurrent_queue_t* queue);
size_t queue_capacity(concurrent_queue_t* queue);
bool queue_is_empty(concurrent_queue_t* queue);
bool queue_is_full(concurrent_queue_t* queue);

queue_error_t queue_get_stats(concurrent_queue_t* queue, queue_stats_t* stats);
const char* queue_error_string(queue_error_t error);

#endif