# Cache-Oblivious Matrix Multiplication

This project implements a cache-oblivious matrix multiplication algorithm in C that achieves optimal cache performance without knowing the cache parameters of the target machine.

## Overview

The cache-oblivious algorithm uses a recursive divide-and-conquer approach that automatically adapts to different cache hierarchies. Unlike cache-aware algorithms that require tuning for specific hardware, this implementation works efficiently across different architectures.

## Implementation Details

### Algorithm
- **Recursive Division**: The algorithm recursively divides matrices along their largest dimension
- **Base Case**: Uses standard multiplication when submatrices are smaller than 64×64 elements
- **Cache Efficiency**: Achieves O(n³/√M) cache complexity for cache size M

### Key Features
- Portable across different architectures
- No hardware-specific tuning required
- Automatic adaptation to multi-level cache hierarchies
- Comparable performance to tuned cache-aware implementations

## Building and Running

```bash
# Build with optimization
make

# Run the program
make run

# Build with debug symbols
make debug

# Clean build artifacts
make clean

# Run with custom matrix size
./matrix_mult 2048
```

## Performance Results

The implementation shows significant performance improvements over naive matrix multiplication:

- **256×256 matrices**: 2.37x speedup (136.8% performance improvement)
- **512×512 matrices**: 1.28x speedup (27.9% performance improvement)
- **1024×1024 matrices**: 1.13x speedup (13.1% performance improvement)

Performance is measured in GFLOPS (Giga Floating-Point Operations Per Second).

## Files

- `matrix_multiplication.h` - Header file with matrix operations interface
- `matrix_multiplication.c` - Implementation of matrix operations and cache-oblivious algorithm
- `main.c` - Test program with correctness verification and performance benchmarks
- `Makefile` - Build configuration with optimization flags

## Compilation Flags

The project uses the following optimization flags:
- `-O3`: Maximum optimization level
- `-march=native`: Generate code for the host architecture
- `-ffast-math`: Enable fast floating-point optimizations
- `-std=c11`: Use C11 standard

## Testing

The program includes:
1. **Correctness tests**: Verifies results match naive implementation
2. **Performance benchmarks**: Measures execution time and GFLOPS
3. **Multiple matrix sizes**: Tests from 4×4 to 1024×1024 matrices