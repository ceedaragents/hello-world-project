# Cache-Efficient Matrix Multiplication in Rust

This project implements a high-performance, cache-efficient matrix multiplication algorithm in Rust, demonstrating various optimization techniques for improved performance over naive implementations.

## Features

- **Multiple Multiplication Algorithms**:
  - Naive implementation (baseline)
  - Blocked/tiled multiplication
  - Fully optimized implementation with multiple techniques

- **Cache Optimization Techniques**:
  - Block/tile decomposition for better cache locality
  - Matrix transposition for sequential memory access
  - Loop unrolling for reduced overhead
  - Adaptive block size selection based on cache hierarchy

- **Performance**: Up to 1.56x speedup over naive implementation for 256x256 matrices

## Usage

```rust
use matrix_multiply::Matrix;

// Create matrices
let a = Matrix::from_vec(vec![
    vec![1.0, 2.0],
    vec![3.0, 4.0],
]);

let b = Matrix::from_vec(vec![
    vec![5.0, 6.0],
    vec![7.0, 8.0],
]);

// Multiply using the optimized algorithm
let result = a.multiply(&b).unwrap();

// Also available: multiply_naive() and multiply_blocked()
```

## Building and Running

```bash
# Run tests
cargo test

# Run the demonstration
cargo run

# Run benchmarks
cargo bench
```

## Implementation Details

### Storage Layout
- Uses row-major ordering with flat `Vec<f64>` storage
- Ensures contiguous memory access for better cache performance

### Optimization Strategies

1. **Blocking/Tiling**: Divides matrices into smaller blocks that fit in L1 cache
2. **Matrix Transposition**: Pre-transposes the second matrix for sequential access patterns
3. **Loop Unrolling**: Processes multiple elements per iteration to reduce loop overhead
4. **Adaptive Block Size**: Calculates optimal block size based on matrix dimensions and cache size

### Algorithm Selection
- Small matrices (< 10,000 elements): Uses naive algorithm to avoid overhead
- Large matrices: Uses fully optimized implementation with all techniques

## Performance

On a typical system with 64KB L1 cache:
- 256x256 matrices: ~1.56x speedup
- Cache warming effect visible in repeated runs
- Performance scales well with larger matrices

## License

This project is part of the hello-world-project repository.