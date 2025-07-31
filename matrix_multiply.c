#include "matrix_multiply.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define L1_CACHE_LINE_SIZE 64
#define L1_CACHE_SIZE (32 * 1024)  // 32KB L1 cache
#define L2_CACHE_SIZE (256 * 1024) // 256KB L2 cache

// Naive matrix multiplication
void naive_matmul(const double* A, const double* B, double* C, size_t N) {
    memset(C, 0, N * N * sizeof(double));
    
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < N; k++) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

// Blocked matrix multiplication with configurable block size
void blocked_matmul(const double* A, const double* B, double* C, size_t N, size_t block_size) {
    memset(C, 0, N * N * sizeof(double));
    
    // Triple-nested blocking loops
    for (size_t i0 = 0; i0 < N; i0 += block_size) {
        size_t imax = MIN(i0 + block_size, N);
        
        for (size_t j0 = 0; j0 < N; j0 += block_size) {
            size_t jmax = MIN(j0 + block_size, N);
            
            for (size_t k0 = 0; k0 < N; k0 += block_size) {
                size_t kmax = MIN(k0 + block_size, N);
                
                // Compute the block - inner loops with better cache locality
                for (size_t i = i0; i < imax; i++) {
                    for (size_t k = k0; k < kmax; k++) {
                        double a_ik = A[i * N + k];
                        for (size_t j = j0; j < jmax; j++) {
                            C[i * N + j] += a_ik * B[k * N + j];
                        }
                    }
                }
            }
        }
    }
}

// Cache-efficient matrix multiplication with automatic block size selection
void cache_efficient_matmul(const double* A, const double* B, double* C, size_t N) {
    // Calculate optimal block size based on L1 cache
    // We need to fit 3 blocks (from A, B, and C) in cache
    // Each block has block_size^2 doubles (8 bytes each)
    size_t optimal_block_size = (size_t)sqrt(L1_CACHE_SIZE / (3.0 * sizeof(double)));
    
    // Align block size to cache line boundaries for better performance
    optimal_block_size = (optimal_block_size / 8) * 8;
    
    // Ensure minimum block size
    if (optimal_block_size < 16) {
        optimal_block_size = 16;
    }
    
    // For small matrices, use naive implementation
    if (N <= optimal_block_size) {
        naive_matmul(A, B, C, N);
        return;
    }
    
    // Use blocked multiplication with optimal block size
    blocked_matmul(A, B, C, N, optimal_block_size);
}

// Matrix allocation aligned to cache line boundaries
double* allocate_matrix(size_t N) {
    size_t size = N * N * sizeof(double);
    double* matrix;
    
    // Allocate aligned memory for better cache performance
    if (posix_memalign((void**)&matrix, L1_CACHE_LINE_SIZE, size) != 0) {
        return NULL;
    }
    
    return matrix;
}

void free_matrix(double* matrix) {
    free(matrix);
}

// Initialize matrix with random values
void init_matrix_random(double* matrix, size_t N, unsigned int seed) {
    srand(seed);
    for (size_t i = 0; i < N * N; i++) {
        matrix[i] = (double)rand() / RAND_MAX;
    }
}

// Initialize identity matrix
void init_matrix_identity(double* matrix, size_t N) {
    memset(matrix, 0, N * N * sizeof(double));
    for (size_t i = 0; i < N; i++) {
        matrix[i * N + i] = 1.0;
    }
}

// Copy matrix
void copy_matrix(const double* src, double* dst, size_t N) {
    memcpy(dst, src, N * N * sizeof(double));
}

// Compare two matrices with tolerance for floating-point errors
int compare_matrices(const double* A, const double* B, size_t N, double tolerance) {
    for (size_t i = 0; i < N * N; i++) {
        if (fabs(A[i] - B[i]) > tolerance) {
            return 0;  // Matrices are different
        }
    }
    return 1;  // Matrices are equal within tolerance
}

// Print matrix (or portion of it for large matrices)
void print_matrix(const double* matrix, size_t N, size_t max_display) {
    size_t display_size = MIN(N, max_display);
    
    for (size_t i = 0; i < display_size; i++) {
        for (size_t j = 0; j < display_size; j++) {
            printf("%8.4f ", matrix[i * N + j]);
        }
        if (display_size < N) {
            printf("...");
        }
        printf("\n");
    }
    if (display_size < N) {
        printf("...\n");
    }
}

// Get current time in seconds
static double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Benchmark matrix multiplication function
double benchmark_matmul(void (*matmul_func)(const double*, const double*, double*, size_t),
                       const double* A, const double* B, double* C, size_t N, int iterations) {
    double start_time = get_time();
    
    for (int i = 0; i < iterations; i++) {
        matmul_func(A, B, C, N);
    }
    
    double end_time = get_time();
    double total_time = end_time - start_time;
    double avg_time = total_time / iterations;
    
    // Calculate GFLOPS (2*N^3 operations per multiplication)
    double gflops = (2.0 * N * N * N * iterations) / (total_time * 1e9);
    
    printf("Average time: %.6f seconds, Performance: %.2f GFLOPS\n", avg_time, gflops);
    
    return avg_time;
}