/**
 * Concurrent Bounded FIFO Queue Design Specification
 * 
 * This header file contains the complete design for a thread-safe, bounded
 * FIFO (First-In-First-Out) queue implementation in C.
 */

#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

/* Error codes for queue operations */
typedef enum {
    QUEUE_SUCCESS = 0,
    QUEUE_ERROR_NULL_POINTER = -1,
    QUEUE_ERROR_INVALID_SIZE = -2,
    QUEUE_ERROR_MEMORY_ALLOCATION = -3,
    QUEUE_ERROR_FULL = -4,
    QUEUE_ERROR_EMPTY = -5,
    QUEUE_ERROR_MUTEX_INIT = -6,
    QUEUE_ERROR_COND_INIT = -7,
    QUEUE_ERROR_MUTEX_LOCK = -8,
    QUEUE_ERROR_COND_WAIT = -9,
    QUEUE_ERROR_COND_SIGNAL = -10,
    QUEUE_ERROR_DESTROYED = -11,
    QUEUE_ERROR_TIMEOUT = -12
} queue_error_t;

/* Forward declaration */
typedef struct concurrent_queue concurrent_queue_t;

/**
 * Data Structure Design
 * 
 * The queue uses a circular buffer implementation for efficient memory usage
 * and O(1) enqueue/dequeue operations.
 */
struct concurrent_queue {
    /* Data storage */
    void **buffer;          /* Array of void pointers to store queue elements */
    size_t capacity;        /* Maximum number of elements the queue can hold */
    size_t size;            /* Current number of elements in the queue */
    size_t head;            /* Index of the first element (for dequeue) */
    size_t tail;            /* Index where the next element will be inserted */
    
    /* Synchronization primitives */
    pthread_mutex_t mutex;      /* Mutex for protecting queue state */
    pthread_cond_t not_full;    /* Condition variable for blocking when full */
    pthread_cond_t not_empty;   /* Condition variable for blocking when empty */
    
    /* Queue state */
    bool is_destroyed;          /* Flag to indicate if queue is being destroyed */
    
    /* Statistics (optional, for monitoring) */
    size_t total_enqueued;      /* Total number of items ever enqueued */
    size_t total_dequeued;      /* Total number of items ever dequeued */
    size_t max_size_reached;    /* Maximum size the queue has reached */
};

/**
 * API Function Signatures
 */

/**
 * queue_create - Create a new concurrent bounded FIFO queue
 * @capacity: Maximum number of elements the queue can hold
 * @error: Pointer to store error code (can be NULL)
 * 
 * Returns: Pointer to the newly created queue, or NULL on failure
 * 
 * Thread Safety: Safe to call from any thread
 * Memory: Caller is responsible for calling queue_destroy()
 */
concurrent_queue_t* queue_create(size_t capacity, queue_error_t *error);

/**
 * queue_destroy - Destroy a queue and free all associated resources
 * @queue: Pointer to the queue to destroy
 * 
 * Returns: QUEUE_SUCCESS or error code
 * 
 * Thread Safety: Not safe to call while other threads are using the queue
 * Memory: Frees all memory associated with the queue structure
 * Note: Does NOT free the data pointed to by queue elements
 */
queue_error_t queue_destroy(concurrent_queue_t *queue);

/**
 * queue_enqueue - Add an element to the tail of the queue (blocking)
 * @queue: Pointer to the queue
 * @data: Pointer to the data to enqueue
 * 
 * Returns: QUEUE_SUCCESS or error code
 * 
 * Thread Safety: Safe to call from multiple threads concurrently
 * Blocking: Blocks if the queue is full until space becomes available
 */
queue_error_t queue_enqueue(concurrent_queue_t *queue, void *data);

/**
 * queue_dequeue - Remove and return an element from the head of the queue (blocking)
 * @queue: Pointer to the queue
 * @data: Pointer to store the dequeued data
 * 
 * Returns: QUEUE_SUCCESS or error code
 * 
 * Thread Safety: Safe to call from multiple threads concurrently
 * Blocking: Blocks if the queue is empty until an element becomes available
 */
queue_error_t queue_dequeue(concurrent_queue_t *queue, void **data);

/**
 * queue_try_enqueue - Try to add an element to the queue (non-blocking)
 * @queue: Pointer to the queue
 * @data: Pointer to the data to enqueue
 * 
 * Returns: QUEUE_SUCCESS, QUEUE_ERROR_FULL, or other error code
 * 
 * Thread Safety: Safe to call from multiple threads concurrently
 * Non-blocking: Returns immediately with QUEUE_ERROR_FULL if queue is full
 */
