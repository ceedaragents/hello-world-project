#ifndef MATRIX_MULTIPLICATION_H
#define MATRIX_MULTIPLICATION_H

#include <stddef.h>

typedef struct {
    double *data;
    size_t rows;
    size_t cols;
} Matrix;

Matrix* matrix_create(size_t rows, size_t cols);
void matrix_destroy(Matrix *mat);
void matrix_fill_random(Matrix *mat, double min, double max);
void matrix_print(const Matrix *mat);
int matrix_equals(const Matrix *a, const Matrix *b, double tolerance);

void matrix_multiply_naive(const Matrix *a, const Matrix *b, Matrix *c);
void matrix_multiply_cache_oblivious(const Matrix *a, const Matrix *b, Matrix *c);

double get_time_in_seconds(void);
double calculate_gflops(size_t n, double time_seconds);

#endif