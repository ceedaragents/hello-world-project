/**
 * Concurrent Bounded FIFO Queue - Implementation Skeleton
 * 
 * This file demonstrates how the design would be implemented with
 * proper error handling and thread safety.
 */

#include "concurrent_queue_design.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

/* Helper macro for cleanup on error */
#define CLEANUP_AND_RETURN(queue, err) do { \
    if (queue) { \
        pthread_cond_destroy(&(queue)->not_empty); \
        pthread_cond_destroy(&(queue)->not_full); \
        pthread_mutex_destroy(&(queue)->mutex); \
        free((queue)->buffer); \
        free(queue); \
    } \
    return err; \
} while(0)

/* Create a new concurrent queue */
concurrent_queue_t* queue_create(size_t capacity, queue_error_t *error) {
    queue_error_t local_error = QUEUE_SUCCESS;
    
    /* Validate parameters */
    if (capacity == 0) {
        if (error) *error = QUEUE_ERROR_INVALID_SIZE;
        return NULL;
    }
    
    /* Allocate queue structure */
    concurrent_queue_t *queue = calloc(1, sizeof(concurrent_queue_t));
    if (!queue) {
        if (error) *error = QUEUE_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }
    
    /* Allocate buffer */
    queue->buffer = calloc(capacity, sizeof(void*));
    if (!queue->buffer) {
        free(queue);
        if (error) *error = QUEUE_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }
    
    /* Initialize queue state */
    queue->capacity = capacity;
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;
    queue->is_destroyed = false;
    queue->total_enqueued = 0;
    queue->total_dequeued = 0;
    queue->max_size_reached = 0;
    
    /* Initialize mutex */
    int ret = pthread_mutex_init(&queue->mutex, NULL);
    if (ret != 0) {
        free(queue->buffer);
        free(queue);
        if (error) *error = QUEUE_ERROR_MUTEX_INIT;
        return NULL;
    }
    
    /* Initialize condition variables */
    ret = pthread_cond_init(&queue->not_full, NULL);
    if (ret != 0) {
        pthread_mutex_destroy(&queue->mutex);
        free(queue->buffer);
        free(queue);
        if (error) *error = QUEUE_ERROR_COND_INIT;
        return NULL;
    }
    
    ret = pthread_cond_init(&queue->not_empty, NULL);
    if (ret != 0) {
        pthread_cond_destroy(&queue->not_full);
        pthread_mutex_destroy(&queue->mutex);
        free(queue->buffer);
        free(queue);
        if (error) *error = QUEUE_ERROR_COND_INIT;
        return NULL;
    }
    
    if (error) *error = QUEUE_SUCCESS;
    return queue;
}

/* Destroy a queue */
queue_error_t queue_destroy(concurrent_queue_t *queue) {
    if (!queue) {
        return QUEUE_ERROR_NULL_POINTER;
    }
    
    /* Lock and mark as destroyed */
    pthread_mutex_lock(&queue->mutex);
    queue->is_destroyed = true;
    
    /* Wake up all waiting threads */
    pthread_cond_broadcast(&queue->not_full);
    pthread_cond_broadcast(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    
    /* Clean up resources */
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
    pthread_mutex_destroy(&queue->mutex);
    
    free(queue->buffer);
    free(queue);
    
    return QUEUE_SUCCESS;
}

/* Blocking enqueue operation */
queue_error_t queue_enqueue(concurrent_queue_t *queue, void *data) {
    if (!queue) {
        return QUEUE_ERROR_NULL_POINTER;
    }
    
    pthread_mutex_lock(&queue->mutex);
    
    /* Wait while queue is full and not destroyed */
    while (queue->size >= queue->capacity && !queue->is_destroyed) {
        int ret = pthread_cond_wait(&queue->not_full, &queue->mutex);
        if (ret != 0) {
            pthread_mutex_unlock(&queue->mutex);
            return QUEUE_ERROR_COND_WAIT;
        }
    }
    
    /* Check if queue was destroyed while waiting */
    if (queue->is_destroyed) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_DESTROYED;
    }
    
    /* Add element to queue */
    queue->buffer[queue->tail] = data;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
    
    /* Update statistics */
    queue->total_enqueued++;
    if (queue->size > queue->max_size_reached) {
        queue->max_size_reached = queue->size;
    }
    
    /* Signal that queue is not empty */
    pthread_cond_signal(&queue->not_empty);
    
    pthread_mutex_unlock(&queue->mutex);
    return QUEUE_SUCCESS;
}

/* Blocking dequeue operation */
queue_error_t queue_dequeue(concurrent_queue_t *queue, void **data) {
    if (!queue || !data) {
        return QUEUE_ERROR_NULL_POINTER;
    }
    
    pthread_mutex_lock(&queue->mutex);
    
    /* Wait while queue is empty and not destroyed */
    while (queue->size == 0 && !queue->is_destroyed) {
        int ret = pthread_cond_wait(&queue->not_empty, &queue->mutex);
        if (ret != 0) {
            pthread_mutex_unlock(&queue->mutex);
            return QUEUE_ERROR_COND_WAIT;
        }
    }
    
    /* Check if queue was destroyed while waiting */
    if (queue->is_destroyed && queue->size == 0) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_DESTROYED;
    }
    
    /* Remove element from queue */
    *data = queue->buffer[queue->head];
    queue->buffer[queue->head] = NULL;  /* Clear reference */
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    
    /* Update statistics */
    queue->total_dequeued++;
    
    /* Signal that queue is not full */
    pthread_cond_signal(&queue->not_full);
    
    pthread_mutex_unlock(&queue->mutex);
    return QUEUE_SUCCESS;
}

