//! Benchmarks for MCTS library

use criterion::{black_box, criterion_group, criterion_main, Criterion, BenchmarkId};
use mcts::{GameState, Player, MCTSConfig, MCTSTree, ConcurrentMCTS};

/// Simple test game for benchmarking
#[derive(Clone)]
struct BenchmarkGame {
    depth: usize,
    max_depth: usize,
    branching_factor: usize,
}

impl BenchmarkGame {
    fn new(max_depth: usize, branching_factor: usize) -> Self {
        Self {
            depth: 0,
            max_depth,
            branching_factor,
        }
    }
}

impl GameState for BenchmarkGame {
    type Action = usize;
    
    fn current_player(&self) -> Player {
        if self.depth % 2 == 0 {
            Player::One
        } else {
            Player::Two
        }
    }
    
    fn legal_actions(&self) -> Vec<Self::Action> {
        if self.is_terminal() {
            vec![]
        } else {
            (0..self.branching_factor).collect()
        }
    }
    
    fn apply_action(&self, _action: &Self::Action) -> Self {
        Self {
            depth: self.depth + 1,
            max_depth: self.max_depth,
            branching_factor: self.branching_factor,
        }
    }
    
    fn is_terminal(&self) -> bool {
        self.depth >= self.max_depth
    }
    
    fn terminal_value(&self, player: Player) -> Option<f64> {
        if self.is_terminal() {
            // Pseudo-random but deterministic value
            let value = ((self.depth * 7 + 13) % 100) as f64 / 100.0;
            Some(if player == Player::One { value } else { 1.0 - value })
        } else {
            None
        }
    }
}

fn benchmark_single_threaded(c: &mut Criterion) {
    let mut group = c.benchmark_group("single_threaded");
    
    for iterations in [100, 1000, 5000].iter() {
        group.bench_with_input(
            BenchmarkId::from_parameter(iterations),
            iterations,
            |b, &iterations| {
                let game = BenchmarkGame::new(10, 5);
                let config = MCTSConfig {
                    exploration_constant: 1.414,
                    max_iterations: iterations,
                    max_simulation_depth: 20,
                    num_threads: 1,
                };
                
                b.iter(|| {
                    let tree = MCTSTree::new(game.clone(), config.clone());
                    black_box(tree.run())
                });
            },
        );
    }
    
    group.finish();
}

fn benchmark_concurrent(c: &mut Criterion) {
    let mut group = c.benchmark_group("concurrent");
    
    for threads in [1, 2, 4, 8].iter() {
        group.bench_with_input(
            BenchmarkId::from_parameter(threads),
            threads,
            |b, &threads| {
                let game = BenchmarkGame::new(10, 5);
                let config = MCTSConfig {
                    exploration_constant: 1.414,
                    max_iterations: 5000,
                    max_simulation_depth: 20,
                    num_threads: threads,
                };
                
                b.iter(|| {
                    let mcts = ConcurrentMCTS::new(game.clone(), config.clone());
                    black_box(mcts.run())
                });
            },
        );
    }
    
    group.finish();
}

fn benchmark_tree_depth(c: &mut Criterion) {
    let mut group = c.benchmark_group("tree_depth");
    
    for depth in [5, 10, 15, 20].iter() {
        group.bench_with_input(
            BenchmarkId::from_parameter(depth),
            depth,
            |b, &depth| {
                let game = BenchmarkGame::new(depth, 3);
                let config = MCTSConfig {
                    exploration_constant: 1.414,
                    max_iterations: 1000,
                    max_simulation_depth: depth * 2,
                    num_threads: 4,
                };
                
                b.iter(|| {
                    let mcts = ConcurrentMCTS::new(game.clone(), config.clone());
                    black_box(mcts.run())
                });
            },
        );
    }
    
    group.finish();
}

fn benchmark_branching_factor(c: &mut Criterion) {
    let mut group = c.benchmark_group("branching_factor");
    
    for branching in [2, 5, 10, 20].iter() {
        group.bench_with_input(
            BenchmarkId::from_parameter(branching),
            branching,
            |b, &branching| {
                let game = BenchmarkGame::new(8, branching);
                let config = MCTSConfig {
                    exploration_constant: 1.414,
                    max_iterations: 1000,
                    max_simulation_depth: 20,
                    num_threads: 4,
                };
                
                b.iter(|| {
                    let mcts = ConcurrentMCTS::new(game.clone(), config.clone());
                    black_box(mcts.run())
                });
            },
        );
    }
    
    group.finish();
}

criterion_group!(
    benches,
    benchmark_single_threaded,
    benchmark_concurrent,
    benchmark_tree_depth,
    benchmark_branching_factor
);
criterion_main!(benches);