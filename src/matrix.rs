/// A matrix structure optimized for cache-efficient operations
#[derive(Debug, Clone, PartialEq)]
pub struct Matrix {
    data: Vec<f64>,
    rows: usize,
    cols: usize,
}

impl Matrix {
    /// Create a new matrix with given dimensions
    pub fn new(rows: usize, cols: usize) -> Self {
        Matrix {
            data: vec![0.0; rows * cols],
            rows,
            cols,
        }
    }

    /// Create a matrix from a 2D vector
    pub fn from_vec(data: Vec<Vec<f64>>) -> Self {
        let rows = data.len();
        let cols = if rows > 0 { data[0].len() } else { 0 };
        let flat_data: Vec<f64> = data.into_iter().flatten().collect();

        Matrix {
            data: flat_data,
            rows,
            cols,
        }
    }

    /// Get element at (row, col)
    pub fn get(&self, row: usize, col: usize) -> f64 {
        self.data[row * self.cols + col]
    }

    /// Set element at (row, col)
    pub fn set(&mut self, row: usize, col: usize, value: f64) {
        self.data[row * self.cols + col] = value;
    }

    /// Get matrix dimensions
    pub fn dimensions(&self) -> (usize, usize) {
        (self.rows, self.cols)
    }

    /// Naive matrix multiplication (baseline implementation)
    pub fn multiply_naive(&self, other: &Matrix) -> Result<Matrix, String> {
        if self.cols != other.rows {
            return Err(format!(
                "Cannot multiply {}x{} matrix with {}x{} matrix",
                self.rows, self.cols, other.rows, other.cols
            ));
        }

        let mut result = Matrix::new(self.rows, other.cols);

        for i in 0..self.rows {
            for j in 0..other.cols {
                let mut sum = 0.0;
                for k in 0..self.cols {
                    sum += self.get(i, k) * other.get(k, j);
                }
                result.set(i, j, sum);
            }
        }

        Ok(result)
    }

    /// Cache-efficient blocked matrix multiplication
    /// Uses tiling/blocking to improve cache locality
    pub fn multiply_blocked(&self, other: &Matrix) -> Result<Matrix, String> {
        if self.cols != other.rows {
            return Err(format!(
                "Cannot multiply {}x{} matrix with {}x{} matrix",
                self.rows, self.cols, other.rows, other.cols
            ));
        }

        let mut result = Matrix::new(self.rows, other.cols);

        // Block size optimized for typical L1 cache (64KB)
        // For f64 (8 bytes), sqrt(64KB / 8 / 3) ≈ 52
        // We use 64 for power-of-2 optimization
        const BLOCK_SIZE: usize = 64;

        // Process in blocks
        for i0 in (0..self.rows).step_by(BLOCK_SIZE) {
            for j0 in (0..other.cols).step_by(BLOCK_SIZE) {
                for k0 in (0..self.cols).step_by(BLOCK_SIZE) {
                    // Process each block
                    let i_max = (i0 + BLOCK_SIZE).min(self.rows);
                    let j_max = (j0 + BLOCK_SIZE).min(other.cols);
                    let k_max = (k0 + BLOCK_SIZE).min(self.cols);

                    for i in i0..i_max {
                        for j in j0..j_max {
                            let mut sum = result.get(i, j);
                            for k in k0..k_max {
                                sum += self.get(i, k) * other.get(k, j);
                            }
                            result.set(i, j, sum);
                        }
                    }
                }
            }
        }

        Ok(result)
    }

