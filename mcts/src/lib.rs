//! A memory-safe Monte Carlo Tree Search library with concurrency support.
//!
//! This library provides a generic implementation of MCTS that can be used
//! for various game-playing and decision-making applications.

use std::sync::Arc;
use parking_lot::{RwLock, Mutex};
use rand::Rng;
use std::fmt::Debug;
use std::hash::Hash;

pub mod node;
pub mod tree;
pub mod traits;
pub mod concurrent;

pub use traits::{GameState, Player};
pub use node::Node;
pub use tree::MCTSTree;
pub use concurrent::ConcurrentMCTS;

/// Configuration for MCTS algorithm
#[derive(Debug, Clone)]
pub struct MCTSConfig {
    /// Exploration constant (typically sqrt(2))
    pub exploration_constant: f64,
    /// Maximum number of iterations to run
    pub max_iterations: usize,
    /// Maximum depth for simulation
    pub max_simulation_depth: usize,
    /// Number of threads for concurrent execution
    pub num_threads: usize,
}

impl Default for MCTSConfig {
    fn default() -> Self {
        Self {
            exploration_constant: std::f64::consts::SQRT_2,
            max_iterations: 10000,
            max_simulation_depth: 1000,
            num_threads: 4,
        }
    }
}

/// Result of running MCTS
#[derive(Debug, Clone)]
pub struct MCTSResult<A> {
    /// The best action found
    pub best_action: A,
    /// Number of iterations performed
    pub iterations: usize,
    /// Root node visit count
    pub root_visits: usize,
    /// Action visit counts
    pub action_visits: Vec<(A, usize)>,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_config_default() {
        let config = MCTSConfig::default();
        assert_eq!(config.exploration_constant, std::f64::consts::SQRT_2);
        assert_eq!(config.max_iterations, 10000);
        assert_eq!(config.max_simulation_depth, 1000);
        assert_eq!(config.num_threads, 4);
    }
}