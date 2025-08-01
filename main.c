#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"
#include "matrix_multiply.h"
#include "matrix_test.h"
#include "matrix_benchmark.h"

void print_usage(const char* program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  -h, --help        Show this help message\n");
    printf("  -t, --test        Run tests only\n");
    printf("  -b, --benchmark   Run benchmarks only\n");
    printf("  -a, --all         Run both tests and benchmarks (default)\n");
}

int main(int argc, char* argv[]) {
    int run_tests = 1;
    int run_benchmarks = 1;
    
    // Parse command line arguments
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                print_usage(argv[0]);
                return 0;
            } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--test") == 0) {
                run_tests = 1;
                run_benchmarks = 0;
            } else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--benchmark") == 0) {
                run_tests = 0;
                run_benchmarks = 1;
            } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
                run_tests = 1;
                run_benchmarks = 1;
            } else {
                fprintf(stderr, "Unknown option: %s\n", argv[i]);
                print_usage(argv[0]);
                return 1;
            }
        }
    }
    
    printf("Cache-Efficient Matrix Multiplication\n");
    printf("=====================================\n");
    
    int exit_code = 0;
    
    // Run tests if requested
    if (run_tests) {
        TestResults results = run_all_tests();
        if (results.failed_tests > 0) {
            exit_code = 1;
        }
    }
    
    // Run benchmarks if requested
    if (run_benchmarks && exit_code == 0) {
        run_performance_benchmarks();
    }
    
    return exit_code;
}