    /// Optimized cache-efficient matrix multiplication
    /// Uses blocking with additional optimizations
    pub fn multiply(&self, other: &Matrix) -> Result<Matrix, String> {
        if self.cols != other.rows {
            return Err(format!(
                "Cannot multiply {}x{} matrix with {}x{} matrix",
                self.rows, self.cols, other.rows, other.cols
            ));
        }

        // For small matrices, use naive algorithm
        if self.rows * self.cols * other.cols < 10000 {
            return self.multiply_naive(other);
        }

        let mut result = Matrix::new(self.rows, other.cols);

        // Adaptive block size based on matrix dimensions
        let block_size = Self::optimal_block_size(self.rows, self.cols, other.cols);

        // Create transposed version of the second matrix for better cache locality
        let other_t = other.transpose();

        // Process in blocks with transposed access pattern
        for i0 in (0..self.rows).step_by(block_size) {
            for j0 in (0..other.cols).step_by(block_size) {
                for k0 in (0..self.cols).step_by(block_size) {
                    let i_max = (i0 + block_size).min(self.rows);
                    let j_max = (j0 + block_size).min(other.cols);
                    let k_max = (k0 + block_size).min(self.cols);

                    // Inner loops with better cache access pattern
                    for i in i0..i_max {
                        let row_offset = i * self.cols;
                        for j in j0..j_max {
                            let col_offset = j * other_t.cols;
                            let mut sum = result.get(i, j);

                            // Unroll inner loop for better performance
                            let mut k = k0;
                            let k_end = k_max - (k_max - k0) % 4;

                            // Process 4 elements at a time
                            while k < k_end {
                                sum += self.data[row_offset + k] * other_t.data[col_offset + k]
                                    + self.data[row_offset + k + 1]
                                        * other_t.data[col_offset + k + 1]
                                    + self.data[row_offset + k + 2]
                                        * other_t.data[col_offset + k + 2]
                                    + self.data[row_offset + k + 3]
                                        * other_t.data[col_offset + k + 3];
                                k += 4;
                            }

                            // Handle remaining elements
                            while k < k_max {
                                sum += self.data[row_offset + k] * other_t.data[col_offset + k];
                                k += 1;
                            }

                            result.set(i, j, sum);
                        }
                    }
                }
            }
        }

        Ok(result)
    }

    /// Transpose the matrix for better cache access patterns
    fn transpose(&self) -> Matrix {
        let mut result = Matrix::new(self.cols, self.rows);

        // Block-wise transpose for cache efficiency
        const BLOCK_SIZE: usize = 32;

        for i0 in (0..self.rows).step_by(BLOCK_SIZE) {
            for j0 in (0..self.cols).step_by(BLOCK_SIZE) {
                let i_max = (i0 + BLOCK_SIZE).min(self.rows);
                let j_max = (j0 + BLOCK_SIZE).min(self.cols);

                for i in i0..i_max {
                    for j in j0..j_max {
                        result.set(j, i, self.get(i, j));
                    }
                }
            }
        }

        result
    }

