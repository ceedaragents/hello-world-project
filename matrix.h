#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

/**
 * Matrix structure for storing 2D data in row-major format
 */
typedef struct {
    size_t rows;
    size_t cols;
    double* data;
} Matrix;

/**
 * Creates a new matrix with the specified dimensions.
 * All elements are initialized to zero.
 * 
 * @param rows Number of rows
 * @param cols Number of columns
 * @return Pointer to the created matrix, or NULL on failure
 */
Matrix* matrix_create(size_t rows, size_t cols);

/**
 * Frees the memory allocated for a matrix.
 * 
 * @param m Matrix to free
 */
void matrix_free(Matrix* m);

/**
 * Gets the value at position (i, j) in the matrix.
 * 
 * @param m Matrix to read from
 * @param i Row index (0-based)
 * @param j Column index (0-based)
 * @return Value at position (i, j), or 0.0 if indices are invalid
 */
double matrix_get(const Matrix* m, size_t i, size_t j);

/**
 * Sets the value at position (i, j) in the matrix.
 * 
 * @param m Matrix to modify
 * @param i Row index (0-based)
 * @param j Column index (0-based)
 * @param value Value to set
 */
void matrix_set(Matrix* m, size_t i, size_t j, double value);

/**
 * Validates that a matrix is properly initialized.
 * 
 * @param m Matrix to validate
 * @return 1 if valid, 0 otherwise
 */
int matrix_is_valid(const Matrix* m);

/**
 * Creates a copy of a matrix.
 * 
 * @param src Source matrix to copy
 * @return Pointer to the new matrix copy, or NULL on failure
 */
Matrix* matrix_copy(const Matrix* src);

/**
 * Fills a matrix with random values in the specified range.
 * 
 * @param m Matrix to fill
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 */
void matrix_fill_random(Matrix* m, double min, double max);

/**
 * Prints a matrix to stdout for debugging.
 * 
 * @param m Matrix to print
 * @param name Optional name/label for the matrix
 */
void matrix_print(const Matrix* m, const char* name);

#endif /* MATRIX_H */