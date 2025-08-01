# Cache-Efficient Matrix Multiplication

This project implements several cache-efficient matrix multiplication algorithms in C with a clean, modular architecture.

## Algorithms Implemented

1. **Naive Algorithm** - Standard triple-nested loop implementation
2. **Blocked Algorithm** - Divides matrices into blocks to improve cache locality
3. **Cache-Oblivious Algorithm** - Recursive divide-and-conquer approach that adapts to any cache hierarchy

## Key Features

- **Modular Architecture**: Clean separation of matrix operations, algorithms, testing, and benchmarking
- **Cache Efficiency**: Blocked and cache-oblivious algorithms minimize cache misses
- **Flexible Block Sizes**: Blocked algorithm supports configurable block sizes
- **Comprehensive Testing**: Unit tests verify correctness with various matrix sizes
- **Performance Benchmarks**: Measure GFLOPS performance across different matrix sizes
- **Relative Error Testing**: Uses relative error for numerical precision verification
- **Enhanced Error Handling**: Robust validation and error reporting throughout
- **Command-Line Interface**: Flexible options for running tests and benchmarks

## Project Structure

```
├── main.c                  # Program entry point and CLI
├── matrix.h/c              # Core matrix operations
├── matrix_multiply.h       # Multiplication algorithm interfaces
├── matrix_multiply_impl.c  # Multiplication algorithm implementations
├── matrix_test.h/c         # Testing framework
├── matrix_benchmark.h/c    # Benchmarking utilities
├── Makefile                # Build configuration
├── README.md               # This file
└── CHANGELOG.md            # Version history
```

## Building

```bash
make           # Build the executable
make clean     # Clean build artifacts
make debug     # Build with debug symbols
make help      # Show all available targets
```

## Running

```bash
make run              # Run both tests and benchmarks
make test             # Run tests only
make benchmark        # Run benchmarks only
./matrix_multiply -h  # Show command-line options
```

### Command-Line Options

- `-h, --help` - Show usage information
- `-t, --test` - Run tests only
- `-b, --benchmark` - Run benchmarks only
- `-a, --all` - Run both tests and benchmarks (default)

## Implementation Details

### Cache-Oblivious Algorithm
The cache-oblivious algorithm recursively divides the largest dimension until reaching a base case (64x64 by default). This approach automatically adapts to the cache hierarchy without needing to know cache parameters.

### Blocked Algorithm
Processes matrices in square blocks, keeping working sets in cache. Default block size is 64, which works well for modern CPUs.

### Matrix Structure
Uses row-major storage with dynamic allocation for flexibility.

## Performance Results

Example performance on modern hardware:
- Naive: ~2.5 GFLOPS for small matrices
- Blocked (64): ~4.5 GFLOPS for 256x256 matrices
- Cache-Oblivious: ~4.7 GFLOPS for 256x256 matrices

## Testing

The implementation includes:
- Basic 2x2 matrix multiplication test
- Identity matrix multiplication test
- Larger matrices (10x10 to 200x200)
- Relative error checking for numerical stability