//! Core traits for implementing MCTS

use std::fmt::Debug;
use std::hash::Hash;

/// Represents a player in a game
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Player {
    /// First player
    One,
    /// Second player
    Two,
}

impl Player {
    /// Get the opponent player
    pub fn opponent(&self) -> Self {
        match self {
            Player::One => Player::Two,
            Player::Two => Player::One,
        }
    }
}

/// Trait for game states that can be used with MCTS
pub trait GameState: Clone + Send + Sync {
    /// The type of actions that can be taken
    type Action: Clone + Debug + PartialEq + Eq + Hash + Send + Sync;
    
    /// Get the current player
    fn current_player(&self) -> Player;
    
    /// Get all legal actions from this state
    fn legal_actions(&self) -> Vec<Self::Action>;
    
    /// Apply an action to get a new state
    fn apply_action(&self, action: &Self::Action) -> Self;
    
    /// Check if the game is over
    fn is_terminal(&self) -> bool;
    
    /// Get the value of a terminal state for a given player
    /// Returns Some(value) if terminal, None otherwise
    /// Value should be in range [0, 1] where 1 is win, 0 is loss, 0.5 is draw
    fn terminal_value(&self, player: Player) -> Option<f64>;
    
    /// Heuristic evaluation of the state for a given player
    /// Used during simulations
    fn evaluate(&self, player: Player) -> f64 {
        if let Some(value) = self.terminal_value(player) {
            value
        } else {
            0.5 // Default to neutral if not terminal
        }
    }
}

/// Trait for simulation policies
pub trait SimulationPolicy<S: GameState>: Send + Sync {
    /// Select an action during simulation
    fn select_action(&self, state: &S, actions: &[S::Action]) -> S::Action;
}

/// Default random simulation policy
pub struct RandomPolicy;

impl<S: GameState> SimulationPolicy<S> for RandomPolicy {
    fn select_action(&self, _state: &S, actions: &[S::Action]) -> S::Action {
        use rand::seq::SliceRandom;
        let mut rng = rand::thread_rng();
        actions.choose(&mut rng).unwrap().clone()
    }
}