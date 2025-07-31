#ifndef FIBONACCI_H
#define FIBONACCI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_FIBONACCI_TERMS 93

typedef unsigned long long fibonacci_t;

void print_fibonacci_iterative(int n);
fibonacci_t fibonacci_recursive(int n);
void print_fibonacci_recursive_sequence(int n);
bool validate_input(int n);

#endif