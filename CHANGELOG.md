# Changelog

## [0.1.0] - 2025-01-31

### Added
- Implemented cache-efficient matrix multiplication in Rust
- Three multiplication algorithms:
  - `multiply_naive()`: Baseline implementation with standard triple-loop algorithm
  - `multiply_blocked()`: Cache-efficient blocked/tiled multiplication with fixed block size
  - `multiply()`: Optimized implementation with adaptive block size, matrix transposition, and loop unrolling
- Matrix struct with row-major storage layout for optimal cache performance
- Comprehensive test suite covering:
  - Basic matrix operations (creation, get/set)
  - Matrix multiplication correctness
  - Edge cases (dimension mismatches, rectangular matrices)
  - Large matrix multiplication verification
- Performance benchmarks for comparing different implementations
- Demonstration program showing performance improvements (up to 1.56x speedup)

### Performance Optimizations
- **Blocked/Tiled Multiplication**: Divides matrices into cache-friendly blocks
- **Matrix Transposition**: Pre-transposes second matrix for sequential memory access
- **Loop Unrolling**: Processes 4 elements at a time in inner loop
- **Adaptive Block Size**: Calculates optimal block size based on L1 cache size
- **Small Matrix Fast Path**: Uses naive algorithm for small matrices where overhead isn't worth it

### Technical Details
- Uses flat `Vec<f64>` storage for contiguous memory layout
- Row-major ordering for efficient row-wise access
- Block size optimization targets typical 64KB L1 cache
- Supports arbitrary matrix dimensions with proper boundary handling