#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct {
    size_t rows;
    size_t cols;
    double* data;
} Matrix;

Matrix* matrix_create(size_t rows, size_t cols) {
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    m->rows = rows;
    m->cols = cols;
    m->data = (double*)calloc(rows * cols, sizeof(double));
    return m;
}

void matrix_free(Matrix* m) {
    if (m) {
        free(m->data);
        free(m);
    }
}

double matrix_get(const Matrix* m, size_t i, size_t j) {
    return m->data[i * m->cols + j];
}

void matrix_set(Matrix* m, size_t i, size_t j, double value) {
    m->data[i * m->cols + j] = value;
}

Matrix* multiply_naive(const Matrix* A, const Matrix* B) {
    if (A->cols != B->rows) {
        fprintf(stderr, "Matrix dimensions incompatible\n");
        return NULL;
    }
    
    Matrix* C = matrix_create(A->rows, B->cols);
    
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
    if (A->cols != B->rows) {
        fprintf(stderr, "Matrix dimensions incompatible\n");
        return NULL;
    }
    
    Matrix* C = matrix_create(A->rows, B->cols);
    
    const size_t M = A->rows;
    const size_t N = B->cols;
    const size_t K = A->cols;
    
    for (size_t i0 = 0; i0 < M; i0 += block_size) {
        for (size_t j0 = 0; j0 < N; j0 += block_size) {
            for (size_t k0 = 0; k0 < K; k0 += block_size) {
                size_t i_max = (i0 + block_size < M) ? i0 + block_size : M;
                size_t j_max = (j0 + block_size < N) ? j0 + block_size : N;
                size_t k_max = (k0 + block_size < K) ? k0 + block_size : K;
                
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

static void multiply_recursive_impl(
    const double* A, const double* B, double* C,
    size_t M, size_t N, size_t K,
    size_t lda, size_t ldb, size_t ldc,
    size_t A_row, size_t A_col,
    size_t B_row, size_t B_col,
    size_t C_row, size_t C_col,
    size_t threshold) {
    
    if (M <= threshold && N <= threshold && K <= threshold) {
        for (size_t i = 0; i < M; i++) {
            for (size_t j = 0; j < N; j++) {
                double sum = 0.0;
                for (size_t k = 0; k < K; k++) {
                    sum += A[(A_row + i) * lda + (A_col + k)] * 
                           B[(B_row + k) * ldb + (B_col + j)];
                }
                C[(C_row + i) * ldc + (C_col + j)] += sum;
            }
        }
        return;
    }
    
    if (M >= N && M >= K) {
        size_t M1 = M / 2;
        size_t M2 = M - M1;
        
        multiply_recursive_impl(A, B, C, M1, N, K, lda, ldb, ldc,
                              A_row, A_col, B_row, B_col, C_row, C_col, threshold);
        
        multiply_recursive_impl(A, B, C, M2, N, K, lda, ldb, ldc,
                              A_row + M1, A_col, B_row, B_col, C_row + M1, C_col, threshold);
    }
    else if (N >= K) {
        size_t N1 = N / 2;
        size_t N2 = N - N1;
        
        multiply_recursive_impl(A, B, C, M, N1, K, lda, ldb, ldc,
                              A_row, A_col, B_row, B_col, C_row, C_col, threshold);
        
        multiply_recursive_impl(A, B, C, M, N2, K, lda, ldb, ldc,
                              A_row, A_col, B_row, B_col + N1, C_row, C_col + N1, threshold);
    }
    else {
        size_t K1 = K / 2;
        size_t K2 = K - K1;
        
        multiply_recursive_impl(A, B, C, M, N, K1, lda, ldb, ldc,
                              A_row, A_col, B_row, B_col, C_row, C_col, threshold);
        
        multiply_recursive_impl(A, B, C, M, N, K2, lda, ldb, ldc,
                              A_row, A_col + K1, B_row + K1, B_col, C_row, C_col, threshold);
    }
}

Matrix* multiply_cache_oblivious(const Matrix* A, const Matrix* B) {
    if (A->cols != B->rows) {
        fprintf(stderr, "Matrix dimensions incompatible\n");
        return NULL;
    }
    
    Matrix* C = matrix_create(A->rows, B->cols);
    
    multiply_recursive_impl(
        A->data, B->data, C->data,
        A->rows, B->cols, A->cols,
        A->cols, B->cols, C->cols,
        0, 0, 0, 0, 0, 0,
        64
    );
    
    return C;
}

int matrices_equal(const Matrix* A, const Matrix* B, double tolerance) {
    if (A->rows != B->rows || A->cols != B->cols) {
        return 0;
    }
    
    for (size_t i = 0; i < A->rows; i++) {
        for (size_t j = 0; j < A->cols; j++) {
            double a_val = matrix_get(A, i, j);
            double b_val = matrix_get(B, i, j);
            double diff = fabs(a_val - b_val);
            double max_val = fmax(fabs(a_val), fabs(b_val));
            
            // Use relative error for large values
            if (max_val > 1.0) {
                if (diff / max_val > tolerance) {
                    return 0;
                }
            } else {
                if (diff > tolerance) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

double get_time() {
    return (double)clock() / CLOCKS_PER_SEC;
}

void test_basic() {
    printf("Testing basic 2x2 multiplication...\n");
    
    Matrix* A = matrix_create(2, 2);
    matrix_set(A, 0, 0, 1); matrix_set(A, 0, 1, 2);
    matrix_set(A, 1, 0, 3); matrix_set(A, 1, 1, 4);
    
    Matrix* B = matrix_create(2, 2);
    matrix_set(B, 0, 0, 5); matrix_set(B, 0, 1, 6);
    matrix_set(B, 1, 0, 7); matrix_set(B, 1, 1, 8);
    
    Matrix* expected = matrix_create(2, 2);
    matrix_set(expected, 0, 0, 19); matrix_set(expected, 0, 1, 22);
    matrix_set(expected, 1, 0, 43); matrix_set(expected, 1, 1, 50);
    
    Matrix* result_naive = multiply_naive(A, B);
    Matrix* result_blocked = multiply_blocked(A, B, 64);
    Matrix* result_co = multiply_cache_oblivious(A, B);
    
    if (matrices_equal(result_naive, expected, 1e-10) &&
        matrices_equal(result_blocked, expected, 1e-10) &&
        matrices_equal(result_co, expected, 1e-10)) {
        printf("✓ Basic test passed\n");
    } else {
        printf("✗ Basic test failed\n");
        exit(1);
    }
    
    matrix_free(A);
    matrix_free(B);
    matrix_free(expected);
    matrix_free(result_naive);
    matrix_free(result_blocked);
    matrix_free(result_co);
}

void test_larger_matrices() {
    printf("\nTesting larger matrices...\n");
    
    size_t sizes[] = {10, 50, 100, 200};
    for (int idx = 0; idx < 4; idx++) {
        size_t n = sizes[idx];
        
        Matrix* A = matrix_create(n, n);
        Matrix* B = matrix_create(n, n);
        
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                matrix_set(A, i, j, ((double)(i + j) / n) * 0.01);
                matrix_set(B, i, j, ((double)(i - j) / n) * 0.01);
            }
        }
        
        Matrix* result_naive = multiply_naive(A, B);
        Matrix* result_blocked = multiply_blocked(A, B, 64);
        Matrix* result_co = multiply_cache_oblivious(A, B);
        
        double tolerance = (n >= 100) ? 1e-6 : 1e-8;
        if (matrices_equal(result_blocked, result_naive, tolerance) &&
            matrices_equal(result_co, result_naive, tolerance)) {
            printf("✓ %zux%zu test passed\n", n, n);
        } else {
            printf("✗ %zux%zu test failed\n", n, n);
            
            // Debug: find first mismatch
            for (size_t i = 0; i < n && i < 5; i++) {
                for (size_t j = 0; j < n && j < 5; j++) {
                    double naive_val = matrix_get(result_naive, i, j);
                    double blocked_val = matrix_get(result_blocked, i, j);
                    double co_val = matrix_get(result_co, i, j);
                    if (fabs(naive_val - blocked_val) > tolerance || fabs(naive_val - co_val) > tolerance) {
                        printf("  Mismatch at (%zu,%zu): naive=%.10f, blocked=%.10f, co=%.10f\n", 
                               i, j, naive_val, blocked_val, co_val);
                    }
                }
            }
            exit(1);
        }
        
        matrix_free(A);
        matrix_free(B);
        matrix_free(result_naive);
        matrix_free(result_blocked);
        matrix_free(result_co);
    }
}

void benchmark_performance() {
    printf("\nPerformance Benchmark:\n");
    printf("%-20s %-10s %-15s %-15s\n", "Algorithm", "Size", "Time (s)", "GFLOPS");
    printf("--------------------------------------------------------\n");
    
    size_t sizes[] = {128, 256, 512};
    
    for (int idx = 0; idx < 3; idx++) {
        size_t n = sizes[idx];
        
        Matrix* A = matrix_create(n, n);
        Matrix* B = matrix_create(n, n);
        
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                matrix_set(A, i, j, (double)rand() / RAND_MAX);
                matrix_set(B, i, j, (double)rand() / RAND_MAX);
            }
        }
        
        double start, end;
        Matrix* C;
        double ops = 2.0 * n * n * n / 1e9;
        
        if (n <= 256) {
            start = get_time();
            C = multiply_naive(A, B);
            end = get_time();
            printf("%-20s %-10zu %-15.6f %-15.2f\n", 
                   "Naive", n, end - start, ops / (end - start));
            matrix_free(C);
        }
        
        start = get_time();
        C = multiply_blocked(A, B, 64);
        end = get_time();
        printf("%-20s %-10zu %-15.6f %-15.2f\n", 
               "Blocked (64)", n, end - start, ops / (end - start));
        matrix_free(C);
        
        start = get_time();
        C = multiply_cache_oblivious(A, B);
        end = get_time();
        printf("%-20s %-10zu %-15.6f %-15.2f\n", 
               "Cache Oblivious", n, end - start, ops / (end - start));
        matrix_free(C);
        
        matrix_free(A);
        matrix_free(B);
    }
}

int main() {
    printf("Matrix Multiplication Implementation\n");
    printf("====================================\n\n");
    
    test_basic();
    test_larger_matrices();
    benchmark_performance();
    
    printf("\nAll tests passed! ✓\n");
    
    return 0;
}