queue_error_t queue_try_enqueue(concurrent_queue_t *queue, void *data);

/**
 * queue_try_dequeue - Try to remove an element from the queue (non-blocking)
 * @queue: Pointer to the queue
 * @data: Pointer to store the dequeued data
 * 
 * Returns: QUEUE_SUCCESS, QUEUE_ERROR_EMPTY, or other error code
 * 
 * Thread Safety: Safe to call from multiple threads concurrently
 * Non-blocking: Returns immediately with QUEUE_ERROR_EMPTY if queue is empty
 */
queue_error_t queue_try_dequeue(concurrent_queue_t *queue, void **data);

/**
 * queue_enqueue_timeout - Add an element with timeout
 * @queue: Pointer to the queue
 * @data: Pointer to the data to enqueue
 * @timeout_ms: Timeout in milliseconds (0 = no wait, -1 = infinite wait)
 * 
 * Returns: QUEUE_SUCCESS, QUEUE_ERROR_TIMEOUT, or other error code
 * 
 * Thread Safety: Safe to call from multiple threads concurrently
 */
queue_error_t queue_enqueue_timeout(concurrent_queue_t *queue, void *data, int timeout_ms);

/**
 * queue_dequeue_timeout - Remove an element with timeout
 * @queue: Pointer to the queue
 * @data: Pointer to store the dequeued data
 * @timeout_ms: Timeout in milliseconds (0 = no wait, -1 = infinite wait)
 * 
 * Returns: QUEUE_SUCCESS, QUEUE_ERROR_TIMEOUT, or other error code
 * 
 * Thread Safety: Safe to call from multiple threads concurrently
 */
queue_error_t queue_dequeue_timeout(concurrent_queue_t *queue, void **data, int timeout_ms);

/**
 * queue_size - Get the current number of elements in the queue
 * @queue: Pointer to the queue
 * 
 * Returns: Current size or 0 if queue is NULL
 * 
 * Thread Safety: Safe to call from multiple threads, but result may be stale
 */
size_t queue_size(concurrent_queue_t *queue);

/**
 * queue_capacity - Get the maximum capacity of the queue
 * @queue: Pointer to the queue
 * 
 * Returns: Queue capacity or 0 if queue is NULL
 * 
 * Thread Safety: Safe to call from multiple threads
 */
size_t queue_capacity(concurrent_queue_t *queue);

/**
 * queue_is_empty - Check if the queue is empty
 * @queue: Pointer to the queue
 * 
 * Returns: true if empty, false otherwise
 * 
 * Thread Safety: Safe to call from multiple threads, but result may be stale
 */
bool queue_is_empty(concurrent_queue_t *queue);

/**
 * queue_is_full - Check if the queue is full
 * @queue: Pointer to the queue
 * 
 * Returns: true if full, false otherwise
 * 
 * Thread Safety: Safe to call from multiple threads, but result may be stale
 */
bool queue_is_full(concurrent_queue_t *queue);

/**
 * queue_clear - Remove all elements from the queue
 * @queue: Pointer to the queue
 * 
 * Returns: QUEUE_SUCCESS or error code
 * 
 * Thread Safety: Not safe to call while other threads are using the queue
 * Note: Does NOT free the data pointed to by queue elements
 */
queue_error_t queue_clear(concurrent_queue_t *queue);

/**
 * queue_get_stats - Get queue statistics
 * @queue: Pointer to the queue
 * @total_enqueued: Pointer to store total enqueued count (can be NULL)
 * @total_dequeued: Pointer to store total dequeued count (can be NULL)
 * @max_size_reached: Pointer to store max size reached (can be NULL)
 * 
 * Returns: QUEUE_SUCCESS or error code
 * 
 * Thread Safety: Safe to call from multiple threads
 */
queue_error_t queue_get_stats(concurrent_queue_t *queue, 
                              size_t *total_enqueued,
                              size_t *total_dequeued,
                              size_t *max_size_reached);

/**
 * queue_error_string - Get human-readable error message
 * @error: Error code
 * 
 * Returns: String describing the error
 * 
 * Thread Safety: Safe to call from multiple threads
 */
const char* queue_error_string(queue_error_t error);

#endif /* CONCURRENT_QUEUE_H */