/* Non-blocking enqueue */
queue_error_t queue_try_enqueue(concurrent_queue_t *queue, void *data) {
    if (!queue) {
        return QUEUE_ERROR_NULL_POINTER;
    }
    
    pthread_mutex_lock(&queue->mutex);
    
    /* Check if destroyed */
    if (queue->is_destroyed) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_DESTROYED;
    }
    
    /* Check if full */
    if (queue->size >= queue->capacity) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_FULL;
    }
    
    /* Add element to queue */
    queue->buffer[queue->tail] = data;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
    
    /* Update statistics */
    queue->total_enqueued++;
    if (queue->size > queue->max_size_reached) {
        queue->max_size_reached = queue->size;
    }
    
    /* Signal that queue is not empty */
    pthread_cond_signal(&queue->not_empty);
    
    pthread_mutex_unlock(&queue->mutex);
    return QUEUE_SUCCESS;
}

/* Non-blocking dequeue */
queue_error_t queue_try_dequeue(concurrent_queue_t *queue, void **data) {
    if (!queue || !data) {
        return QUEUE_ERROR_NULL_POINTER;
    }
    
    pthread_mutex_lock(&queue->mutex);
    
    /* Check if destroyed */
    if (queue->is_destroyed && queue->size == 0) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_DESTROYED;
    }
    
    /* Check if empty */
    if (queue->size == 0) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_EMPTY;
    }
    
    /* Remove element from queue */
    *data = queue->buffer[queue->head];
    queue->buffer[queue->head] = NULL;
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    
    /* Update statistics */
    queue->total_dequeued++;
    
    /* Signal that queue is not full */
    pthread_cond_signal(&queue->not_full);
    
    pthread_mutex_unlock(&queue->mutex);
    return QUEUE_SUCCESS;
}

/* Timed enqueue operation */
queue_error_t queue_enqueue_timeout(concurrent_queue_t *queue, void *data, int timeout_ms) {
    if (!queue) {
        return QUEUE_ERROR_NULL_POINTER;
    }
    
    /* Handle special timeout values */
    if (timeout_ms == 0) {
        return queue_try_enqueue(queue, data);
    } else if (timeout_ms < 0) {
        return queue_enqueue(queue, data);
    }
    
    pthread_mutex_lock(&queue->mutex);
    
    /* Calculate absolute timeout */
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }
    
    /* Wait with timeout while queue is full */
    int ret = 0;
    while (queue->size >= queue->capacity && !queue->is_destroyed && ret == 0) {
        ret = pthread_cond_timedwait(&queue->not_full, &queue->mutex, &ts);
    }
    
    if (ret == ETIMEDOUT) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_TIMEOUT;
    } else if (ret != 0) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_COND_WAIT;
    }
    
    /* Check if queue was destroyed */
    if (queue->is_destroyed) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_DESTROYED;
    }
    
    /* At this point, there should be space in the queue */
    queue->buffer[queue->tail] = data;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
    
    /* Update statistics */
    queue->total_enqueued++;
    if (queue->size > queue->max_size_reached) {
        queue->max_size_reached = queue->size;
    }
    
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    
    return QUEUE_SUCCESS;
}

/* Helper functions implementation */
size_t queue_size(concurrent_queue_t *queue) {
    if (!queue) return 0;
    
    pthread_mutex_lock(&queue->mutex);
    size_t size = queue->size;
    pthread_mutex_unlock(&queue->mutex);
    
    return size;
}

size_t queue_capacity(concurrent_queue_t *queue) {
    if (!queue) return 0;
    return queue->capacity;  /* Capacity is immutable */
}

bool queue_is_empty(concurrent_queue_t *queue) {
    return queue_size(queue) == 0;
}

bool queue_is_full(concurrent_queue_t *queue) {
    if (!queue) return true;
    return queue_size(queue) >= queue->capacity;
}

/* Error string conversion */
const char* queue_error_string(queue_error_t error) {
    switch (error) {
        case QUEUE_SUCCESS:
            return "Success";
        case QUEUE_ERROR_NULL_POINTER:
            return "Null pointer error";
        case QUEUE_ERROR_INVALID_SIZE:
            return "Invalid size parameter";
        case QUEUE_ERROR_MEMORY_ALLOCATION:
            return "Memory allocation failed";
        case QUEUE_ERROR_FULL:
            return "Queue is full";
        case QUEUE_ERROR_EMPTY:
            return "Queue is empty";
        case QUEUE_ERROR_MUTEX_INIT:
            return "Mutex initialization failed";
        case QUEUE_ERROR_COND_INIT:
            return "Condition variable initialization failed";
        case QUEUE_ERROR_MUTEX_LOCK:
            return "Mutex lock failed";
        case QUEUE_ERROR_COND_WAIT:
            return "Condition wait failed";
        case QUEUE_ERROR_COND_SIGNAL:
            return "Condition signal failed";
        case QUEUE_ERROR_DESTROYED:
            return "Queue has been destroyed";
        case QUEUE_ERROR_TIMEOUT:
            return "Operation timed out";
        default:
            return "Unknown error";
    }
}