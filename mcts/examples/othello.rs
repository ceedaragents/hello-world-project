//! Othello (Reversi) example using MCTS
//!
//! This example demonstrates how to use the MCTS library to play Othello.
//! The AI alternates between concurrent and single-threaded MCTS to showcase both implementations.

use mcts::{ConcurrentMCTS, GameState, MCTSConfig, MCTSTree, Player};
use std::fmt;

const BOARD_SIZE: usize = 8;
const DIRECTIONS: [(i32, i32); 8] = [
    (-1, -1), (-1, 0), (-1, 1),
    (0, -1),           (0, 1),
    (1, -1),  (1, 0),  (1, 1),
];

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Position {
    row: usize,
    col: usize,
}

impl Position {
    pub fn new(row: usize, col: usize) -> Self {
        assert!(row < BOARD_SIZE && col < BOARD_SIZE);
        Self { row, col }
    }
}

#[derive(Clone)]
pub struct Othello {
    board: [[Option<Player>; BOARD_SIZE]; BOARD_SIZE],
    current_player: Player,
}

impl Othello {
    pub fn new() -> Self {
        let mut board = [[None; BOARD_SIZE]; BOARD_SIZE];
        board[3][3] = Some(Player::Two);
        board[3][4] = Some(Player::One);
        board[4][3] = Some(Player::One);
        board[4][4] = Some(Player::Two);

        Self {
            board,
            current_player: Player::One,
        }
    }

    fn get_directions(&self, pos: Position, player: Player) -> Vec<Vec<Position>> {
        let mut valid_directions = Vec::new();

        for &(dr, dc) in &DIRECTIONS {
            let mut positions_to_flip = Vec::new();
            let mut r = pos.row as i32 + dr;
            let mut c = pos.col as i32 + dc;
            let mut found_opponent = false;

            while r >= 0 && r < BOARD_SIZE as i32 && c >= 0 && c < BOARD_SIZE as i32 {
                match self.board[r as usize][c as usize] {
                    Some(p) if p != player => {
                        found_opponent = true;
                        positions_to_flip.push(Position::new(r as usize, c as usize));
                    }
                    Some(p) if p == player && found_opponent => {
                        valid_directions.push(positions_to_flip);
                        break;
                    }
                    _ => break,
                }
                r += dr;
                c += dc;
            }
        }

        valid_directions
    }

    fn is_valid_move(&self, pos: Position, player: Player) -> bool {
        if self.board[pos.row][pos.col].is_some() {
            return false;
        }
        !self.get_directions(pos, player).is_empty()
    }

    fn apply_move(&self, pos: Position) -> Self {
        let mut new_state = self.clone();
        let directions = new_state.get_directions(pos, new_state.current_player);
        
        new_state.board[pos.row][pos.col] = Some(new_state.current_player);
        
        for direction in directions {
            for flip_pos in direction {
                new_state.board[flip_pos.row][flip_pos.col] = Some(new_state.current_player);
            }
        }
        
        new_state.current_player = new_state.current_player.opponent();
        new_state
    }

    fn make_move(&mut self, pos: Position) {
        let new_state = self.apply_move(pos);
        *self = new_state;
    }

    fn count_pieces(&self) -> (u32, u32) {
        let mut black = 0;
        let mut white = 0;
        
        for row in &self.board {
            for cell in row {
                match cell {
                    Some(Player::One) => black += 1,
                    Some(Player::Two) => white += 1,
                    None => {}
                }
            }
        }
        
        (black, white)
    }

    fn is_full(&self) -> bool {
        self.board.iter().all(|row| row.iter().all(|cell| cell.is_some()))
    }

    fn has_legal_moves(&self, player: Player) -> bool {
        for row in 0..BOARD_SIZE {
            for col in 0..BOARD_SIZE {
                if self.is_valid_move(Position::new(row, col), player) {
                    return true;
                }
            }
        }
        false
    }

    fn pass_turn(&mut self) {
        self.current_player = self.current_player.opponent();
    }
}

impl GameState for Othello {
    type Action = Position;

    fn current_player(&self) -> Player {
        self.current_player
    }

    fn legal_actions(&self) -> Vec<Self::Action> {
        let mut actions = Vec::new();
        
        for row in 0..BOARD_SIZE {
            for col in 0..BOARD_SIZE {
                let pos = Position::new(row, col);
                if self.is_valid_move(pos, self.current_player) {
                    actions.push(pos);
                }
            }
        }
        
        actions
    }

    fn apply_action(&self, action: &Self::Action) -> Self {
        self.apply_move(*action)
    }

    fn is_terminal(&self) -> bool {
        self.is_full() || 
        (!self.has_legal_moves(Player::One) && !self.has_legal_moves(Player::Two))
    }

    fn terminal_value(&self, player: Player) -> Option<f64> {
        if !self.is_terminal() {
            return None;
        }

        let (black_count, white_count) = self.count_pieces();
        
        let value = match player {
            Player::One => {
                if black_count > white_count {
                    1.0
                } else if black_count < white_count {
                    0.0
                } else {
                    0.5
                }
            }
            Player::Two => {
                if white_count > black_count {
                    1.0
                } else if white_count < black_count {
                    0.0
                } else {
                    0.5
                }
            }
        };
        
        Some(value)
    }
}

