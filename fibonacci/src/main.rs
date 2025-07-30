use std::env;

struct Fibonacci {
    current: u64,
    next: u64,
}

impl Fibonacci {
    fn new() -> Self {
        Fibonacci {
            current: 0,
            next: 1,
        }
    }
}

impl Iterator for Fibonacci {
    type Item = u64;

    fn next(&mut self) -> Option<Self::Item> {
        let current = self.current;
        self.current = self.next;
        self.next = current.saturating_add(self.next);
        Some(current)
    }
}

fn fibonacci(n: u32) -> u64 {
    Fibonacci::new().nth(n as usize).unwrap_or(0)
}

fn fibonacci_sequence(count: usize) -> Vec<u64> {
    Fibonacci::new().take(count).collect()
}

fn main() {
    let args: Vec<String> = env::args().collect();
    
    let count = if args.len() > 1 {
        args[1].parse::<usize>().unwrap_or(10)
    } else {
        10
    };
    
    println!("Fibonacci sequence (first {} numbers):", count);
    
    // Using the iterator directly
    for (i, num) in Fibonacci::new().take(count).enumerate() {
        println!("F({}) = {}", i, num);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_fibonacci_base_cases() {
        assert_eq!(fibonacci(0), 0);
        assert_eq!(fibonacci(1), 1);
    }

    #[test]
    fn test_fibonacci_small_values() {
        assert_eq!(fibonacci(2), 1);
        assert_eq!(fibonacci(3), 2);
        assert_eq!(fibonacci(4), 3);
        assert_eq!(fibonacci(5), 5);
        assert_eq!(fibonacci(6), 8);
        assert_eq!(fibonacci(7), 13);
    }

    #[test]
    fn test_fibonacci_larger_values() {
        assert_eq!(fibonacci(10), 55);
        assert_eq!(fibonacci(15), 610);
        assert_eq!(fibonacci(20), 6765);
    }

    #[test]
    fn test_fibonacci_sequence() {
        let seq = fibonacci_sequence(8);
        assert_eq!(seq, vec![0, 1, 1, 2, 3, 5, 8, 13]);
    }

    #[test]
    fn test_fibonacci_sequence_empty() {
        let seq = fibonacci_sequence(0);
        assert_eq!(seq, vec![]);
    }

    #[test]
    fn test_fibonacci_sequence_single() {
        let seq = fibonacci_sequence(1);
        assert_eq!(seq, vec![0]);
    }

    #[test]
    fn test_fibonacci_iterator() {
        let mut fib = Fibonacci::new();
        assert_eq!(fib.next(), Some(0));
        assert_eq!(fib.next(), Some(1));
        assert_eq!(fib.next(), Some(1));
        assert_eq!(fib.next(), Some(2));
        assert_eq!(fib.next(), Some(3));
        assert_eq!(fib.next(), Some(5));
        assert_eq!(fib.next(), Some(8));
        assert_eq!(fib.next(), Some(13));
    }

    #[test]
    fn test_fibonacci_iterator_take() {
        let fib_iter = Fibonacci::new();
        let first_ten: Vec<u64> = fib_iter.take(10).collect();
        assert_eq!(first_ten, vec![0, 1, 1, 2, 3, 5, 8, 13, 21, 34]);
    }

    #[test]
    fn test_fibonacci_iterator_nth() {
        assert_eq!(Fibonacci::new().nth(0), Some(0));
        assert_eq!(Fibonacci::new().nth(1), Some(1));
        assert_eq!(Fibonacci::new().nth(10), Some(55));
        assert_eq!(Fibonacci::new().nth(20), Some(6765));
    }

    #[test]
    fn test_fibonacci_iterator_skip() {
        let fib_iter = Fibonacci::new();
        let skip_five: Vec<u64> = fib_iter.skip(5).take(5).collect();
        assert_eq!(skip_five, vec![5, 8, 13, 21, 34]);
    }
}