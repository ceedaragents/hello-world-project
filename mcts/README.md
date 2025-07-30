# Monte Carlo Tree Search (MCTS) Library

A high-performance, memory-safe Monte Carlo Tree Search implementation in Rust with built-in concurrency support.

## Features

- **Generic Implementation**: Works with any game or decision problem that implements the `GameState` trait
- **Thread-Safe**: Built with concurrency in mind using Arc, RwLock, and Mutex
- **Parallel Execution**: Utilizes Rayon for efficient parallel tree exploration
- **Virtual Loss**: Implements virtual loss for better load balancing in concurrent scenarios
- **Configurable**: Flexible configuration options for exploration, iterations, and parallelism
- **Memory Safe**: Leverages Rust's ownership system for guaranteed memory safety

## Quick Start

Add this to your `Cargo.toml`:

```toml
[dependencies]
mcts = "0.1.0"
```

## Basic Usage

```rust
use mcts::{GameState, Player, MCTSConfig, ConcurrentMCTS};

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
    num_threads: 4,
};

let game = YourGame::new();
let mcts = ConcurrentMCTS::new(game, config);
let result = mcts.run();

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

- `MCTSTree`: Single-threaded MCTS implementation
- `ConcurrentMCTS`: Multi-threaded MCTS implementation with virtual loss
- `Node`: Tree node with thread-safe statistics
- `MCTSConfig`: Configuration parameters

### Configuration Options

- `exploration_constant`: UCB1 exploration parameter (default: √2)
- `max_iterations`: Maximum number of MCTS iterations
- `max_simulation_depth`: Maximum depth for random rollouts
- `num_threads`: Number of worker threads for parallel execution

## Architecture

The library uses a tree structure where each node represents a game state:

1. **Selection**: Traverse the tree using UCB1 to balance exploration/exploitation
2. **Expansion**: Add new children to leaf nodes
3. **Simulation**: Random playout from leaf to terminal state
4. **Backpropagation**: Update statistics from leaf to root

### Concurrency Design

- Uses `Arc<Node>` for shared ownership
- `RwLock` for children and statistics
- Virtual loss prevents multiple threads from exploring the same path
- Lock-free where possible, minimal critical sections

## Performance

The concurrent implementation provides significant speedup:

- 2-4x faster with 4 threads (depending on game complexity)
- Scales well up to 8-16 threads
- Virtual loss ensures good load balancing

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