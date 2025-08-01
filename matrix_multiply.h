#ifndef MATRIX_MULTIPLY_H
#define MATRIX_MULTIPLY_H

#include "matrix.h"

// Default configuration constants
#define DEFAULT_BLOCK_SIZE 64
#define CACHE_OBLIVIOUS_THRESHOLD 64

/**
 * Performs naive matrix multiplication C = A * B.
 * Time complexity: O(n^3)
 * 
 * @param A First input matrix (m x k)
 * @param B Second input matrix (k x n)
 * @return Result matrix C (m x n), or NULL if dimensions incompatible
 */
Matrix* multiply_naive(const Matrix* A, const Matrix* B);

/**
 * Performs blocked matrix multiplication C = A * B.
 * Improves cache locality by processing matrices in blocks.
 * 
 * @param A First input matrix (m x k)
 * @param B Second input matrix (k x n)
 * @param block_size Size of blocks to use (0 for default)
 * @return Result matrix C (m x n), or NULL if dimensions incompatible
 */
Matrix* multiply_blocked(const Matrix* A, const Matrix* B, size_t block_size);

/**
 * Performs cache-oblivious matrix multiplication C = A * B.
 * Uses recursive divide-and-conquer to automatically adapt to cache hierarchy.
 * 
 * @param A First input matrix (m x k)
 * @param B Second input matrix (k x n)
 * @return Result matrix C (m x n), or NULL if dimensions incompatible
 */
Matrix* multiply_cache_oblivious(const Matrix* A, const Matrix* B);

/**
 * Automatically selects the best multiplication algorithm based on matrix size.
 * 
 * @param A First input matrix (m x k)
 * @param B Second input matrix (k x n)
 * @return Result matrix C (m x n), or NULL if dimensions incompatible
 */
Matrix* matrix_multiply_auto(const Matrix* A, const Matrix* B);

/**
 * Calculates optimal block size for blocked multiplication based on matrix dimensions.
 * 
 * @param A First input matrix
 * @param B Second input matrix
 * @return Recommended block size
 */
size_t calculate_optimal_block_size(const Matrix* A, const Matrix* B);

#endif /* MATRIX_MULTIPLY_H */