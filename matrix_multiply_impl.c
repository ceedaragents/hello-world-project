#include "matrix_multiply.h"
#include <stdio.h>
#include <stdlib.h>

// Helper function to validate matrix compatibility for multiplication
static int validate_multiplication(const Matrix* A, const Matrix* B) {
    if (!matrix_is_valid(A) || !matrix_is_valid(B)) {
        fprintf(stderr, "Error: Invalid matrix provided for multiplication\n");
        return 0;
    }
    
    if (A->cols != B->rows) {
        fprintf(stderr, "Error: Matrix dimensions incompatible: (%zu x %zu) * (%zu x %zu)\n",
                A->rows, A->cols, B->rows, B->cols);
        return 0;
    }
    
    return 1;
}

Matrix* multiply_naive(const Matrix* A, const Matrix* B) {
    if (!validate_multiplication(A, B)) {
        return NULL;
    }
    
    Matrix* C = matrix_create(A->rows, B->cols);
    if (!C) {
        return NULL;
    }
    
    // Standard triple-nested loop multiplication
    for (size_t i = 0; i < A->rows; i++) {
        for (size_t j = 0; j < B->cols; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < A->cols; k++) {
                sum += matrix_get(A, i, k) * matrix_get(B, k, j);
            }
            matrix_set(C, i, j, sum);
        }
    }
    
    return C;
}

Matrix* multiply_blocked(const Matrix* A, const Matrix* B, size_t block_size) {
    if (!validate_multiplication(A, B)) {
        return NULL;
    }
    
    // Use default block size if not specified
    if (block_size == 0) {
        block_size = DEFAULT_BLOCK_SIZE;
    }
    
    Matrix* C = matrix_create(A->rows, B->cols);
    if (!C) {
        return NULL;
    }
    
    const size_t M = A->rows;
    const size_t N = B->cols;
    const size_t K = A->cols;
    
    // Process matrices in blocks for better cache locality
    for (size_t i0 = 0; i0 < M; i0 += block_size) {
        for (size_t j0 = 0; j0 < N; j0 += block_size) {
            for (size_t k0 = 0; k0 < K; k0 += block_size) {
                // Calculate block boundaries
                size_t i_max = (i0 + block_size < M) ? i0 + block_size : M;
                size_t j_max = (j0 + block_size < N) ? j0 + block_size : N;
                size_t k_max = (k0 + block_size < K) ? k0 + block_size : K;
                
                // Multiply current block
                for (size_t i = i0; i < i_max; i++) {
                    for (size_t j = j0; j < j_max; j++) {
                        double sum = matrix_get(C, i, j);
                        for (size_t k = k0; k < k_max; k++) {
                            sum += matrix_get(A, i, k) * matrix_get(B, k, j);
                        }
                        matrix_set(C, i, j, sum);
                    }
                }
            }
        }
    }
    
    return C;
}

// Context structure to reduce parameter passing in recursive implementation
typedef struct {
    const double* A;
    const double* B;
    double* C;
    size_t lda;  // leading dimension of A
    size_t ldb;  // leading dimension of B
    size_t ldc;  // leading dimension of C
    size_t threshold;
} RecursiveContext;

// Recursive implementation for cache-oblivious multiplication
static void multiply_recursive_impl(
    const RecursiveContext* ctx,
    size_t M, size_t N, size_t K,
    size_t A_row, size_t A_col,
    size_t B_row, size_t B_col,
    size_t C_row, size_t C_col) {
    
    // Base case: use naive multiplication for small matrices
    if (M <= ctx->threshold && N <= ctx->threshold && K <= ctx->threshold) {
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                double sum = 0.0;
                for (size_t k = 0; k < K; k++) {
                    sum += ctx->A[(A_row + i) * ctx->lda + (A_col + k)] * 
                           ctx->B[(B_row + k) * ctx->ldb + (B_col + j)];
                }
                ctx->C[(C_row + i) * ctx->ldc + (C_col + j)] += sum;
            }
        }
        return;
    }
    
    // Recursive case: divide along the largest dimension
    if (M >= N && M >= K) {
        // Divide M dimension
        size_t M1 = M / 2;
        size_t M2 = M - M1;
        
        multiply_recursive_impl(ctx, M1, N, K,
                              A_row, A_col, B_row, B_col, C_row, C_col);
        
        multiply_recursive_impl(ctx, M2, N, K,
                              A_row + M1, A_col, B_row, B_col, C_row + M1, C_col);
    }
    else if (N >= K) {
        // Divide N dimension
        size_t N1 = N / 2;
        size_t N2 = N - N1;
        
        multiply_recursive_impl(ctx, M, N1, K,
                              A_row, A_col, B_row, B_col, C_row, C_col);
        
        multiply_recursive_impl(ctx, M, N2, K,
                              A_row, A_col, B_row, B_col + N1, C_row, C_col + N1);
    }
    else {
        // Divide K dimension
        size_t K1 = K / 2;
        size_t K2 = K - K1;
        
        multiply_recursive_impl(ctx, M, N, K1,
                              A_row, A_col, B_row, B_col, C_row, C_col);
        
        multiply_recursive_impl(ctx, M, N, K2,
                              A_row, A_col + K1, B_row + K1, B_col, C_row, C_col);
    }
}

Matrix* multiply_cache_oblivious(const Matrix* A, const Matrix* B) {
    if (!validate_multiplication(A, B)) {
        return NULL;
    }
    
    Matrix* C = matrix_create(A->rows, B->cols);
    if (!C) {
        return NULL;
    }
    
    // Set up context for recursive implementation
    RecursiveContext ctx = {
        .A = A->data,
        .B = B->data,
        .C = C->data,
        .lda = A->cols,
        .ldb = B->cols,
        .ldc = C->cols,
        .threshold = CACHE_OBLIVIOUS_THRESHOLD
    };
    
    multiply_recursive_impl(&ctx, A->rows, B->cols, A->cols,
                          0, 0, 0, 0, 0, 0);
    
    return C;
}

size_t calculate_optimal_block_size(const Matrix* A, const Matrix* B) {
    // Simple heuristic: choose block size based on matrix dimensions
    // and typical cache sizes
    size_t min_dim = A->rows;
    if (B->cols < min_dim) min_dim = B->cols;
    if (A->cols < min_dim) min_dim = A->cols;
    
    // Choose block size as a fraction of the smallest dimension
    size_t block_size = min_dim / 4;
    
    // Clamp to reasonable range
    if (block_size < 16) block_size = 16;
    if (block_size > 128) block_size = 128;
    
    // Round to multiple of 8 for better alignment
    block_size = (block_size / 8) * 8;
    
    return block_size;
}

Matrix* matrix_multiply_auto(const Matrix* A, const Matrix* B) {
    if (!validate_multiplication(A, B)) {
        return NULL;
    }
    
    // Calculate total operations
    size_t total_ops = A->rows * B->cols * A->cols;
    
    // Choose algorithm based on problem size
    if (total_ops < 8000) {
        // Small matrices: naive algorithm is fine
        return multiply_naive(A, B);
    } else if (total_ops < 1000000) {
        // Medium matrices: use blocked algorithm
        size_t block_size = calculate_optimal_block_size(A, B);
        return multiply_blocked(A, B, block_size);
    } else {
        // Large matrices: use cache-oblivious algorithm
        return multiply_cache_oblivious(A, B);
    }
}