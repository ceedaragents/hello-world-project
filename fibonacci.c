#include "fibonacci.h"

bool validate_input(int n) {
    if (n < 0) {
        printf("Error: Number of terms cannot be negative.\n");
        return false;
    }
    if (n > MAX_FIBONACCI_TERMS) {
        printf("Error: Maximum number of terms is %d to prevent overflow.\n", MAX_FIBONACCI_TERMS);
        return false;
    }
    return true;
}

void print_fibonacci_iterative(int n) {
    if (!validate_input(n)) {
        return;
    }
    
    if (n == 0) {
        printf("Fibonacci sequence with 0 terms: (empty)\n");
        return;
    }
    
    printf("Fibonacci sequence (iterative) with %d terms:\n", n);
    
    fibonacci_t first = 0, second = 1, next;
    
    for (int i = 0; i < n; i++) {
        if (i == 0) {
            printf("%llu", first);
        } else if (i == 1) {
            printf(", %llu", second);
        } else {
            next = first + second;
            printf(", %llu", next);
            first = second;
            second = next;
        }
    }
    printf("\n");
}

fibonacci_t fibonacci_recursive(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2);
}

void print_fibonacci_recursive_sequence(int n) {
    if (!validate_input(n)) {
        return;
    }
    
    if (n == 0) {
        printf("Fibonacci sequence with 0 terms: (empty)\n");
        return;
    }
    
    printf("Fibonacci sequence (recursive) with %d terms:\n", n);
    
    for (int i = 0; i < n; i++) {
        if (i > 0) {
            printf(", ");
        }
        printf("%llu", fibonacci_recursive(i));
    }
    printf("\n");
}

int main() {
    int choice, n;
    
    printf("=== Fibonacci Sequence Generator ===\n\n");
    
    while (1) {
        printf("\nMenu:\n");
        printf("1. Generate Fibonacci sequence (iterative method)\n");
        printf("2. Generate Fibonacci sequence (recursive method)\n");
        printf("3. Compare both methods\n");
        printf("4. Exit\n");
        printf("\nEnter your choice (1-4): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        
        if (choice == 4) {
            printf("Thank you for using the Fibonacci Sequence Generator!\n");
            break;
        }
        
        if (choice < 1 || choice > 4) {
            printf("Invalid choice. Please select 1-4.\n");
            continue;
        }
        
        printf("Enter the number of terms to generate: ");
        if (scanf("%d", &n) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        
        switch (choice) {
            case 1:
                print_fibonacci_iterative(n);
                break;
            case 2:
                if (n > 40) {
                    printf("Warning: Recursive method is slow for large values. Consider using iterative method.\n");
                    printf("Continue anyway? (y/n): ");
                    char confirm;
                    scanf(" %c", &confirm);
                    if (confirm != 'y' && confirm != 'Y') {
                        break;
                    }
                }
                print_fibonacci_recursive_sequence(n);
                break;
            case 3:
                printf("\n--- Iterative Method ---\n");
                print_fibonacci_iterative(n);
                
                if (n <= 40) {
                    printf("\n--- Recursive Method ---\n");
                    print_fibonacci_recursive_sequence(n);
                } else {
                    printf("\n--- Recursive Method ---\n");
                    printf("Skipped: Too many terms for recursive method (max 40 for performance).\n");
                }
                break;
        }
    }
    
    return 0;
}