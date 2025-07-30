//! Concurrent MCTS implementation using thread pools

use crate::{
    node::Node,
    traits::{GameState, SimulationPolicy, RandomPolicy},
    MCTSConfig, MCTSResult,
};
use rayon::prelude::*;
use std::sync::{Arc, atomic::{AtomicBool, AtomicUsize, Ordering}};
use std::time::{Duration, Instant};

/// Concurrent Monte Carlo Tree Search implementation
pub struct ConcurrentMCTS<S: GameState> {
    root: Arc<Node<S>>,
    config: MCTSConfig,
    iterations_completed: AtomicUsize,
    should_stop: AtomicBool,
}

impl<S: GameState> ConcurrentMCTS<S> {
    /// Create a new concurrent MCTS instance
    pub fn new(root_state: S, config: MCTSConfig) -> Self {
        // Set up thread pool
        rayon::ThreadPoolBuilder::new()
            .num_threads(config.num_threads)
            .build_global()
            .unwrap_or(());
        
        Self {
            root: Node::new_root(root_state),
            config,
            iterations_completed: AtomicUsize::new(0),
            should_stop: AtomicBool::new(false),
        }
    }
    
    /// Run concurrent MCTS for the configured number of iterations
    pub fn run(&self) -> MCTSResult<S::Action> {
        let policy = RandomPolicy;
        let start_time = Instant::now();
        
        // Run iterations in parallel
        (0..self.config.num_threads).into_par_iter().for_each(|_| {
            self.worker_loop(&policy);
        });
        
        let elapsed = start_time.elapsed();
        println!("MCTS completed in {:?}", elapsed);
        
        MCTSResult {
            best_action: self.root.best_action().expect("No actions available"),
            iterations: self.iterations_completed.load(Ordering::Relaxed),
            root_visits: self.root.visits(),
            action_visits: self.root.action_stats(),
        }
    }
    
    /// Run concurrent MCTS with a time limit
    pub fn run_timed(&self, duration: Duration) -> MCTSResult<S::Action> {
        let policy = RandomPolicy;
        let start_time = Instant::now();
        
        // Run iterations in parallel with time limit
        (0..self.config.num_threads).into_par_iter().for_each(|_| {
            while start_time.elapsed() < duration {
                if !self.run_single_iteration(&policy) {
                    break;
                }
            }
        });
        
        MCTSResult {
            best_action: self.root.best_action().expect("No actions available"),
            iterations: self.iterations_completed.load(Ordering::Relaxed),
            root_visits: self.root.visits(),
            action_visits: self.root.action_stats(),
        }
    }
    
    /// Worker loop for a thread
    fn worker_loop(&self, policy: &impl SimulationPolicy<S>) {
        loop {
            if self.should_stop.load(Ordering::Relaxed) {
                break;
            }
            
            let current_iterations = self.iterations_completed.fetch_add(1, Ordering::Relaxed);
            if current_iterations >= self.config.max_iterations {
                self.should_stop.store(true, Ordering::Relaxed);
                break;
            }
            
            self.run_single_iteration(policy);
        }
    }
    
    /// Run a single MCTS iteration
    fn run_single_iteration(&self, policy: &impl SimulationPolicy<S>) -> bool {
        // Selection with virtual loss
        let (leaf, path) = self.select_with_virtual_loss(&self.root);
        
        // Expansion
        let node_to_simulate = if !leaf.is_terminal() && leaf.visits() > 0 {
            self.expand(leaf.clone()).unwrap_or(leaf.clone())
        } else {
            leaf.clone()
        };
        
        // Simulation
        let value = self.simulate(&node_to_simulate, policy);
        
        // Backpropagation (removes virtual loss)
        self.backpropagate_concurrent(node_to_simulate, value, &path);
        
        true
    }
    
    /// Selection phase with virtual loss for concurrent execution
    fn select_with_virtual_loss(&self, node: &Arc<Node<S>>) -> (Arc<Node<S>>, Vec<Arc<Node<S>>>) {
        let mut current = node.clone();
        let mut path = vec![current.clone()];
        
        // Apply virtual loss to root
        current.stats.write().visits += 1;
        
        while !current.is_terminal() && current.is_fully_expanded() {
            if let Some(best_child) = current.best_child(self.config.exploration_constant) {
                // Apply virtual loss
                best_child.stats.write().visits += 1;
                path.push(best_child.clone());
                current = best_child;
            } else {
                break;
            }
        }
        
        (current, path)
    }
    
    /// Thread-safe expansion
    fn expand(&self, node: Arc<Node<S>>) -> Option<Arc<Node<S>>> {
        if node.is_terminal() {
            return None;
        }
        
        let unexpanded = node.unexpanded_actions();
        if unexpanded.is_empty() {
            return None;
        }
        
        // Try to expand with a random action
        use rand::seq::SliceRandom;
        let mut rng = rand::thread_rng();
        
        // Try multiple times in case of concurrent expansions
        for _ in 0..3 {
            let unexpanded = node.unexpanded_actions();
            if let Some(action) = unexpanded.choose(&mut rng) {
                let action = action.clone();
                
                // Check if another thread already expanded this action
                {
                    let children = node.children.read();
                    if children.contains_key(&action) {
                        continue;
                    }
                }
                
                // Create new state and child node
                let new_state = node.state.apply_action(&action);
                let child = Node::new_child(new_state, action.clone(), node.clone());
                
                // Add child to parent (thread-safe)
                node.add_child(action, child.clone());
                
                return Some(child);
            }
        }
        
        None
    }
    
    /// Simulation phase (same as single-threaded)
    fn simulate(&self, node: &Arc<Node<S>>, policy: &impl SimulationPolicy<S>) -> f64 {
        let mut state = node.state.clone();
        let simulation_player = node.parent
            .as_ref()
            .map(|p| p.state.current_player())
            .unwrap_or(crate::Player::One);
        
        let mut depth = 0;
        while !state.is_terminal() && depth < self.config.max_simulation_depth {
            let actions = state.legal_actions();
            if actions.is_empty() {
                break;
            }
            
            let action = policy.select_action(&state, &actions);
            state = state.apply_action(&action);
            depth += 1;
        }
        
        state.evaluate(simulation_player)
    }
    
    /// Concurrent backpropagation (removes virtual loss)
    fn backpropagate_concurrent(&self, mut node: Arc<Node<S>>, value: f64, path: &[Arc<Node<S>>]) {
        // Update from leaf to root
        loop {
            {
                let mut stats = node.stats.write();
                stats.visits -= 1; // Remove virtual loss
                stats.visits += 1; // Add real visit
                stats.total_value += value;
                stats.avg_value = stats.total_value / stats.visits as f64;
            }
            
            if let Some(parent) = &node.parent {
                node = parent.clone();
            } else {
                break;
            }
        }
        
        // Remove remaining virtual losses from path
        for node in path {
            let mut stats = node.stats.write();
            if stats.visits > 0 {
                stats.visits -= 1;
            }
        }
    }
    
    /// Stop the concurrent execution
    pub fn stop(&self) {
        self.should_stop.store(true, Ordering::Relaxed);
    }
    
    /// Get the root node
    pub fn root(&self) -> &Arc<Node<S>> {
        &self.root
    }
    
    /// Get the number of completed iterations
    pub fn iterations_completed(&self) -> usize {
        self.iterations_completed.load(Ordering::Relaxed)
    }
}