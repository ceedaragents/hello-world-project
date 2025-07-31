#include "concurrent_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define QUEUE_SIZE 5
#define NUM_MESSAGES 20

typedef struct {
    int id;
    char message[256];
} message_t;

void* producer(void* arg) {
    concurrent_queue_t* queue = (concurrent_queue_t*)arg;
    
    for (int i = 0; i < NUM_MESSAGES; i++) {
        message_t* msg = malloc(sizeof(message_t));
        msg->id = i;
        snprintf(msg->message, sizeof(msg->message), "Message #%d from producer", i);
        
        printf("Producer: Sending %s\n", msg->message);
        queue_error_t err = queue_enqueue(queue, msg);
        if (err != QUEUE_SUCCESS) {
            printf("Producer: Failed to send message: %s\n", queue_error_string(err));
            free(msg);
            break;
        }
        
        usleep(100000);
    }
    
    printf("Producer: Done sending messages\n");
    return NULL;
}

void* consumer(void* arg) {
    concurrent_queue_t* queue = (concurrent_queue_t*)arg;
    
    for (int i = 0; i < NUM_MESSAGES; i++) {
        message_t* msg = NULL;
        queue_error_t err = queue_dequeue(queue, (void**)&msg);
        
        if (err != QUEUE_SUCCESS) {
            printf("Consumer: Failed to receive message: %s\n", queue_error_string(err));
            break;
        }
        
        printf("Consumer: Received %s\n", msg->message);
        free(msg);
        
        usleep(150000);
    }
    
    printf("Consumer: Done receiving messages\n");
    return NULL;
}

int main() {
    printf("Concurrent Queue Example\n");
    printf("========================\n");
    printf("Queue capacity: %d\n", QUEUE_SIZE);
    printf("Number of messages: %d\n\n", NUM_MESSAGES);
    
    concurrent_queue_t* queue = queue_create(QUEUE_SIZE);
    if (!queue) {
        fprintf(stderr, "Failed to create queue\n");
        return 1;
    }
    
    pthread_t producer_thread, consumer_thread;
    
    if (pthread_create(&producer_thread, NULL, producer, queue) != 0) {
        fprintf(stderr, "Failed to create producer thread\n");
        queue_destroy(queue);
        return 1;
    }
    
    if (pthread_create(&consumer_thread, NULL, consumer, queue) != 0) {
        fprintf(stderr, "Failed to create consumer thread\n");
        pthread_join(producer_thread, NULL);
        queue_destroy(queue);
        return 1;
    }
    
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    
    queue_stats_t stats;
    queue_get_stats(queue, &stats);
    
    printf("\nQueue Statistics:\n");
    printf("  Total enqueued: %zu\n", stats.total_enqueued);
    printf("  Total dequeued: %zu\n", stats.total_dequeued);
    printf("  Max size reached: %zu\n", stats.max_size_reached);
    printf("  Final size: %zu\n", stats.current_size);
    
    queue_destroy(queue);
    
    return 0;
}