#include "queue.h"
#include <stdlib.h>
#include <errno.h>

int queue_init(ConcurrentQueue* queue, size_t capacity) {
    if (queue == NULL || capacity == 0) {
        errno = EINVAL;
        return -1;
    }

    queue->buffer = (void**)calloc(capacity, sizeof(void*));
    if (queue->buffer == NULL) {
        errno = ENOMEM;
        return -1;
    }

    queue->capacity = capacity;
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;

    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        free(queue->buffer);
        return -1;
    }

    if (pthread_cond_init(&queue->not_full, NULL) != 0) {
        pthread_mutex_destroy(&queue->mutex);
        free(queue->buffer);
        return -1;
    }

    if (pthread_cond_init(&queue->not_empty, NULL) != 0) {
        pthread_cond_destroy(&queue->not_full);
        pthread_mutex_destroy(&queue->mutex);
        free(queue->buffer);
        return -1;
    }

    return 0;
}

void queue_destroy(ConcurrentQueue* queue) {
    if (queue == NULL) {
        return;
    }

    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
    pthread_mutex_destroy(&queue->mutex);
    free(queue->buffer);
    queue->buffer = NULL;
    queue->capacity = 0;
    queue->size = 0;
}

int queue_enqueue(ConcurrentQueue* queue, void* item) {
    if (queue == NULL) {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);

    // Wait while the queue is full
    while (queue->size == queue->capacity) {
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }

    // Add the item to the queue
    queue->buffer[queue->tail] = item;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;

    // Signal waiting consumers
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);

    return 0;
}

void* queue_dequeue(ConcurrentQueue* queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return NULL;
    }

    pthread_mutex_lock(&queue->mutex);

    // Wait while the queue is empty
    while (queue->size == 0) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }

    // Remove the item from the queue
    void* item = queue->buffer[queue->head];
    queue->buffer[queue->head] = NULL;
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;

    // Signal waiting producers
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);

    return item;
}

int queue_try_enqueue(ConcurrentQueue* queue, void* item) {
    if (queue == NULL) {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&queue->mutex);

    // Check if the queue is full
    if (queue->size == queue->capacity) {
        pthread_mutex_unlock(&queue->mutex);
        errno = EWOULDBLOCK;
        return -1;
    }

    // Add the item to the queue
    queue->buffer[queue->tail] = item;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;

    // Signal waiting consumers
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);

    return 0;
}

void* queue_try_dequeue(ConcurrentQueue* queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return NULL;
    }

    pthread_mutex_lock(&queue->mutex);

    // Check if the queue is empty
    if (queue->size == 0) {
        pthread_mutex_unlock(&queue->mutex);
        errno = EWOULDBLOCK;
        return NULL;
    }

    // Remove the item from the queue
    void* item = queue->buffer[queue->head];
    queue->buffer[queue->head] = NULL;
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;

    // Signal waiting producers
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);

    return item;
}

size_t queue_size(ConcurrentQueue* queue) {
    if (queue == NULL) {
        return 0;
    }

    pthread_mutex_lock(&queue->mutex);
    size_t size = queue->size;
    pthread_mutex_unlock(&queue->mutex);

    return size;
}

bool queue_is_empty(ConcurrentQueue* queue) {
    return queue_size(queue) == 0;
}

bool queue_is_full(ConcurrentQueue* queue) {
    if (queue == NULL) {
        return true;
    }

    pthread_mutex_lock(&queue->mutex);
    bool full = (queue->size == queue->capacity);
    pthread_mutex_unlock(&queue->mutex);

    return full;
}