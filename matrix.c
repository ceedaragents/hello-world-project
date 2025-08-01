#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Matrix* matrix_create(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
        fprintf(stderr, "Error: Cannot create matrix with zero dimensions\n");
        return NULL;
    }
    
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    if (!m) {
        fprintf(stderr, "Error: Failed to allocate matrix structure\n");
        return NULL;
    }
    
    m->rows = rows;
    m->cols = cols;
    m->data = (double*)calloc(rows * cols, sizeof(double));
    
    if (!m->data) {
        fprintf(stderr, "Error: Failed to allocate matrix data (%zu x %zu)\n", rows, cols);
        free(m);
        return NULL;
    }
    
    return m;
}

void matrix_free(Matrix* m) {
    if (m) {
        if (m->data) {
            free(m->data);
        }
        free(m);
    }
}

double matrix_get(const Matrix* m, size_t i, size_t j) {
    if (!m || !m->data) {
        fprintf(stderr, "Error: Attempting to access NULL matrix\n");
        return 0.0;
    }
    
    if (i >= m->rows || j >= m->cols) {
        fprintf(stderr, "Error: Matrix access out of bounds (%zu,%zu) in (%zu x %zu) matrix\n",
                i, j, m->rows, m->cols);
        return 0.0;
    }
    
    return m->data[i * m->cols + j];
}

void matrix_set(Matrix* m, size_t i, size_t j, double value) {
    if (!m || !m->data) {
        fprintf(stderr, "Error: Attempting to modify NULL matrix\n");
        return;
    }
    
    if (i >= m->rows || j >= m->cols) {
        fprintf(stderr, "Error: Matrix set out of bounds (%zu,%zu) in (%zu x %zu) matrix\n",
                i, j, m->rows, m->cols);
        return;
    }
    
    m->data[i * m->cols + j] = value;
}

int matrix_is_valid(const Matrix* m) {
    return m != NULL && m->data != NULL && m->rows > 0 && m->cols > 0;
}

Matrix* matrix_copy(const Matrix* src) {
    if (!matrix_is_valid(src)) {
        return NULL;
    }
    
    Matrix* dst = matrix_create(src->rows, src->cols);
    if (!dst) {
        return NULL;
    }
    
    memcpy(dst->data, src->data, src->rows * src->cols * sizeof(double));
    return dst;
}

void matrix_fill_random(Matrix* m, double min, double max) {
    if (!matrix_is_valid(m)) {
        return;
    }
    
    double range = max - min;
    for (size_t i = 0; i < m->rows * m->cols; i++) {
        m->data[i] = min + (range * rand() / (double)RAND_MAX);
    }
}

void matrix_print(const Matrix* m, const char* name) {
    if (!matrix_is_valid(m)) {
        printf("Matrix %s: Invalid or NULL\n", name ? name : "");
        return;
    }
    
    if (name) {
        printf("Matrix %s (%zu x %zu):\n", name, m->rows, m->cols);
    } else {
        printf("Matrix (%zu x %zu):\n", m->rows, m->cols);
    }
    
    // Only print small matrices in full
    if (m->rows <= 10 && m->cols <= 10) {
        for (size_t i = 0; i < m->rows; i++) {
            for (size_t j = 0; j < m->cols; j++) {
                printf("%8.3f ", matrix_get(m, i, j));
            }
            printf("\n");
        }
    } else {
        printf("  [Matrix too large to display, showing corners]\n");
        // Show top-left corner
        for (size_t i = 0; i < 3 && i < m->rows; i++) {
            for (size_t j = 0; j < 3 && j < m->cols; j++) {
                printf("%8.3f ", matrix_get(m, i, j));
            }
            printf("... ");
            // Show top-right corner
            for (size_t j = m->cols - 3; j < m->cols && j >= 3; j++) {
                printf("%8.3f ", matrix_get(m, i, j));
            }
            printf("\n");
        }
        printf("  ...\n");
    }
    printf("\n");
}