    /// Calculate optimal block size based on matrix dimensions
    fn optimal_block_size(_m: usize, _k: usize, _n: usize) -> usize {
        // L1 cache size assumption: 64KB
        const L1_CACHE_SIZE: usize = 64 * 1024;
        const ELEMENT_SIZE: usize = 8; // size of f64

        // We need to fit 3 blocks in cache: A[block_size, block_size],
        // B[block_size, block_size], and C[block_size, block_size]
        let max_block_size = ((L1_CACHE_SIZE / ELEMENT_SIZE / 3) as f64).sqrt() as usize;

        // Find a block size that divides well into the matrix dimensions
        let mut block_size = max_block_size.min(64);

        // Prefer powers of 2 or multiples of 8 for better alignment
        if block_size > 32 {
            block_size = 64;
        } else if block_size > 16 {
            block_size = 32;
        } else {
            block_size = 16;
        }

        block_size
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_matrix_creation() {
        let mat = Matrix::new(3, 3);
        assert_eq!(mat.dimensions(), (3, 3));
    }

    #[test]
    fn test_matrix_from_vec() {
        let data = vec![
            vec![1.0, 2.0, 3.0],
            vec![4.0, 5.0, 6.0],
            vec![7.0, 8.0, 9.0],
        ];
        let mat = Matrix::from_vec(data);
        assert_eq!(mat.dimensions(), (3, 3));
        assert_eq!(mat.get(0, 0), 1.0);
        assert_eq!(mat.get(1, 1), 5.0);
        assert_eq!(mat.get(2, 2), 9.0);
    }

    #[test]
    fn test_matrix_set_get() {
        let mut mat = Matrix::new(2, 2);
        mat.set(0, 0, 1.0);
        mat.set(0, 1, 2.0);
        mat.set(1, 0, 3.0);
        mat.set(1, 1, 4.0);

        assert_eq!(mat.get(0, 0), 1.0);
        assert_eq!(mat.get(0, 1), 2.0);
        assert_eq!(mat.get(1, 0), 3.0);
        assert_eq!(mat.get(1, 1), 4.0);
    }

    #[test]
    fn test_transpose() {
        let mat = Matrix::from_vec(vec![vec![1.0, 2.0, 3.0], vec![4.0, 5.0, 6.0]]);

        let transposed = mat.transpose();
        assert_eq!(transposed.dimensions(), (3, 2));
        assert_eq!(transposed.get(0, 0), 1.0);
        assert_eq!(transposed.get(1, 0), 2.0);
        assert_eq!(transposed.get(2, 0), 3.0);
        assert_eq!(transposed.get(0, 1), 4.0);
        assert_eq!(transposed.get(1, 1), 5.0);
        assert_eq!(transposed.get(2, 1), 6.0);
    }

    #[test]
    fn test_multiplication_dimensions_mismatch() {
        let a = Matrix::new(2, 3);
        let b = Matrix::new(2, 2);

        assert!(a.multiply(&b).is_err());
        assert!(a.multiply_naive(&b).is_err());
        assert!(a.multiply_blocked(&b).is_err());
    }

    #[test]
    fn test_multiplication_identity() {
        let mut identity = Matrix::new(3, 3);
        for i in 0..3 {
            identity.set(i, i, 1.0);
        }

        let mat = Matrix::from_vec(vec![
            vec![1.0, 2.0, 3.0],
            vec![4.0, 5.0, 6.0],
            vec![7.0, 8.0, 9.0],
        ]);

        let result = mat.multiply(&identity).unwrap();
        assert_eq!(result, mat);
    }

    #[test]
    fn test_multiplication_simple() {
        let a = Matrix::from_vec(vec![vec![1.0, 2.0], vec![3.0, 4.0]]);

        let b = Matrix::from_vec(vec![vec![5.0, 6.0], vec![7.0, 8.0]]);

        let expected = Matrix::from_vec(vec![vec![19.0, 22.0], vec![43.0, 50.0]]);

        // Test all multiplication methods
        let result_naive = a.multiply_naive(&b).unwrap();
        let result_blocked = a.multiply_blocked(&b).unwrap();
        let result_optimized = a.multiply(&b).unwrap();

        assert_eq!(result_naive, expected);
        assert_eq!(result_blocked, expected);
        assert_eq!(result_optimized, expected);
    }

    #[test]
    fn test_multiplication_rectangular() {
        let a = Matrix::from_vec(vec![vec![1.0, 2.0, 3.0], vec![4.0, 5.0, 6.0]]);

        let b = Matrix::from_vec(vec![vec![7.0, 8.0], vec![9.0, 10.0], vec![11.0, 12.0]]);

        let expected = Matrix::from_vec(vec![vec![58.0, 64.0], vec![139.0, 154.0]]);

        let result = a.multiply(&b).unwrap();
        assert_eq!(result, expected);
    }

    #[test]
    fn test_multiplication_large() {
        let size = 100;
        let mut a = Matrix::new(size, size);
        let mut b = Matrix::new(size, size);

        // Fill with simple pattern
        for i in 0..size {
            for j in 0..size {
                a.set(i, j, (i + j) as f64);
                b.set(i, j, (i * j) as f64);
            }
        }

        // All methods should produce the same result
        let result_naive = a.multiply_naive(&b).unwrap();
        let result_blocked = a.multiply_blocked(&b).unwrap();
        let result_optimized = a.multiply(&b).unwrap();

        // Check a few sample values
        assert_eq!(result_naive.get(0, 0), result_blocked.get(0, 0));
        assert_eq!(result_naive.get(0, 0), result_optimized.get(0, 0));
        assert_eq!(result_naive.get(50, 50), result_blocked.get(50, 50));
        assert_eq!(result_naive.get(50, 50), result_optimized.get(50, 50));
        assert_eq!(result_naive.get(99, 99), result_blocked.get(99, 99));
        assert_eq!(result_naive.get(99, 99), result_optimized.get(99, 99));
    }

    #[test]
    fn test_optimal_block_size() {
        // Test that optimal block size returns reasonable values
        assert!(Matrix::optimal_block_size(100, 100, 100) > 0);
        assert!(Matrix::optimal_block_size(1000, 1000, 1000) <= 64);
        assert!(Matrix::optimal_block_size(32, 32, 32) >= 16);
    }
}
