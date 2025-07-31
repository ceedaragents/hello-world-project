#include "../src/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define QUEUE_CAPACITY 5
#define NUM_MESSAGES 10

typedef struct {
    int id;
    char message[256];
} Message;

// Producer thread that generates messages
void* producer(void* arg) {
    ConcurrentQueue* queue = (ConcurrentQueue*)arg;
    
    for (int i = 0; i < NUM_MESSAGES; i++) {
        Message* msg = malloc(sizeof(Message));
        msg->id = i;
        snprintf(msg->message, sizeof(msg->message), 
                 "Message %d from producer", i);
        
        printf("[Producer] Sending: %s\n", msg->message);
        queue_enqueue(queue, msg);
        
        // Simulate some work
        usleep(100000); // 100ms
    }
    
    printf("[Producer] Finished sending all messages\n");
    return NULL;
}

// Consumer thread that processes messages
void* consumer(void* arg) {
    ConcurrentQueue* queue = (ConcurrentQueue*)arg;
    
    for (int i = 0; i < NUM_MESSAGES; i++) {
        Message* msg = (Message*)queue_dequeue(queue);
        
        printf("[Consumer] Received: %s\n", msg->message);
        
        // Simulate processing
        usleep(150000); // 150ms
        
        free(msg);
    }
    
    printf("[Consumer] Finished processing all messages\n");
    return NULL;
}

int main() {
    printf("Concurrent Bounded FIFO Queue Example\n");
    printf("====================================\n\n");
    printf("Queue capacity: %d\n", QUEUE_CAPACITY);
    printf("Number of messages: %d\n\n", NUM_MESSAGES);
    
    // Initialize the queue
    ConcurrentQueue queue;
    if (queue_init(&queue, QUEUE_CAPACITY) != 0) {
        fprintf(stderr, "Failed to initialize queue\n");
        return 1;
    }
    
    // Create producer and consumer threads
    pthread_t producer_thread, consumer_thread;
    
    if (pthread_create(&producer_thread, NULL, producer, &queue) != 0) {
        fprintf(stderr, "Failed to create producer thread\n");
        queue_destroy(&queue);
        return 1;
    }
    
    if (pthread_create(&consumer_thread, NULL, consumer, &queue) != 0) {
        fprintf(stderr, "Failed to create consumer thread\n");
        queue_destroy(&queue);
        return 1;
    }
    
    // Wait for threads to complete
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    
    // Verify queue is empty
    printf("\nFinal queue size: %zu\n", queue_size(&queue));
    printf("Queue is empty: %s\n", queue_is_empty(&queue) ? "true" : "false");
    
    // Clean up
    queue_destroy(&queue);
    
    printf("\nExample completed successfully!\n");
    return 0;
}