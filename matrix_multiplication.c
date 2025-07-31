#include "matrix_multiplication.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define CACHE_OBLIVIOUS_THRESHOLD 64

Matrix* matrix_create(size_t rows, size_t cols) {
    Matrix *mat = malloc(sizeof(Matrix));
    if (!mat) return NULL;
    
    mat->rows = rows;
    mat->cols = cols;
    mat->data = calloc(rows * cols, sizeof(double));
    
    if (!mat->data) {
        free(mat);
        return NULL;
    }
    
    return mat;
}

void matrix_destroy(Matrix *mat) {
    if (mat) {
        free(mat->data);
        free(mat);
    }
}

void matrix_fill_random(Matrix *mat, double min, double max) {
    if (!mat || !mat->data) return;
    
    for (size_t i = 0; i < mat->rows * mat->cols; i++) {
        mat->data[i] = min + (max - min) * ((double)rand() / RAND_MAX);
    }
}

void matrix_print(const Matrix *mat) {
    if (!mat || !mat->data) return;
    
    for (size_t i = 0; i < mat->rows; i++) {
        for (size_t j = 0; j < mat->cols; j++) {
            printf("%8.2f ", mat->data[i * mat->cols + j]);
        }
        printf("\n");
    }
}

int matrix_equals(const Matrix *a, const Matrix *b, double tolerance) {
    if (!a || !b || a->rows != b->rows || a->cols != b->cols) {
        return 0;
    }
    
    for (size_t i = 0; i < a->rows * a->cols; i++) {
        if (fabs(a->data[i] - b->data[i]) > tolerance) {
            return 0;
        }
    }
    
    return 1;
}

void matrix_multiply_naive(const Matrix *a, const Matrix *b, Matrix *c) {
    if (!a || !b || !c || a->cols != b->rows || 
        c->rows != a->rows || c->cols != b->cols) {
        return;
    }
    
    memset(c->data, 0, c->rows * c->cols * sizeof(double));
    
    for (size_t i = 0; i < a->rows; i++) {
        for (size_t j = 0; j < b->cols; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < a->cols; k++) {
                sum += a->data[i * a->cols + k] * b->data[k * b->cols + j];
            }
            c->data[i * c->cols + j] = sum;
        }
    }
}

static void cache_oblivious_recursive(
    const double *a, const double *b, double *c,
    size_t m, size_t n, size_t k,
    size_t a_row_start, size_t a_col_start, size_t a_cols,
    size_t b_row_start, size_t b_col_start, size_t b_cols,
    size_t c_row_start, size_t c_col_start, size_t c_cols) {
    
    if (m <= CACHE_OBLIVIOUS_THRESHOLD && 
        n <= CACHE_OBLIVIOUS_THRESHOLD && 
        k <= CACHE_OBLIVIOUS_THRESHOLD) {
        
        for (size_t i = 0; i < m; i++) {
            for (size_t j = 0; j < n; j++) {
                double sum = 0.0;
                for (size_t p = 0; p < k; p++) {
                    sum += a[(a_row_start + i) * a_cols + (a_col_start + p)] *
                           b[(b_row_start + p) * b_cols + (b_col_start + j)];
                }
                c[(c_row_start + i) * c_cols + (c_col_start + j)] += sum;
            }
        }
        return;
    }
    
    if (m >= n && m >= k) {
        size_t m2 = m / 2;
        
        cache_oblivious_recursive(a, b, c, m2, n, k,
            a_row_start, a_col_start, a_cols,
            b_row_start, b_col_start, b_cols,
            c_row_start, c_col_start, c_cols);
        
        cache_oblivious_recursive(a, b, c, m - m2, n, k,
            a_row_start + m2, a_col_start, a_cols,
            b_row_start, b_col_start, b_cols,
            c_row_start + m2, c_col_start, c_cols);
    }
    else if (n >= m && n >= k) {
        size_t n2 = n / 2;
        
        cache_oblivious_recursive(a, b, c, m, n2, k,
            a_row_start, a_col_start, a_cols,
            b_row_start, b_col_start, b_cols,
            c_row_start, c_col_start, c_cols);
        
        cache_oblivious_recursive(a, b, c, m, n - n2, k,
            a_row_start, a_col_start, a_cols,
            b_row_start, b_col_start + n2, b_cols,
            c_row_start, c_col_start + n2, c_cols);
    }
    else {
        size_t k2 = k / 2;
        
        cache_oblivious_recursive(a, b, c, m, n, k2,
            a_row_start, a_col_start, a_cols,
            b_row_start, b_col_start, b_cols,
            c_row_start, c_col_start, c_cols);
        
        cache_oblivious_recursive(a, b, c, m, n, k - k2,
            a_row_start, a_col_start + k2, a_cols,
            b_row_start + k2, b_col_start, b_cols,
            c_row_start, c_col_start, c_cols);
    }
}

void matrix_multiply_cache_oblivious(const Matrix *a, const Matrix *b, Matrix *c) {
    if (!a || !b || !c || a->cols != b->rows || 
        c->rows != a->rows || c->cols != b->cols) {
        return;
    }
    
    memset(c->data, 0, c->rows * c->cols * sizeof(double));
    
    cache_oblivious_recursive(
        a->data, b->data, c->data,
        a->rows, b->cols, a->cols,
        0, 0, a->cols,
        0, 0, b->cols,
        0, 0, c->cols
    );
}

double get_time_in_seconds(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

double calculate_gflops(size_t n, double time_seconds) {
    double operations = 2.0 * n * n * n;
    return (operations / 1e9) / time_seconds;
}