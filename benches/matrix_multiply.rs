use matrix_multiply::Matrix;
use std::time::Instant;

fn benchmark_multiplication(name: &str, size: usize, f: impl Fn(&Matrix, &Matrix) -> Matrix) {
    // Create test matrices
    let mut a = Matrix::new(size, size);
    let mut b = Matrix::new(size, size);

    // Fill with some data
    for i in 0..size {
        for j in 0..size {
            a.set(i, j, (i + j) as f64);
            b.set(i, j, (i * j) as f64);
        }
    }

    // Warm up
    let _ = f(&a, &b);

    // Benchmark
    let start = Instant::now();
    let _ = f(&a, &b);
    let duration = start.elapsed();

    println!("{} {}x{}: {:?}", name, size, size, duration);
}

fn main() {
    println!("Matrix Multiplication Benchmarks");
    println!("================================");

    let sizes = vec![64, 128, 256, 512, 1024];

    for &size in &sizes {
        println!("\nMatrix size: {}x{}", size, size);

        benchmark_multiplication("Naive", size, |a, b| a.multiply_naive(b).unwrap());

        benchmark_multiplication("Blocked", size, |a, b| a.multiply_blocked(b).unwrap());

        benchmark_multiplication("Optimized", size, |a, b| a.multiply(b).unwrap());
    }

    // Test cache efficiency with different access patterns
    println!("\n\nCache Efficiency Test (512x512 matrices)");
    println!("=========================================");

    let size = 512;
    let mut a = Matrix::new(size, size);
    let mut b = Matrix::new(size, size);

    for i in 0..size {
        for j in 0..size {
            a.set(i, j, (i + j) as f64);
            b.set(i, j, (i * j) as f64);
        }
    }

    // Measure multiple runs to see cache effects
    println!("\nMultiple runs (optimized version):");
    for i in 1..=5 {
        let start = Instant::now();
        let _ = a.multiply(&b).unwrap();
        let duration = start.elapsed();
        println!("Run {}: {:?}", i, duration);
    }
}
