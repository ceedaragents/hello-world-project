use std::env;

fn fibonacci(n: u32) -> u64 {
    match n {
        0 => 0,
        1 => 1,
        _ => {
            let mut a = 0u64;
            let mut b = 1u64;
            for _ in 2..=n {
                let temp = a + b;
                a = b;
                b = temp;
            }
            b
        }
    }
}

fn fibonacci_sequence(count: usize) -> Vec<u64> {
    (0..count as u32).map(fibonacci).collect()
}

fn main() {
    let args: Vec<String> = env::args().collect();
    
    let count = if args.len() > 1 {
        args[1].parse::<usize>().unwrap_or(10)
    } else {
        10
    };
    
    println!("Fibonacci sequence (first {} numbers):", count);
    let sequence = fibonacci_sequence(count);
    
    for (i, num) in sequence.iter().enumerate() {
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
}