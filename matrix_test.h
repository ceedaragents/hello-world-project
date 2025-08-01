#ifndef MATRIX_TEST_H
#define MATRIX_TEST_H

#include "matrix.h"

// Test tolerance values
#define DEFAULT_TOLERANCE 1e-10
#define LARGE_MATRIX_TOLERANCE 1e-6

// Test result structure
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
} TestResults;

/**
 * Compares two matrices for equality within a tolerance.
 * 
 * @param A First matrix
 * @param B Second matrix
 * @param tolerance Maximum allowed difference between elements
 * @return 1 if matrices are equal within tolerance, 0 otherwise
 */
int matrices_equal(const Matrix* A, const Matrix* B, double tolerance);

/**
 * Runs all matrix multiplication tests.
 * 
 * @return Test results summary
 */
TestResults run_all_tests(void);

/**
 * Tests basic 2x2 matrix multiplication.
 * 
 * @return 1 if test passes, 0 otherwise
 */
int test_basic(void);

/**
 * Tests larger matrix multiplications (10x10 to 200x200).
 * 
 * @return 1 if test passes, 0 otherwise
 */
int test_larger_matrices(void);

/**
 * Prints test result in a consistent format.
 * 
 * @param test_name Name of the test
 * @param passed Whether the test passed (1) or failed (0)
 */
void print_test_result(const char* test_name, int passed);

/**
 * Prints a summary of test results.
 * 
 * @param results Test results to summarize
 */
void print_test_summary(const TestResults* results);

#endif /* MATRIX_TEST_H */