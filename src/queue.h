#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    void** buffer;              // Array of pointers to store queue elements
    size_t capacity;            // Maximum number of elements
    size_t size;                // Current number of elements
    size_t head;                // Index of the first element
    size_t tail;                // Index where next element will be inserted
    pthread_mutex_t mutex;      // Mutex for thread safety
    pthread_cond_t not_full;    // Condition variable for producer threads
    pthread_cond_t not_empty;   // Condition variable for consumer threads
} ConcurrentQueue;

// Initialize a new concurrent queue with the specified capacity
// Returns 0 on success, -1 on failure
int queue_init(ConcurrentQueue* queue, size_t capacity);

// Destroy the queue and free all resources
// Note: Does not free the stored elements themselves
void queue_destroy(ConcurrentQueue* queue);

// Add an element to the queue (blocking if full)
// Returns 0 on success, -1 on failure
int queue_enqueue(ConcurrentQueue* queue, void* item);

// Remove and return an element from the queue (blocking if empty)
// Returns the element on success, NULL on failure
void* queue_dequeue(ConcurrentQueue* queue);

// Try to add an element without blocking
// Returns 0 on success, -1 if queue is full or on error
int queue_try_enqueue(ConcurrentQueue* queue, void* item);

// Try to remove an element without blocking
// Returns the element on success, NULL if queue is empty or on error
void* queue_try_dequeue(ConcurrentQueue* queue);

// Get the current number of elements in the queue
size_t queue_size(ConcurrentQueue* queue);

// Check if the queue is empty
bool queue_is_empty(ConcurrentQueue* queue);

// Check if the queue is full
bool queue_is_full(ConcurrentQueue* queue);

#endif // CONCURRENT_QUEUE_H