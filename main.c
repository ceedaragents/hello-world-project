#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix_multiply.h"

// Test matrix multiplication correctness
void test_correctness(size_t N) {
    printf("\n=== Testing Correctness (N=%zu) ===\n", N);
    
    // Allocate matrices
    double* A = allocate_matrix(N);
    double* B = allocate_matrix(N);
    double* C_naive = allocate_matrix(N);
    double* C_blocked = allocate_matrix(N);
    double* C_efficient = allocate_matrix(N);
    
    if (!A || !B || !C_naive || !C_blocked || !C_efficient) {
        fprintf(stderr, "Failed to allocate matrices\n");
        return;
    }
    
    // Initialize matrices
    init_matrix_random(A, N, 42);
    init_matrix_random(B, N, 123);
    
    // Test naive implementation
    printf("Testing naive implementation...\n");
    naive_matmul(A, B, C_naive, N);
    
    // Test blocked implementation with different block sizes
    printf("Testing blocked implementation (block_size=32)...\n");
    blocked_matmul(A, B, C_blocked, N, 32);
    
    // Test cache-efficient implementation
    printf("Testing cache-efficient implementation...\n");
    cache_efficient_matmul(A, B, C_efficient, N);
    
    // Compare results
    double tolerance = 1e-10;
    if (compare_matrices(C_naive, C_blocked, N, tolerance)) {
        printf("✓ Blocked implementation matches naive implementation\n");
    } else {
        printf("✗ Blocked implementation differs from naive implementation\n");
    }
    
    if (compare_matrices(C_naive, C_efficient, N, tolerance)) {
        printf("✓ Cache-efficient implementation matches naive implementation\n");
    } else {
        printf("✗ Cache-efficient implementation differs from naive implementation\n");
    }
    
    // Print small portion of result
    if (N <= 10) {
        printf("\nResult matrix C (A × B):\n");
        print_matrix(C_efficient, N, 10);
    }
    
    // Cleanup
    free_matrix(A);
    free_matrix(B);
    free_matrix(C_naive);
    free_matrix(C_blocked);
    free_matrix(C_efficient);
}

// Wrapper functions for blocked multiplication with fixed block sizes
static void blocked_matmul_16(const double* A, const double* B, double* C, size_t N) {
    blocked_matmul(A, B, C, N, 16);
}

static void blocked_matmul_32(const double* A, const double* B, double* C, size_t N) {
    blocked_matmul(A, B, C, N, 32);
}

static void blocked_matmul_64(const double* A, const double* B, double* C, size_t N) {
    blocked_matmul(A, B, C, N, 64);
}

// Benchmark different implementations
void benchmark_implementations(size_t N, int iterations) {
    printf("\n=== Benchmarking (N=%zu, %d iterations) ===\n", N, iterations);
    
    // Allocate matrices
    double* A = allocate_matrix(N);
    double* B = allocate_matrix(N);
    double* C = allocate_matrix(N);
    
    if (!A || !B || !C) {
        fprintf(stderr, "Failed to allocate matrices for benchmarking\n");
        return;
    }
    
    // Initialize matrices
    init_matrix_random(A, N, 42);
    init_matrix_random(B, N, 123);
    
    // Benchmark naive implementation (only for small matrices)
    if (N <= 512) {
        printf("\nNaive implementation:\n");
        benchmark_matmul(naive_matmul, A, B, C, N, iterations);
    }
    
    // Benchmark blocked implementation with different block sizes
    printf("\nBlocked implementation (block_size=16):\n");
    benchmark_matmul(blocked_matmul_16, A, B, C, N, iterations);
    
    printf("\nBlocked implementation (block_size=32):\n");
    benchmark_matmul(blocked_matmul_32, A, B, C, N, iterations);
    
    printf("\nBlocked implementation (block_size=64):\n");
    benchmark_matmul(blocked_matmul_64, A, B, C, N, iterations);
    
    // Benchmark cache-efficient implementation
    printf("\nCache-efficient implementation (auto-tuned):\n");
    benchmark_matmul(cache_efficient_matmul, A, B, C, N, iterations);
    
    // Cleanup
    free_matrix(A);
    free_matrix(B);
    free_matrix(C);
}

// Performance test with increasing matrix sizes
void performance_scaling_test() {
    printf("\n=== Performance Scaling Test ===\n");
    printf("Testing how performance scales with matrix size...\n");
    
    size_t sizes[] = {64, 128, 256, 512, 1024};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < num_sizes; i++) {
        size_t N = sizes[i];
        int iterations = (N <= 256) ? 5 : 1;
        
        printf("\n--- Matrix size: %zu×%zu ---\n", N, N);
        
        // Allocate matrices
        double* A = allocate_matrix(N);
        double* B = allocate_matrix(N);
        double* C = allocate_matrix(N);
        
        if (!A || !B || !C) {
            fprintf(stderr, "Failed to allocate matrices of size %zu\n", N);
            continue;
        }
        
        // Initialize matrices
        init_matrix_random(A, N, 42);
        init_matrix_random(B, N, 123);
        
        // Benchmark cache-efficient implementation
        printf("Cache-efficient implementation:\n");
        benchmark_matmul(cache_efficient_matmul, A, B, C, N, iterations);
        
        // Cleanup
        free_matrix(A);
        free_matrix(B);
        free_matrix(C);
    }
}

int main(int argc, char* argv[]) {
    printf("Cache-Efficient Matrix Multiplication in C\n");
    printf("==========================================\n");
    
    // Test correctness with small matrices
    test_correctness(8);
    test_correctness(64);
    
    // Benchmark different implementations
    benchmark_implementations(256, 5);
    benchmark_implementations(512, 2);
    
    // Performance scaling test
    performance_scaling_test();
    
    return 0;
}