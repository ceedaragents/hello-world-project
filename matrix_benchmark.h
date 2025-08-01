#ifndef MATRIX_BENCHMARK_H
#define MATRIX_BENCHMARK_H

#include "matrix.h"

// Benchmark configuration
#define BENCHMARK_WARMUP_RUNS 3
#define BENCHMARK_NUM_RUNS 5

/**
 * Gets current wall-clock time in seconds.
 * 
 * @return Current time in seconds
 */
double get_wall_time(void);

/**
 * Benchmarks a single matrix multiplication algorithm.
 * 
 * @param algorithm_name Name of the algorithm for display
 * @param multiply_func Function pointer to the multiplication algorithm
 * @param A First input matrix
 * @param B Second input matrix
 * @param warmup Number of warmup runs (0 to skip)
 * @param num_runs Number of timed runs
 * @return Average time per multiplication in seconds
 */
double benchmark_algorithm(
    const char* algorithm_name,
    Matrix* (*multiply_func)(const Matrix*, const Matrix*),
    const Matrix* A,
    const Matrix* B,
    int warmup,
    int num_runs
);

/**
 * Runs performance benchmarks on all multiplication algorithms.
 * Tests various matrix sizes and reports GFLOPS performance.
 */
void run_performance_benchmarks(void);

/**
 * Calculates GFLOPS (Giga Floating-Point Operations Per Second).
 * 
 * @param M Number of rows in result matrix
 * @param N Number of columns in result matrix
 * @param K Inner dimension of multiplication
 * @param time_seconds Time taken in seconds
 * @return GFLOPS performance metric
 */
double calculate_gflops(size_t M, size_t N, size_t K, double time_seconds);

#endif /* MATRIX_BENCHMARK_H */