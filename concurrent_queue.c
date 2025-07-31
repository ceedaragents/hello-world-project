#include "concurrent_queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct concurrent_queue {
    void** buffer;
    size_t capacity;
    size_t size;
    size_t head;
    size_t tail;
    
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
    
    bool is_destroyed;
    
    size_t total_enqueued;
    size_t total_dequeued;
    size_t max_size_reached;
};

concurrent_queue_t* queue_create(size_t capacity) {
    if (capacity == 0) {
        return NULL;
    }
    
    concurrent_queue_t* queue = calloc(1, sizeof(concurrent_queue_t));
    if (!queue) {
        return NULL;
    }
    
    queue->buffer = calloc(capacity, sizeof(void*));
    if (!queue->buffer) {
        free(queue);
        return NULL;
    }
    
    queue->capacity = capacity;
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;
    queue->is_destroyed = false;
    
    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        free(queue->buffer);
        free(queue);
        return NULL;
    }
    
    if (pthread_cond_init(&queue->not_full, NULL) != 0) {
        pthread_mutex_destroy(&queue->mutex);
        free(queue->buffer);
        free(queue);
        return NULL;
    }
    
    if (pthread_cond_init(&queue->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&queue->mutex);
        pthread_cond_destroy(&queue->not_full);
        free(queue->buffer);
        free(queue);
        return NULL;
    }
    
    return queue;
}

queue_error_t queue_destroy(concurrent_queue_t* queue) {
    if (!queue) {
        return QUEUE_ERROR_NULL_PARAM;
    }
    
    pthread_mutex_lock(&queue->mutex);
    queue->is_destroyed = true;
    pthread_cond_broadcast(&queue->not_full);
    pthread_cond_broadcast(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_full);
    pthread_cond_destroy(&queue->not_empty);
    
    free(queue->buffer);
    free(queue);
    
    return QUEUE_SUCCESS;
}

queue_error_t queue_enqueue(concurrent_queue_t* queue, void* data) {
    if (!queue || !data) {
        return QUEUE_ERROR_NULL_PARAM;
    }
    
    pthread_mutex_lock(&queue->mutex);
    
    while (queue->size == queue->capacity && !queue->is_destroyed) {
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }
    
    if (queue->is_destroyed) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_DESTROYED;
    }
    
    queue->buffer[queue->tail] = data;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
    queue->total_enqueued++;
    
    if (queue->size > queue->max_size_reached) {
        queue->max_size_reached = queue->size;
    }
    
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    
    return QUEUE_SUCCESS;
}

queue_error_t queue_dequeue(concurrent_queue_t* queue, void** data) {
    if (!queue || !data) {
        return QUEUE_ERROR_NULL_PARAM;
    }
    
    pthread_mutex_lock(&queue->mutex);
    
    while (queue->size == 0 && !queue->is_destroyed) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }
    
    if (queue->is_destroyed && queue->size == 0) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_DESTROYED;
    }
    
    *data = queue->buffer[queue->head];
    queue->buffer[queue->head] = NULL;
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    queue->total_dequeued++;
    
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);
    
    return QUEUE_SUCCESS;
}

queue_error_t queue_try_enqueue(concurrent_queue_t* queue, void* data) {
    if (!queue || !data) {
        return QUEUE_ERROR_NULL_PARAM;
    }
    
    pthread_mutex_lock(&queue->mutex);
    
    if (queue->is_destroyed) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_DESTROYED;
    }
    
    if (queue->size == queue->capacity) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_FULL;
    }
    
    queue->buffer[queue->tail] = data;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
    queue->total_enqueued++;
    
    if (queue->size > queue->max_size_reached) {
        queue->max_size_reached = queue->size;
    }
    
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    
    return QUEUE_SUCCESS;
}

queue_error_t queue_try_dequeue(concurrent_queue_t* queue, void** data) {
    if (!queue || !data) {
        return QUEUE_ERROR_NULL_PARAM;
    }
    
    pthread_mutex_lock(&queue->mutex);
    
    if (queue->is_destroyed) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_DESTROYED;
    }
    
    if (queue->size == 0) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_EMPTY;
    }
    
    *data = queue->buffer[queue->head];
    queue->buffer[queue->head] = NULL;
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    queue->total_dequeued++;
    
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);
    
    return QUEUE_SUCCESS;
}

