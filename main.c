#include "matrix_multiplication.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void test_correctness(size_t n) {
    printf("\n=== Testing Correctness (n=%zu) ===\n", n);
    
    Matrix *a = matrix_create(n, n);
    Matrix *b = matrix_create(n, n);
    Matrix *c_naive = matrix_create(n, n);
    Matrix *c_cache_oblivious = matrix_create(n, n);
    
    if (!a || !b || !c_naive || !c_cache_oblivious) {
        printf("Failed to allocate matrices\n");
        goto cleanup;
    }
    
    matrix_fill_random(a, -10.0, 10.0);
    matrix_fill_random(b, -10.0, 10.0);
    
    printf("Computing naive multiplication...\n");
    matrix_multiply_naive(a, b, c_naive);
    
    printf("Computing cache-oblivious multiplication...\n");
    matrix_multiply_cache_oblivious(a, b, c_cache_oblivious);
    
    if (matrix_equals(c_naive, c_cache_oblivious, 1e-10)) {
        printf("✓ Results match!\n");
    } else {
        printf("✗ Results do not match!\n");
        
        if (n <= 4) {
            printf("\nMatrix A:\n");
            matrix_print(a);
            printf("\nMatrix B:\n");
            matrix_print(b);
            printf("\nNaive result:\n");
            matrix_print(c_naive);
            printf("\nCache-oblivious result:\n");
            matrix_print(c_cache_oblivious);
        }
    }
    
cleanup:
    matrix_destroy(a);
    matrix_destroy(b);
    matrix_destroy(c_naive);
    matrix_destroy(c_cache_oblivious);
}

void benchmark_performance(size_t n, int iterations) {
    printf("\n=== Benchmarking Performance (n=%zu, iterations=%d) ===\n", n, iterations);
    
    Matrix *a = matrix_create(n, n);
    Matrix *b = matrix_create(n, n);
    Matrix *c = matrix_create(n, n);
    
    if (!a || !b || !c) {
        printf("Failed to allocate matrices\n");
        goto cleanup;
    }
    
    matrix_fill_random(a, -10.0, 10.0);
    matrix_fill_random(b, -10.0, 10.0);
    
    printf("\nNaive multiplication:\n");
    double total_time_naive = 0.0;
    for (int i = 0; i < iterations; i++) {
        double start = get_time_in_seconds();
        matrix_multiply_naive(a, b, c);
        double end = get_time_in_seconds();
        total_time_naive += (end - start);
    }
    double avg_time_naive = total_time_naive / iterations;
    double gflops_naive = calculate_gflops(n, avg_time_naive);
    printf("  Average time: %.4f seconds\n", avg_time_naive);
    printf("  Performance: %.2f GFLOPS\n", gflops_naive);
    
    printf("\nCache-oblivious multiplication:\n");
    double total_time_co = 0.0;
    for (int i = 0; i < iterations; i++) {
        double start = get_time_in_seconds();
        matrix_multiply_cache_oblivious(a, b, c);
        double end = get_time_in_seconds();
        total_time_co += (end - start);
    }
    double avg_time_co = total_time_co / iterations;
    double gflops_co = calculate_gflops(n, avg_time_co);
    printf("  Average time: %.4f seconds\n", avg_time_co);
    printf("  Performance: %.2f GFLOPS\n", gflops_co);
    
    printf("\nSpeedup: %.2fx\n", avg_time_naive / avg_time_co);
    printf("Performance improvement: %.1f%%\n", 
           ((gflops_co - gflops_naive) / gflops_naive) * 100);
    
cleanup:
    matrix_destroy(a);
    matrix_destroy(b);
    matrix_destroy(c);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    printf("Cache-Oblivious Matrix Multiplication\n");
    printf("=====================================\n");
    
    test_correctness(4);
    test_correctness(16);
    test_correctness(64);
    test_correctness(128);
    
    benchmark_performance(256, 5);
    benchmark_performance(512, 3);
    benchmark_performance(1024, 1);
    
    if (argc > 1) {
        size_t custom_size = atoi(argv[1]);
        if (custom_size > 0) {
            printf("\n=== Custom size benchmark ===\n");
            benchmark_performance(custom_size, 1);
        }
    }
    
    return 0;
}