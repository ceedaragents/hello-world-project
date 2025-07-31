#ifndef MATRIX_MULTIPLY_H
#define MATRIX_MULTIPLY_H

#include <stddef.h>

// Matrix multiplication functions
void naive_matmul(const double* A, const double* B, double* C, size_t N);
void blocked_matmul(const double* A, const double* B, double* C, size_t N, size_t block_size);
void cache_efficient_matmul(const double* A, const double* B, double* C, size_t N);

// Utility functions
double* allocate_matrix(size_t N);
void free_matrix(double* matrix);
void init_matrix_random(double* matrix, size_t N, unsigned int seed);
void init_matrix_identity(double* matrix, size_t N);
void copy_matrix(const double* src, double* dst, size_t N);
int compare_matrices(const double* A, const double* B, size_t N, double tolerance);
void print_matrix(const double* matrix, size_t N, size_t max_display);

// Benchmarking functions
double benchmark_matmul(void (*matmul_func)(const double*, const double*, double*, size_t),
                       const double* A, const double* B, double* C, size_t N, int iterations);

#endif // MATRIX_MULTIPLY_H