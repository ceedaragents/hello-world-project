//! Integration tests for MCTS library

use mcts::{GameState, Player, MCTSConfig, MCTSTree, ConcurrentMCTS};
use std::sync::Arc;
use std::time::Duration;

/// Simple counting game for testing
#[derive(Clone, Debug)]
struct CountingGame {
    count: i32,
    max_count: i32,
    current_player: Player,
}

impl CountingGame {
    fn new(max_count: i32) -> Self {
        Self {
            count: 0,
            max_count,
            current_player: Player::One,
        }
    }
}

impl GameState for CountingGame {
    type Action = i32; // Add 1, 2, or 3
    
    fn current_player(&self) -> Player {
        self.current_player
    }
    
    fn legal_actions(&self) -> Vec<Self::Action> {
        if self.is_terminal() {
            vec![]
        } else {
            let remaining = self.max_count - self.count;
            (1..=3).filter(|&x| x <= remaining).collect()
        }
    }
    
    fn apply_action(&self, action: &Self::Action) -> Self {
        Self {
            count: self.count + action,
            max_count: self.max_count,
            current_player: self.current_player.opponent(),
        }
    }
    
    fn is_terminal(&self) -> bool {
        self.count >= self.max_count
    }
    
    fn terminal_value(&self, player: Player) -> Option<f64> {
        if self.is_terminal() {
            // The player who made the last move wins
            let winner = self.current_player.opponent();
            Some(if winner == player { 1.0 } else { 0.0 })
        } else {
            None
        }
    }
}

#[test]
fn test_single_threaded_mcts() {
    let game = CountingGame::new(10);
    let config = MCTSConfig {
        exploration_constant: 1.414,
        max_iterations: 1000,
        max_simulation_depth: 50,
        num_threads: 1,
    };
    
    let tree = MCTSTree::new(game, config);
    let result = tree.run();
    
    assert_eq!(result.iterations, 1000);
    assert!(result.root_visits >= 1000);
    assert!(!result.action_visits.is_empty());
    assert!(result.action_visits.iter().any(|(_, visits)| *visits > 0));
}

#[test]
fn test_concurrent_mcts() {
    let game = CountingGame::new(15);
    let config = MCTSConfig {
        exploration_constant: 1.414,
        max_iterations: 5000,
        max_simulation_depth: 50,
        num_threads: 4,
    };
    
    let mcts = ConcurrentMCTS::new(game, config);
    let result = mcts.run();
    
    assert!(result.iterations >= 4000); // Some overhead is expected
    assert!(result.root_visits > 0);
    assert!(!result.action_visits.is_empty());
}

#[test]
fn test_concurrent_mcts_timed() {
    let game = CountingGame::new(20);
    let config = MCTSConfig {
        exploration_constant: 1.414,
        max_iterations: 100000, // High limit, time will be the constraint
        max_simulation_depth: 50,
        num_threads: 2,
    };
    
    let mcts = ConcurrentMCTS::new(game, config);
    let result = mcts.run_timed(Duration::from_millis(100));
    
    assert!(result.iterations > 0);
    assert!(result.root_visits > 0);
    assert!(!result.action_visits.is_empty());
}

#[test]
fn test_deterministic_win() {
    // In a game where count reaches 21, the first player can always win
    // by ensuring the count is always 1, 5, 9, 13, 17, 21 after their turn
    let game = CountingGame::new(21);
    let config = MCTSConfig {
        exploration_constant: 0.1, // Lower exploration for more exploitation
        max_iterations: 10000,
        max_simulation_depth: 50,
        num_threads: 1,
    };
    
    let tree = MCTSTree::new(game, config);
    let result = tree.run();
    
    // First player should choose 1 to start the winning sequence
    assert_eq!(result.best_action, 1);
}

#[test]
fn test_memory_safety_concurrent() {
    // Test that concurrent access is memory safe
    let game = CountingGame::new(30);
    let config = MCTSConfig {
        exploration_constant: 1.414,
        max_iterations: 10000,
        max_simulation_depth: 100,
        num_threads: 8, // Many threads to test concurrency
    };
    
    let mcts = Arc::new(ConcurrentMCTS::new(game, config));
    
    // Run multiple times to ensure no data races
    for _ in 0..5 {
        let mcts_clone = mcts.clone();
        std::thread::spawn(move || {
            let _result = mcts_clone.run_timed(Duration::from_millis(50));
        });
    }
    
    std::thread::sleep(Duration::from_millis(300));
}

#[test]
fn test_node_expansion() {
    use mcts::node::Node;
    
    let game = CountingGame::new(10);
    let root = Node::new_root(game.clone());
    
    assert_eq!(root.visits(), 0);
    assert!(!root.is_terminal());
    assert!(!root.is_fully_expanded());
    
    // Update statistics
    root.update(0.5);
    assert_eq!(root.visits(), 1);
    assert_eq!(root.avg_value(), 0.5);
    
    // Add children
    let actions = game.legal_actions();
    for action in &actions {
        let child_state = game.apply_action(action);
        let child = Node::new_child(child_state, *action, root.clone());
        root.add_child(*action, child);
    }
    
    assert!(root.is_fully_expanded());
}

#[test]
fn test_ucb1_calculation() {
    use mcts::node::Node;
    
    let game = CountingGame::new(10);
    let root = Node::new_root(game.clone());
    
    // Create and add a child
    let action = 1;
    let child_state = game.apply_action(&action);
    let child = Node::new_child(child_state, action, root.clone());
    root.add_child(action, child.clone());
    
    // Update both nodes
    root.update(0.0);
    root.update(0.0);
    child.update(1.0);
    
    // Calculate UCB1
    let ucb_value = root.ucb1_value(&child, 1.414);
    assert!(ucb_value > 0.0);
    assert!(ucb_value < f64::INFINITY);
}

/// Performance comparison test
#[test]
#[ignore] // Run with --ignored flag
fn test_performance_comparison() {
    let game = CountingGame::new(50);
    let iterations = 50000;
    
    // Single-threaded
    let config_single = MCTSConfig {
        exploration_constant: 1.414,
        max_iterations: iterations,
        max_simulation_depth: 100,
        num_threads: 1,
    };
    
    let start = std::time::Instant::now();
    let tree = MCTSTree::new(game.clone(), config_single);
    let _result1 = tree.run();
    let single_duration = start.elapsed();
    
    // Multi-threaded
    let config_multi = MCTSConfig {
        exploration_constant: 1.414,
        max_iterations: iterations,
        max_simulation_depth: 100,
        num_threads: 4,
    };
    
    let start = std::time::Instant::now();
    let mcts = ConcurrentMCTS::new(game, config_multi);
    let _result2 = mcts.run();
    let multi_duration = start.elapsed();
    
    println!("Single-threaded: {:?}", single_duration);
    println!("Multi-threaded (4 threads): {:?}", multi_duration);
    println!("Speedup: {:.2}x", single_duration.as_secs_f64() / multi_duration.as_secs_f64());
    
    // Multi-threaded should be faster
    assert!(multi_duration < single_duration);
}