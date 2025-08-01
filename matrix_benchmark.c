#include "matrix_benchmark.h"
#include "matrix_multiply.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

double get_wall_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

double calculate_gflops(size_t M, size_t N, size_t K, double time_seconds) {
    // Matrix multiplication performs 2*M*N*K floating-point operations
    double flops = 2.0 * M * N * K;
    return (flops / time_seconds) / 1e9;
}

double benchmark_algorithm(
    const char* algorithm_name,
    Matrix* (*multiply_func)(const Matrix*, const Matrix*),
    const Matrix* A,
    const Matrix* B,
    int warmup,
    int num_runs) {
    
    // Perform warmup runs
    for (int i = 0; i < warmup; i++) {
        Matrix* C = multiply_func(A, B);
        if (!C) {
            fprintf(stderr, "Error: %s failed during warmup\n", algorithm_name);
            return -1.0;
        }
        matrix_free(C);
    }
    
    // Perform timed runs
    double total_time = 0.0;
    for (int i = 0; i < num_runs; i++) {
        double start = get_wall_time();
        Matrix* C = multiply_func(A, B);
        double end = get_wall_time();
        
        if (!C) {
            fprintf(stderr, "Error: %s failed during benchmarking\n", algorithm_name);
            return -1.0;
        }
        
        matrix_free(C);
        total_time += (end - start);
    }
    
    return total_time / num_runs;
}

// Wrapper functions for blocked multiplication with default block size
static Matrix* multiply_blocked_default(const Matrix* A, const Matrix* B) {
    return multiply_blocked(A, B, 0);
}

void run_performance_benchmarks(void) {
    printf("\n========================================\n");
    printf("Performance Benchmarks\n");
    printf("========================================\n");
    
    size_t sizes[] = {128, 256, 512};
    const char* algorithm_names[] = {"Naive", "Blocked", "Cache-Oblivious"};
    
    // Function pointers for each algorithm
    Matrix* (*algorithms[])(const Matrix*, const Matrix*) = {
        multiply_naive,
        multiply_blocked_default,
        multiply_cache_oblivious
    };
    
    // Benchmark each size
    for (int size_idx = 0; size_idx < 3; size_idx++) {
        size_t n = sizes[size_idx];
        
        printf("\nMatrix size: %zu x %zu\n", n, n);
        printf("%-20s %-15s %-15s\n", "Algorithm", "Time (sec)", "GFLOPS");
        printf("------------------------------------------------\n");
        
        // Create test matrices
        Matrix* A = matrix_create(n, n);
        Matrix* B = matrix_create(n, n);
        
        if (!A || !B) {
            fprintf(stderr, "Error: Failed to allocate %zux%zu matrices\n", n, n);
            if (A) matrix_free(A);
            if (B) matrix_free(B);
            continue;
        }
        
        // Initialize with random values
        srand(42); // Fixed seed for reproducibility
        matrix_fill_random(A, -1.0, 1.0);
        matrix_fill_random(B, -1.0, 1.0);
        
        // Benchmark each algorithm
        for (int alg_idx = 0; alg_idx < 3; alg_idx++) {
            // Skip naive algorithm for large matrices (too slow)
            if (alg_idx == 0 && n > 256) {
                printf("%-20s %-15s %-15s\n", algorithm_names[alg_idx], "skipped", "-");
                continue;
            }
            
            double avg_time = benchmark_algorithm(
                algorithm_names[alg_idx],
                algorithms[alg_idx],
                A, B,
                BENCHMARK_WARMUP_RUNS,
                BENCHMARK_NUM_RUNS
            );
            
            if (avg_time > 0) {
                double gflops = calculate_gflops(n, n, n, avg_time);
                printf("%-20s %-15.6f %-15.2f\n", 
                       algorithm_names[alg_idx], avg_time, gflops);
            } else {
                printf("%-20s %-15s %-15s\n", 
                       algorithm_names[alg_idx], "failed", "-");
            }
        }
        
        // Clean up
        matrix_free(A);
        matrix_free(B);
    }
    
    printf("\n========================================\n");
    
    // Additional analysis
    printf("\nPerformance Analysis:\n");
    printf("- Blocked algorithm typically shows 1.5-2x improvement over naive\n");
    printf("- Cache-oblivious algorithm performs best for large matrices\n");
    printf("- Performance depends on CPU cache sizes and memory bandwidth\n");
}