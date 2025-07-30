//! MCTS tree node implementation

use crate::traits::{GameState, Player};
use std::sync::Arc;
use std::collections::HashMap;
use std::cell::{RefCell, Cell};

/// Statistics for a node
#[derive(Debug, Clone)]
pub struct NodeStats {
    /// Total number of visits
    pub visits: usize,
    /// Total value accumulated
    pub total_value: f64,
    /// Average value (total_value / visits)
    pub avg_value: f64,
}

impl Default for NodeStats {
    fn default() -> Self {
        Self {
            visits: 0,
            total_value: 0.0,
            avg_value: 0.0,
        }
    }
}

/// A node in the MCTS tree
pub struct Node<S: GameState> {
    /// The game state at this node
    pub state: S,
    /// The action that led to this node
    pub action: Option<S::Action>,
    /// The parent node
    pub parent: Option<Arc<Node<S>>>,
    /// Child nodes indexed by action
    pub children: RefCell<HashMap<S::Action, Arc<Node<S>>>>,
    /// Node statistics
    pub stats: RefCell<NodeStats>,
    /// Whether this node has been fully expanded
    pub fully_expanded: Cell<bool>,
    /// Cached legal actions
    legal_actions: Vec<S::Action>,
}

impl<S: GameState> Node<S> {
    /// Create a new root node
    pub fn new_root(state: S) -> Arc<Self> {
        let legal_actions = state.legal_actions();
        let fully_expanded = legal_actions.is_empty();
        
        Arc::new(Self {
            state,
            action: None,
            parent: None,
            children: RefCell::new(HashMap::new()),
            stats: RefCell::new(NodeStats::default()),
            fully_expanded: Cell::new(fully_expanded),
            legal_actions,
        })
    }
    
    /// Create a new child node
    pub fn new_child(state: S, action: S::Action, parent: Arc<Node<S>>) -> Arc<Self> {
        let legal_actions = state.legal_actions();
        let fully_expanded = legal_actions.is_empty();
        
        Arc::new(Self {
            state,
            action: Some(action),
            parent: Some(parent),
            children: RefCell::new(HashMap::new()),
            stats: RefCell::new(NodeStats::default()),
            fully_expanded: Cell::new(fully_expanded),
            legal_actions,
        })
    }
    
    /// Check if this is a terminal node
    pub fn is_terminal(&self) -> bool {
        self.state.is_terminal()
    }
    
    /// Check if this node is fully expanded
    pub fn is_fully_expanded(&self) -> bool {
        self.fully_expanded.get()
    }
    
    /// Get the number of visits
    pub fn visits(&self) -> usize {
        self.stats.borrow().visits
    }
    
    /// Get the average value
    pub fn avg_value(&self) -> f64 {
        self.stats.borrow().avg_value
    }
    
    /// Get unexpanded actions
    pub fn unexpanded_actions(&self) -> Vec<S::Action> {
        let children = self.children.borrow();
        self.legal_actions
            .iter()
            .filter(|action| !children.contains_key(action))
            .cloned()
            .collect()
    }
    
    /// Update statistics with a new value
    pub fn update(&self, value: f64) {
        let mut stats = self.stats.borrow_mut();
        stats.visits += 1;
        stats.total_value += value;
        stats.avg_value = stats.total_value / stats.visits as f64;
    }
    
    /// Add a child node
    pub fn add_child(self: &Arc<Self>, action: S::Action, child: Arc<Node<S>>) {
        let mut children = self.children.borrow_mut();
        children.insert(action.clone(), child);
        
        // Check if fully expanded
        if children.len() == self.legal_actions.len() {
            self.fully_expanded.set(true);
        }
    }
    
    /// Calculate UCB1 value for a child
    pub fn ucb1_value(&self, child: &Node<S>, exploration_constant: f64) -> f64 {
        let child_stats = child.stats.borrow();
        if child_stats.visits == 0 {
            f64::INFINITY
        } else {
            let parent_visits = self.visits() as f64;
            let child_visits = child_stats.visits as f64;
            let exploitation = child_stats.avg_value;
            let exploration = exploration_constant * (parent_visits.ln() / child_visits).sqrt();
            
            // Adjust value based on whose turn it is
            let value = if self.state.current_player() == Player::One {
                exploitation
            } else {
                1.0 - exploitation
            };
            
            value + exploration
        }
    }
    
    /// Select the best child using UCB1
    pub fn best_child(&self, exploration_constant: f64) -> Option<Arc<Node<S>>> {
        let children = self.children.borrow();
        children
            .values()
            .max_by(|a, b| {
                let a_value = self.ucb1_value(a, exploration_constant);
                let b_value = self.ucb1_value(b, exploration_constant);
                a_value.partial_cmp(&b_value).unwrap()
            })
            .cloned()
    }
    
    /// Get the best action based on visit count
    pub fn best_action(&self) -> Option<S::Action> {
        let children = self.children.borrow();
        children
            .iter()
            .max_by_key(|(_, child)| child.visits())
            .map(|(action, _)| action.clone())
    }
    
    /// Get action statistics
    pub fn action_stats(&self) -> Vec<(S::Action, usize)> {
        let children = self.children.borrow();
        let mut stats: Vec<_> = children
            .iter()
            .map(|(action, child)| (action.clone(), child.visits()))
            .collect();
        stats.sort_by(|a, b| b.1.cmp(&a.1));
        stats
    }
}