use matrix_multiply::Matrix;
use std::time::Instant;

fn main() {
    println!("Cache-Efficient Matrix Multiplication Demonstration");
    println!("==================================================\n");

    // Example 1: Small matrices
    println!("Example 1: Small 3x3 matrices");
    let a = Matrix::from_vec(vec![
        vec![1.0, 2.0, 3.0],
        vec![4.0, 5.0, 6.0],
        vec![7.0, 8.0, 9.0],
    ]);

    let b = Matrix::from_vec(vec![
        vec![9.0, 8.0, 7.0],
        vec![6.0, 5.0, 4.0],
        vec![3.0, 2.0, 1.0],
    ]);

    let result = a.multiply(&b).unwrap();
    println!("Matrix A * Matrix B:");
    print_matrix(&result);

    // Example 2: Performance comparison
    println!("\nExample 2: Performance Comparison (256x256 matrices)");
    let size = 256;
    let mut large_a = Matrix::new(size, size);
    let mut large_b = Matrix::new(size, size);

    // Fill with test data
    for i in 0..size {
        for j in 0..size {
            large_a.set(i, j, ((i + j) % 10) as f64);
            large_b.set(i, j, ((i * j) % 10) as f64);
        }
    }

    // Benchmark naive multiplication
    let start = Instant::now();
    let _ = large_a.multiply_naive(&large_b).unwrap();
    let naive_time = start.elapsed();
    println!("Naive multiplication: {:?}", naive_time);

    // Benchmark blocked multiplication
    let start = Instant::now();
    let _ = large_a.multiply_blocked(&large_b).unwrap();
    let blocked_time = start.elapsed();
    println!("Blocked multiplication: {:?}", blocked_time);

    // Benchmark optimized multiplication
    let start = Instant::now();
    let _ = large_a.multiply(&large_b).unwrap();
    let optimized_time = start.elapsed();
    println!("Optimized multiplication: {:?}", optimized_time);

    // Calculate speedup
    let speedup_blocked = naive_time.as_secs_f64() / blocked_time.as_secs_f64();
    let speedup_optimized = naive_time.as_secs_f64() / optimized_time.as_secs_f64();

    println!("\nSpeedup over naive implementation:");
    println!("Blocked: {:.2}x", speedup_blocked);
    println!("Optimized: {:.2}x", speedup_optimized);

    // Example 3: Cache optimization demonstration
    println!("\nExample 3: Cache Optimization Benefits");
    println!("Running the same multiplication 3 times to show cache warming:");

    for i in 1..=3 {
        let start = Instant::now();
        let _ = large_a.multiply(&large_b).unwrap();
        let duration = start.elapsed();
        println!("Run {}: {:?}", i, duration);
    }

    println!("\nCache-efficient implementation features:");
    println!("- Blocked/tiled multiplication for better cache locality");
    println!("- Transposed second matrix for sequential memory access");
    println!("- Loop unrolling for reduced overhead");
    println!("- Adaptive block size based on matrix dimensions");
}

fn print_matrix(matrix: &Matrix) {
    let (rows, cols) = matrix.dimensions();
    for i in 0..rows {
        print!("[");
        for j in 0..cols {
            print!("{:6.1}", matrix.get(i, j));
            if j < cols - 1 {
                print!(", ");
            }
        }
        println!("]");
    }
}
