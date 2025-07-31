#include "concurrent_queue.h"
#include <stdio.h>
#include <assert.h>

int main() {
    printf("Testing basic operations...\n");
    
    concurrent_queue_t* queue = queue_create(5);
    assert(queue != NULL);
    printf("Queue created\n");
    
    int data = 42;
    queue_error_t err = queue_enqueue(queue, &data);
    printf("Enqueue result: %s\n", queue_error_string(err));
    
    int* retrieved = NULL;
    err = queue_dequeue(queue, (void**)&retrieved);
    printf("Dequeue result: %s, value: %d\n", queue_error_string(err), retrieved ? *retrieved : -1);
    
    queue_destroy(queue);
    printf("Queue destroyed\n");
    
    return 0;
}