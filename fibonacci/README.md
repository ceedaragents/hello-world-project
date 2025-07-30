# Fibonacci Sequence Generator

A Rust implementation of a Fibonacci sequence generator.

## Features

- Generates Fibonacci numbers efficiently using iterative approach
- Command-line interface to specify how many numbers to generate
- Comprehensive unit tests

## Usage

```bash
# Generate first 10 Fibonacci numbers (default)
cargo run

# Generate first N Fibonacci numbers
cargo run -- 20
```

## Running Tests

```bash
cargo test
```

## Implementation Details

The implementation uses an iterative approach for calculating Fibonacci numbers, which is more memory efficient than recursive solutions and avoids stack overflow for large values.