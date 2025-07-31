#include "../src/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#define TEST_CAPACITY 10
#define NUM_PRODUCERS 4
#define NUM_CONSUMERS 4
#define ITEMS_PER_THREAD 100

typedef struct {
    ConcurrentQueue* queue;
    int thread_id;
    int num_items;
} ThreadData;

// Test basic queue operations
void test_basic_operations() {
    printf("Running test_basic_operations...\n");
    
    ConcurrentQueue queue;
    assert(queue_init(&queue, TEST_CAPACITY) == 0);
    
    // Test empty queue
    assert(queue_is_empty(&queue) == true);
    assert(queue_is_full(&queue) == false);
    assert(queue_size(&queue) == 0);
    
    // Test enqueue
    int data1 = 42;
    assert(queue_enqueue(&queue, &data1) == 0);
    assert(queue_size(&queue) == 1);
    assert(queue_is_empty(&queue) == false);
    
    // Test dequeue
    int* result = (int*)queue_dequeue(&queue);
    assert(result != NULL);
    assert(*result == 42);
    assert(queue_size(&queue) == 0);
    assert(queue_is_empty(&queue) == true);
    
    queue_destroy(&queue);
    printf("test_basic_operations PASSED\n\n");
}

// Test queue capacity limits
void test_capacity_limits() {
    printf("Running test_capacity_limits...\n");
    
    ConcurrentQueue queue;
    assert(queue_init(&queue, 3) == 0);
    
    int data[] = {1, 2, 3, 4};
    
    // Fill the queue to capacity
    assert(queue_enqueue(&queue, &data[0]) == 0);
    assert(queue_enqueue(&queue, &data[1]) == 0);
    assert(queue_enqueue(&queue, &data[2]) == 0);
    assert(queue_is_full(&queue) == true);
    
    // Try non-blocking enqueue when full
    assert(queue_try_enqueue(&queue, &data[3]) == -1);
    
    // Dequeue one item
    int* result = (int*)queue_dequeue(&queue);
    assert(*result == 1);
    assert(queue_is_full(&queue) == false);
    
    // Now we can enqueue again
    assert(queue_enqueue(&queue, &data[3]) == 0);
    assert(queue_is_full(&queue) == true);
    
    queue_destroy(&queue);
    printf("test_capacity_limits PASSED\n\n");
}

// Test FIFO ordering
void test_fifo_ordering() {
    printf("Running test_fifo_ordering...\n");
    
    ConcurrentQueue queue;
    assert(queue_init(&queue, 5) == 0);
    
    int data[] = {10, 20, 30, 40, 50};
    
    // Enqueue multiple items
    for (int i = 0; i < 5; i++) {
        assert(queue_enqueue(&queue, &data[i]) == 0);
    }
    
    // Dequeue and verify FIFO order
    for (int i = 0; i < 5; i++) {
        int* result = (int*)queue_dequeue(&queue);
        assert(result != NULL);
        assert(*result == data[i]);
    }
    
    queue_destroy(&queue);
    printf("test_fifo_ordering PASSED\n\n");
}

// Test non-blocking operations
void test_non_blocking_operations() {
    printf("Running test_non_blocking_operations...\n");
    
    ConcurrentQueue queue;
    assert(queue_init(&queue, 2) == 0);
    
    // Try dequeue on empty queue
    assert(queue_try_dequeue(&queue) == NULL);
    
    int data1 = 100, data2 = 200, data3 = 300;
    
    // Fill queue
    assert(queue_try_enqueue(&queue, &data1) == 0);
    assert(queue_try_enqueue(&queue, &data2) == 0);
    
    // Try enqueue on full queue
    assert(queue_try_enqueue(&queue, &data3) == -1);
    
    // Dequeue and verify
    int* result1 = (int*)queue_try_dequeue(&queue);
    assert(result1 != NULL && *result1 == 100);
    
    int* result2 = (int*)queue_try_dequeue(&queue);
    assert(result2 != NULL && *result2 == 200);
    
    // Try dequeue on empty queue again
    assert(queue_try_dequeue(&queue) == NULL);
    
    queue_destroy(&queue);
    printf("test_non_blocking_operations PASSED\n\n");
}

// Producer thread function
void* producer_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    for (int i = 0; i < data->num_items; i++) {
        int* item = malloc(sizeof(int));
        *item = data->thread_id * 1000 + i;
        
        if (queue_enqueue(data->queue, item) != 0) {
            fprintf(stderr, "Producer %d: Failed to enqueue item %d\n", 
                    data->thread_id, i);
            free(item);
        }
        
        // Small random delay
        usleep(rand() % 1000);
    }
    
    return NULL;
}

// Consumer thread function
void* consumer_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int count = 0;
    
    for (int i = 0; i < data->num_items; i++) {
        int* item = (int*)queue_dequeue(data->queue);
        if (item != NULL) {
            count++;
            free(item);
        }
        
        // Small random delay
        usleep(rand() % 1000);
    }
    
    return (void*)(intptr_t)count;
}

// Test concurrent access
void test_concurrent_access() {
    printf("Running test_concurrent_access...\n");
    
    ConcurrentQueue queue;
    assert(queue_init(&queue, 50) == 0);
    
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    ThreadData producer_data[NUM_PRODUCERS];
    ThreadData consumer_data[NUM_CONSUMERS];
    
    // Create producer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_data[i].queue = &queue;
        producer_data[i].thread_id = i;
        producer_data[i].num_items = ITEMS_PER_THREAD;
        pthread_create(&producers[i], NULL, producer_thread, &producer_data[i]);
    }
    
    // Create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumer_data[i].queue = &queue;
        consumer_data[i].thread_id = i;
        consumer_data[i].num_items = ITEMS_PER_THREAD;
        pthread_create(&consumers[i], NULL, consumer_thread, &consumer_data[i]);
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    
    int total_consumed = 0;
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        void* result;
        pthread_join(consumers[i], &result);
        total_consumed += (int)(intptr_t)result;
    }
    
    // Verify all items were consumed
    assert(total_consumed == NUM_PRODUCERS * ITEMS_PER_THREAD);
    assert(queue_is_empty(&queue) == true);
    
    queue_destroy(&queue);
    printf("test_concurrent_access PASSED\n\n");
}

// Test error handling
void test_error_handling() {
    printf("Running test_error_handling...\n");
    
    ConcurrentQueue queue;
    
    // Test NULL queue
    assert(queue_init(NULL, 10) == -1);
    
    // Test zero capacity
    assert(queue_init(&queue, 0) == -1);
    
    // Test operations on NULL queue
    assert(queue_enqueue(NULL, (void*)1) == -1);
    assert(queue_dequeue(NULL) == NULL);
    assert(queue_try_enqueue(NULL, (void*)1) == -1);
    assert(queue_try_dequeue(NULL) == NULL);
    assert(queue_size(NULL) == 0);
    assert(queue_is_empty(NULL) == true);
    assert(queue_is_full(NULL) == true);
    
    // Test destroy NULL queue (should not crash)
    queue_destroy(NULL);
    
    printf("test_error_handling PASSED\n\n");
}

int main() {
    printf("Running Concurrent Bounded FIFO Queue Tests\n");
    printf("==========================================\n\n");
    
    test_basic_operations();
    test_capacity_limits();
    test_fifo_ordering();
    test_non_blocking_operations();
    test_error_handling();
    test_concurrent_access();
    
    printf("All tests PASSED!\n");
    return 0;
}