impl fmt::Display for Othello {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        writeln!(f, "  a b c d e f g h")?;
        writeln!(f, " ┌─────────────────┐")?;
        
        for (row_idx, row) in self.board.iter().enumerate() {
            write!(f, "{}│ ", row_idx + 1)?;
            for cell in row {
                let symbol = match cell {
                    Some(Player::One) => "● ",
                    Some(Player::Two) => "○ ",
                    None => "· ",
                };
                write!(f, "{}", symbol)?;
            }
            writeln!(f, "│")?;
        }
        
        writeln!(f, " └─────────────────┘")?;
        
        let (black, white) = self.count_pieces();
        writeln!(f, " Black (●): {}  White (○): {}", black, white)?;
        writeln!(f, " Current player: {}", 
            match self.current_player {
                Player::One => "Black (●)",
                Player::Two => "White (○)",
            }
        )?;
        
        Ok(())
    }
}

fn main() {
    println!("=== Othello with MCTS ===\n");
    
    let mut game = Othello::new();
    let mut move_count = 0;
    
    let config = MCTSConfig {
        exploration_constant: 1.414,
        max_iterations: 10000,
        max_simulation_depth: 200,
        num_threads: 4,
    };
    
    println!("Initial board:");
    println!("{}", game);
    
    while !game.is_terminal() {
        move_count += 1;
        println!("Move {}", move_count);
        
        let legal_moves = game.legal_actions();
        if legal_moves.is_empty() {
            println!("{} must pass - no legal moves available", 
                match game.current_player() {
                    Player::One => "Black",
                    Player::Two => "White",
                }
            );
            game.pass_turn();
            continue;
        }
        
        let start = std::time::Instant::now();
        
        // Demonstrate both concurrent and single-threaded MCTS
        // Black uses concurrent (4 threads), White uses single-threaded
        let mcts = if move_count % 2 == 1 {
            println!("Black thinking (concurrent MCTS)...");
            ConcurrentMCTS::new(game.clone(), config.clone())
        } else {
            println!("White thinking (single-threaded MCTS)...");
            let single_config = MCTSConfig {
                num_threads: 1,
                ..config.clone()
            };
            ConcurrentMCTS::new(game.clone(), single_config)
        };
        
        let result = mcts.run();
        let elapsed = start.elapsed();
        
        let best_action = result.best_action;
        println!("Best move: {}{}",
            (b'a' + best_action.col as u8) as char,
            best_action.row + 1
        );
        println!("Iterations: {} in {:.2}s ({:.0} iter/s)",
            result.iterations,
            elapsed.as_secs_f64(),
            result.iterations as f64 / elapsed.as_secs_f64()
        );
        
        if result.action_visits.len() > 1 {
            println!("Top moves by visit count:");
            for (action, visits) in result.action_visits.iter().take(3) {
                println!("  {}{}: {} visits",
                    (b'a' + action.col as u8) as char,
                    action.row + 1,
                    visits
                );
            }
        }
        
        game.make_move(best_action);
        println!("\n{}", game);
    }
    
    println!("=== Game Over ===");
    let (black, white) = game.count_pieces();
    if black > white {
        println!("Black wins! {} - {}", black, white);
    } else if white > black {
        println!("White wins! {} - {}", white, black);
    } else {
        println!("It's a draw! {} - {}", black, white);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_initial_board() {
        let game = Othello::new();
        assert_eq!(game.board[3][3], Some(Player::Two));
        assert_eq!(game.board[3][4], Some(Player::One));
        assert_eq!(game.board[4][3], Some(Player::One));
        assert_eq!(game.board[4][4], Some(Player::Two));
        assert_eq!(game.current_player(), Player::One);
    }

    #[test]
    fn test_initial_legal_moves() {
        let game = Othello::new();
        let moves = game.legal_actions();
        assert_eq!(moves.len(), 4);
        assert!(moves.contains(&Position::new(2, 3)));
        assert!(moves.contains(&Position::new(3, 2)));
        assert!(moves.contains(&Position::new(4, 5)));
        assert!(moves.contains(&Position::new(5, 4)));
    }

    #[test]
    fn test_move_application() {
        let game = Othello::new();
        let new_game = game.apply_action(&Position::new(2, 3));
        
        assert_eq!(new_game.board[2][3], Some(Player::One));
        assert_eq!(new_game.board[3][3], Some(Player::One));
        assert_eq!(new_game.current_player(), Player::Two);
    }

    #[test]
    fn test_terminal_detection() {
        let game = Othello::new();
        assert!(!game.is_terminal());
        
        let mut full_board = [[Some(Player::One); 8]; 8];
        full_board[7][7] = Some(Player::Two);
        let terminal_game = Othello {
            board: full_board,
            current_player: Player::One,
        };
        assert!(terminal_game.is_terminal());
    }

    #[test]
    fn test_mcts_integration() {
        let game = Othello::new();
        let config = MCTSConfig {
            exploration_constant: 1.414,
            max_iterations: 100,
            max_simulation_depth: 50,
            num_threads: 1,
        };
        
        let mcts = ConcurrentMCTS::new(game, config);
        let result = mcts.run();
        
        assert!(result.best_action.is_some());
        assert!(result.iterations > 0);
    }
}