//! Tic-Tac-Toe example using MCTS

use mcts::{GameState, Player, MCTSConfig, MCTSTree};
use std::fmt;

/// Tic-Tac-Toe board position
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Position {
    row: usize,
    col: usize,
}

impl Position {
    pub fn new(row: usize, col: usize) -> Self {
        Self { row, col }
    }
}

/// Tic-Tac-Toe game state
#[derive(Clone)]
pub struct TicTacToe {
    board: [[Option<Player>; 3]; 3],
    current_player: Player,
}

impl TicTacToe {
    /// Create a new empty board
    pub fn new() -> Self {
        Self {
            board: [[None; 3]; 3],
            current_player: Player::One,
        }
    }
    
    /// Make a move
    pub fn make_move(&mut self, pos: Position) {
        self.board[pos.row][pos.col] = Some(self.current_player);
        self.current_player = self.current_player.opponent();
    }
    
    /// Check if a player has won
    fn has_won(&self, player: Player) -> bool {
        // Check rows
        for row in &self.board {
            if row.iter().all(|&cell| cell == Some(player)) {
                return true;
            }
        }
        
        // Check columns
        for col in 0..3 {
            if (0..3).all(|row| self.board[row][col] == Some(player)) {
                return true;
            }
        }
        
        // Check diagonals
        if (0..3).all(|i| self.board[i][i] == Some(player)) {
            return true;
        }
        if (0..3).all(|i| self.board[i][2 - i] == Some(player)) {
            return true;
        }
        
        false
    }
    
    /// Check if the board is full
    fn is_full(&self) -> bool {
        self.board.iter().all(|row| row.iter().all(|cell| cell.is_some()))
    }
}

impl GameState for TicTacToe {
    type Action = Position;
    
    fn current_player(&self) -> Player {
        self.current_player
    }
    
    fn legal_actions(&self) -> Vec<Self::Action> {
        let mut actions = Vec::new();
        for row in 0..3 {
            for col in 0..3 {
                if self.board[row][col].is_none() {
                    actions.push(Position::new(row, col));
                }
            }
        }
        actions
    }
    
    fn apply_action(&self, action: &Self::Action) -> Self {
        let mut new_state = self.clone();
        new_state.board[action.row][action.col] = Some(self.current_player);
        new_state.current_player = self.current_player.opponent();
        new_state
    }
    
    fn is_terminal(&self) -> bool {
        self.has_won(Player::One) || self.has_won(Player::Two) || self.is_full()
    }
    
    fn terminal_value(&self, player: Player) -> Option<f64> {
        if !self.is_terminal() {
            return None;
        }
        
        if self.has_won(player) {
            Some(1.0)
        } else if self.has_won(player.opponent()) {
            Some(0.0)
        } else {
            Some(0.5) // Draw
        }
    }
}

impl fmt::Display for TicTacToe {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "┌───┬───┬───┐")?;
        for (i, row) in self.board.iter().enumerate() {
            write!(f, "│")?;
            for (j, cell) in row.iter().enumerate() {
                match cell {
                    Some(Player::One) => write!(f, " X ")?,
                    Some(Player::Two) => write!(f, " O ")?,
                    None => write!(f, "   ")?,
                }
                if j < 2 {
                    write!(f, "│")?;
                }
            }
            writeln!(f, "│")?;
            if i < 2 {
                writeln!(f, "├───┼───┼───┤")?;
            }
        }
        writeln!(f, "└───┴───┴───┘")?;
        Ok(())
    }
}

fn main() {
    println!("Tic-Tac-Toe MCTS Example");
    println!("========================\n");
    
    // Create initial game state
    let mut game = TicTacToe::new();
    println!("Initial board:");
    println!("{}", game);
    
    // Game loop
    let mut move_count = 0;
    while !game.is_terminal() {
        move_count += 1;
        println!("Move {}: Player {:?}'s turn", move_count, game.current_player());
        
        // Configure MCTS
        let config = MCTSConfig {
            exploration_constant: 1.414,
            max_iterations: 10000,
            max_simulation_depth: 100,
        };
        
        // Run MCTS
        let start = std::time::Instant::now();
        
        // Run MCTS
        let mcts = MCTSTree::new(game.clone(), config);
        let result = mcts.run();
        
        let elapsed = start.elapsed();
        
        // Print results
        println!("MCTS completed {} iterations in {:?}", result.iterations, elapsed);
        println!("Best action: ({}, {})", result.best_action.row, result.best_action.col);
        println!("Action visits:");
        for (action, visits) in result.action_visits.iter().take(5) {
            println!("  ({}, {}): {} visits", action.row, action.col, visits);
        }
        
        // Apply the best move
        game.make_move(result.best_action);
        println!("\nBoard after move:");
        println!("{}", game);
    }
    
    // Game over
    println!("Game Over!");
    if game.has_won(Player::One) {
        println!("Player X wins!");
    } else if game.has_won(Player::Two) {
        println!("Player O wins!");
    } else {
        println!("It's a draw!");
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_tictactoe_initial_state() {
        let game = TicTacToe::new();
        assert_eq!(game.current_player(), Player::One);
        assert_eq!(game.legal_actions().len(), 9);
        assert!(!game.is_terminal());
    }
    
    #[test]
    fn test_tictactoe_win_detection() {
        let mut game = TicTacToe::new();
        
        // Create a winning position for Player One
        game.board[0][0] = Some(Player::One);
        game.board[0][1] = Some(Player::One);
        game.board[0][2] = Some(Player::One);
        
        assert!(game.has_won(Player::One));
        assert!(!game.has_won(Player::Two));
        assert!(game.is_terminal());
        assert_eq!(game.terminal_value(Player::One), Some(1.0));
        assert_eq!(game.terminal_value(Player::Two), Some(0.0));
    }
    
    #[test]
    fn test_mcts_on_tictactoe() {
        let game = TicTacToe::new();
        let config = MCTSConfig {
            exploration_constant: 1.414,
            max_iterations: 1000,
            max_simulation_depth: 50,
        };
        
        let tree = MCTSTree::new(game, config);
        let result = tree.run();
        
        assert!(result.iterations > 0);
        assert!(result.root_visits > 0);
        assert!(!result.action_visits.is_empty());
    }
}