#include "matrix_test.h"
#include "matrix_multiply.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int matrices_equal(const Matrix* A, const Matrix* B, double tolerance) {
    if (!matrix_is_valid(A) || !matrix_is_valid(B)) {
        return 0;
    }
    
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
                    printf("  Difference at (%zu,%zu): %.10e (relative: %.10e)\n", 
                           i, j, diff, diff/max_val);
                    return 0;
                }
            } else {
                if (diff > tolerance) {
                    printf("  Difference at (%zu,%zu): %.10e\n", i, j, diff);
                    return 0;
                }
            }
        }
    }
    return 1;
}

void print_test_result(const char* test_name, int passed) {
    printf("[%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

void print_test_summary(const TestResults* results) {
    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Total tests:  %d\n", results->total_tests);
    printf("  Passed:       %d\n", results->passed_tests);
    printf("  Failed:       %d\n", results->failed_tests);
    printf("  Success rate: %.1f%%\n", 
           100.0 * results->passed_tests / results->total_tests);
    printf("========================================\n");
}

int test_basic(void) {
    printf("\nTesting basic 2x2 multiplication...\n");
    
    // Create test matrices
    Matrix* A = matrix_create(2, 2);
    if (!A) return 0;
    
    matrix_set(A, 0, 0, 1); matrix_set(A, 0, 1, 2);
    matrix_set(A, 1, 0, 3); matrix_set(A, 1, 1, 4);
    
    Matrix* B = matrix_create(2, 2);
    if (!B) {
        matrix_free(A);
        return 0;
    }
    
    matrix_set(B, 0, 0, 5); matrix_set(B, 0, 1, 6);
    matrix_set(B, 1, 0, 7); matrix_set(B, 1, 1, 8);
    
    // Expected result: [1 2] * [5 6] = [19 22]
    //                  [3 4]   [7 8]   [43 50]
    Matrix* expected = matrix_create(2, 2);
    if (!expected) {
        matrix_free(A);
        matrix_free(B);
        return 0;
    }
    
    matrix_set(expected, 0, 0, 19); matrix_set(expected, 0, 1, 22);
    matrix_set(expected, 1, 0, 43); matrix_set(expected, 1, 1, 50);
    
    // Test all algorithms
    int all_passed = 1;
    
    // Test naive algorithm
    Matrix* result_naive = multiply_naive(A, B);
    if (!result_naive || !matrices_equal(result_naive, expected, DEFAULT_TOLERANCE)) {
        print_test_result("Basic 2x2 - Naive", 0);
        all_passed = 0;
    } else {
        print_test_result("Basic 2x2 - Naive", 1);
    }
    
    // Test blocked algorithm
    Matrix* result_blocked = multiply_blocked(A, B, 0);
    if (!result_blocked || !matrices_equal(result_blocked, expected, DEFAULT_TOLERANCE)) {
        print_test_result("Basic 2x2 - Blocked", 0);
        all_passed = 0;
    } else {
        print_test_result("Basic 2x2 - Blocked", 1);
    }
    
    // Test cache-oblivious algorithm
    Matrix* result_co = multiply_cache_oblivious(A, B);
    if (!result_co || !matrices_equal(result_co, expected, DEFAULT_TOLERANCE)) {
        print_test_result("Basic 2x2 - Cache Oblivious", 0);
        all_passed = 0;
    } else {
        print_test_result("Basic 2x2 - Cache Oblivious", 1);
    }
    
    // Clean up
    matrix_free(A);
    matrix_free(B);
    matrix_free(expected);
    if (result_naive) matrix_free(result_naive);
    if (result_blocked) matrix_free(result_blocked);
    if (result_co) matrix_free(result_co);
    
    return all_passed;
}

int test_larger_matrices(void) {
    printf("\nTesting larger matrices...\n");
    
    size_t sizes[] = {10, 50, 100, 200};
    int all_passed = 1;
    
    for (int idx = 0; idx < 4; idx++) {
        size_t n = sizes[idx];
        printf("\nTesting %zux%zu matrices:\n", n, n);
        
        // Create test matrices
        Matrix* A = matrix_create(n, n);
        Matrix* B = matrix_create(n, n);
        
        if (!A || !B) {
            if (A) matrix_free(A);
            if (B) matrix_free(B);
            print_test_result("Memory allocation", 0);
            return 0;
        }
        
        // Fill with test data
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                matrix_set(A, i, j, ((double)(i + j) / n) * 0.01);
                matrix_set(B, i, j, ((double)(i - j) / n) * 0.01);
            }
        }
        
        // Test all algorithms
        Matrix* result_naive = multiply_naive(A, B);
        Matrix* result_blocked = multiply_blocked(A, B, 0);
        Matrix* result_co = multiply_cache_oblivious(A, B);
        
        if (!result_naive || !result_blocked || !result_co) {
            print_test_result("Algorithm execution", 0);
            all_passed = 0;
        } else {
            // Use larger tolerance for bigger matrices due to floating-point accumulation
            double tolerance = (n >= 100) ? LARGE_MATRIX_TOLERANCE : DEFAULT_TOLERANCE;
            
            // Check blocked vs naive
            if (matrices_equal(result_blocked, result_naive, tolerance)) {
                char test_name[64];
                snprintf(test_name, sizeof(test_name), "%zux%zu - Blocked vs Naive", n, n);
                print_test_result(test_name, 1);
            } else {
                char test_name[64];
                snprintf(test_name, sizeof(test_name), "%zux%zu - Blocked vs Naive", n, n);
                print_test_result(test_name, 0);
                all_passed = 0;
            }
            
            // Check cache-oblivious vs naive
            if (matrices_equal(result_co, result_naive, tolerance)) {
                char test_name[64];
                snprintf(test_name, sizeof(test_name), "%zux%zu - Cache-Oblivious vs Naive", n, n);
                print_test_result(test_name, 1);
            } else {
                char test_name[64];
                snprintf(test_name, sizeof(test_name), "%zux%zu - Cache-Oblivious vs Naive", n, n);
                print_test_result(test_name, 0);
                all_passed = 0;
            }
        }
        
        // Clean up
        matrix_free(A);
        matrix_free(B);
        if (result_naive) matrix_free(result_naive);
        if (result_blocked) matrix_free(result_blocked);
        if (result_co) matrix_free(result_co);
    }
    
    return all_passed;
}

TestResults run_all_tests(void) {
    TestResults results = {0, 0, 0};
    
    printf("Running matrix multiplication tests...\n");
    
    // Run basic tests
    results.total_tests++;
    if (test_basic()) {
        results.passed_tests++;
    } else {
        results.failed_tests++;
    }
    
    // Run larger matrix tests
    results.total_tests++;
    if (test_larger_matrices()) {
        results.passed_tests++;
    } else {
        results.failed_tests++;
    }
    
    // Print summary
    print_test_summary(&results);
    
    return results;
}