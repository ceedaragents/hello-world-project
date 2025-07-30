# Monte Carlo Tree Search (MCTS) Library

A high-performance, memory-safe Monte Carlo Tree Search implementation in Rust.

## Features

- **Generic Implementation**: Works with any game or decision problem that implements the `GameState` trait
- **Configurable**: Flexible configuration options for exploration and iterations
- **Memory Safe**: Leverages Rust's ownership system for guaranteed memory safety
- **Efficient**: Optimized tree traversal and state management

## Quick Start

Add this to your `Cargo.toml`:

```toml
[dependencies]
mcts = "0.1.0"
```

## Basic Usage

```rust
use mcts::{GameState, Player, MCTSConfig, MCTSTree};

// Implement GameState for your game
impl GameState for YourGame {
    type Action = YourAction;
    
    fn current_player(&self) -> Player { /* ... */ }
    fn legal_actions(&self) -> Vec<Self::Action> { /* ... */ }
    fn apply_action(&self, action: &Self::Action) -> Self { /* ... */ }
    fn is_terminal(&self) -> bool { /* ... */ }
    fn terminal_value(&self, player: Player) -> Option<f64> { /* ... */ }
}

// Configure and run MCTS
let config = MCTSConfig {
    exploration_constant: 1.414,
    max_iterations: 10000,
    max_simulation_depth: 100,
};

let game = YourGame::new();
let tree = MCTSTree::new(game, config);
let result = tree.run();

println!("Best action: {:?}", result.best_action);
```

## Examples

### Tic-Tac-Toe

Run the included Tic-Tac-Toe example:

```bash
cargo run --example tictactoe
```

## API Overview

### Core Traits

- `GameState`: Define your game's rules and state transitions
- `Player`: Represents players (One or Two)
- `SimulationPolicy`: Customize simulation behavior (default: random)

### Main Components

- `MCTSTree`: MCTS implementation
- `Node`: Tree node with statistics
- `MCTSConfig`: Configuration parameters

### Configuration Options

- `exploration_constant`: UCB1 exploration parameter (default: √2)
- `max_iterations`: Maximum number of MCTS iterations
- `max_simulation_depth`: Maximum depth for random rollouts

## Architecture

The library uses a tree structure where each node represents a game state:

1. **Selection**: Traverse the tree using UCB1 to balance exploration/exploitation
2. **Expansion**: Add new children to leaf nodes
3. **Simulation**: Random playout from leaf to terminal state
4. **Backpropagation**: Update statistics from leaf to root

## Testing

Run tests with:

```bash
cargo test
```

Run benchmarks with:

```bash
cargo bench
```

## License

This project is licensed under either of

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or http://www.apache.org/licenses/LICENSE-2.0)
- MIT license ([LICENSE-MIT](LICENSE-MIT) or http://opensource.org/licenses/MIT)

at your option.