queue_error_t queue_enqueue_timeout(concurrent_queue_t* queue, void* data, const struct timespec* timeout) {
    if (!queue || !data || !timeout) {
        return QUEUE_ERROR_NULL_PARAM;
    }
    
    pthread_mutex_lock(&queue->mutex);
    
    int result = 0;
    while (queue->size == queue->capacity && !queue->is_destroyed && result == 0) {
        result = pthread_cond_timedwait(&queue->not_full, &queue->mutex, timeout);
    }
    
    if (queue->is_destroyed) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_DESTROYED;
    }
    
    if (result == ETIMEDOUT) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_TIMEOUT;
    }
    
    if (result != 0) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_SYSTEM;
    }
    
    if (queue->size == queue->capacity) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_TIMEOUT;
    }
    
    queue->buffer[queue->tail] = data;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
    queue->total_enqueued++;
    
    if (queue->size > queue->max_size_reached) {
        queue->max_size_reached = queue->size;
    }
    
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    
    return QUEUE_SUCCESS;
}

queue_error_t queue_dequeue_timeout(concurrent_queue_t* queue, void** data, const struct timespec* timeout) {
    if (!queue || !data || !timeout) {
        return QUEUE_ERROR_NULL_PARAM;
    }
    
    pthread_mutex_lock(&queue->mutex);
    
    int result = 0;
    while (queue->size == 0 && !queue->is_destroyed && result == 0) {
        result = pthread_cond_timedwait(&queue->not_empty, &queue->mutex, timeout);
    }
    
    if (queue->is_destroyed && queue->size == 0) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_DESTROYED;
    }
    
    if (result == ETIMEDOUT) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_TIMEOUT;
    }
    
    if (result != 0) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_SYSTEM;
    }
    
    if (queue->size == 0) {
        pthread_mutex_unlock(&queue->mutex);
        return QUEUE_ERROR_TIMEOUT;
    }
    
    *data = queue->buffer[queue->head];
    queue->buffer[queue->head] = NULL;
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    queue->total_dequeued++;
    
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);
    
    return QUEUE_SUCCESS;
}

size_t queue_size(concurrent_queue_t* queue) {
    if (!queue) {
        return 0;
    }
    
    pthread_mutex_lock(&queue->mutex);
    size_t size = queue->size;
    pthread_mutex_unlock(&queue->mutex);
    
    return size;
}

size_t queue_capacity(concurrent_queue_t* queue) {
    if (!queue) {
        return 0;
    }
    
    return queue->capacity;
}

bool queue_is_empty(concurrent_queue_t* queue) {
    if (!queue) {
        return true;
    }
    
    pthread_mutex_lock(&queue->mutex);
    bool empty = (queue->size == 0);
    pthread_mutex_unlock(&queue->mutex);
    
    return empty;
}

bool queue_is_full(concurrent_queue_t* queue) {
    if (!queue) {
        return false;
    }
    
    pthread_mutex_lock(&queue->mutex);
    bool full = (queue->size == queue->capacity);
    pthread_mutex_unlock(&queue->mutex);
    
    return full;
}

queue_error_t queue_get_stats(concurrent_queue_t* queue, queue_stats_t* stats) {
    if (!queue || !stats) {
        return QUEUE_ERROR_NULL_PARAM;
    }
    
    pthread_mutex_lock(&queue->mutex);
    stats->total_enqueued = queue->total_enqueued;
    stats->total_dequeued = queue->total_dequeued;
    stats->max_size_reached = queue->max_size_reached;
    stats->current_size = queue->size;
    stats->capacity = queue->capacity;
    pthread_mutex_unlock(&queue->mutex);
    
    return QUEUE_SUCCESS;
}

const char* queue_error_string(queue_error_t error) {
    switch (error) {
        case QUEUE_SUCCESS: return "Success";
        case QUEUE_ERROR_NULL_PARAM: return "Null parameter";
        case QUEUE_ERROR_NO_MEMORY: return "Out of memory";
        case QUEUE_ERROR_FULL: return "Queue is full";
        case QUEUE_ERROR_EMPTY: return "Queue is empty";
        case QUEUE_ERROR_DESTROYED: return "Queue is destroyed";
        case QUEUE_ERROR_TIMEOUT: return "Operation timed out";
        case QUEUE_ERROR_SYSTEM: return "System error";
        default: return "Unknown error";
    }
}