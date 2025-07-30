//! MCTS tree implementation

use crate::{
    node::Node,
    traits::{GameState, Player, SimulationPolicy, RandomPolicy},
    MCTSConfig, MCTSResult,
};
use rand::seq::SliceRandom;
use std::sync::Arc;

/// Monte Carlo Tree Search implementation
pub struct MCTSTree<S: GameState> {
    root: Arc<Node<S>>,
    config: MCTSConfig,
}

impl<S: GameState> MCTSTree<S> {
    /// Create a new MCTS tree with the given root state
    pub fn new(root_state: S, config: MCTSConfig) -> Self {
        Self {
            root: Node::new_root(root_state),
            config,
        }
    }
    
    /// Run MCTS for the configured number of iterations
    pub fn run(&self) -> MCTSResult<S::Action> {
        let policy = RandomPolicy;
        
        for i in 0..self.config.max_iterations {
            self.run_iteration(&policy);
        }
        
        MCTSResult {
            best_action: self.root.best_action().expect("No actions available"),
            iterations: self.config.max_iterations,
            root_visits: self.root.visits(),
            action_visits: self.root.action_stats(),
        }
    }
    
    /// Run a single MCTS iteration
    fn run_iteration(&self, policy: &impl SimulationPolicy<S>) {
        // Selection
        let leaf = self.select(&self.root);
        
        // Expansion
        let node_to_simulate = if !leaf.is_terminal() && leaf.visits() > 0 {
            self.expand(leaf.clone()).unwrap_or(leaf.clone())
        } else {
            leaf.clone()
        };
        
        // Simulation
        let value = self.simulate(&node_to_simulate, policy);
        
        // Backpropagation
        self.backpropagate(node_to_simulate, value);
    }
    
    /// Selection phase: traverse tree using UCB1
    fn select(&self, node: &Arc<Node<S>>) -> Arc<Node<S>> {
        let mut current = node.clone();
        
        while !current.is_terminal() && current.is_fully_expanded() {
            if let Some(best_child) = current.best_child(self.config.exploration_constant) {
                current = best_child;
            } else {
                break;
            }
        }
        
        current
    }
    
    /// Expansion phase: add a new child to the tree
    fn expand(&self, node: Arc<Node<S>>) -> Option<Arc<Node<S>>> {
        if node.is_terminal() {
            return None;
        }
        
        let unexpanded = node.unexpanded_actions();
        if unexpanded.is_empty() {
            return None;
        }
        
        // Choose a random unexpanded action
        let mut rng = rand::thread_rng();
        let action = unexpanded.choose(&mut rng)?.clone();
        
        // Create new state and child node
        let new_state = node.state.apply_action(&action);
        let child = Node::new_child(new_state, action.clone(), node.clone());
        
        // Add child to parent
        node.add_child(action, child.clone());
        
        Some(child)
    }
    
    /// Simulation phase: play out random game
    fn simulate(&self, node: &Arc<Node<S>>, policy: &impl SimulationPolicy<S>) -> f64 {
        let mut state = node.state.clone();
        let simulation_player = node.parent
            .as_ref()
            .map(|p| p.state.current_player())
            .unwrap_or(Player::One);
        
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
    
    /// Backpropagation phase: update statistics
    fn backpropagate(&self, mut node: Arc<Node<S>>, value: f64) {
        loop {
            node.update(value);
            
            if let Some(parent) = &node.parent {
                node = parent.clone();
            } else {
                break;
            }
        }
    }
    
    /// Get the root node
    pub fn root(&self) -> &Arc<Node<S>> {
        &self.root
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    
    // Simple test game state for testing
    #[derive(Clone, Debug)]
    struct TestState {
        value: i32,
        player: Player,
        terminal: bool,
    }
    
    impl GameState for TestState {
        type Action = i32;
        
        fn current_player(&self) -> Player {
            self.player
        }
        
        fn legal_actions(&self) -> Vec<Self::Action> {
            if self.terminal {
                vec![]
            } else {
                vec![1, 2, 3]
            }
        }
        
        fn apply_action(&self, action: &Self::Action) -> Self {
            TestState {
                value: self.value + action,
                player: self.player.opponent(),
                terminal: self.value + action >= 10,
            }
        }
        
        fn is_terminal(&self) -> bool {
            self.terminal
        }
        
        fn terminal_value(&self, player: Player) -> Option<f64> {
            if self.terminal {
                Some(if player == Player::One { 1.0 } else { 0.0 })
            } else {
                None
            }
        }
    }
    
    #[test]
    fn test_mcts_creation() {
        let state = TestState {
            value: 0,
            player: Player::One,
            terminal: false,
        };
        let config = MCTSConfig::default();
        let tree = MCTSTree::new(state, config);
        
        assert_eq!(tree.root().visits(), 0);
        assert!(!tree.root().is_terminal());
    }
}