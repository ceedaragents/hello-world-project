# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed
- **BREAKING**: Removed all concurrency support to simplify the library
- **BREAKING**: Removed `ConcurrentMCTS` struct
- **BREAKING**: Removed `num_threads` field from `MCTSConfig`
- **BREAKING**: Removed dependencies on `rayon` and `parking_lot`
- Replaced thread-safe `RwLock` and `Mutex` with single-threaded `RefCell` and `Cell`
- Simplified benchmarks to focus on single-threaded performance
- Updated documentation to reflect single-threaded design

### Removed
- `ConcurrentMCTS` implementation
- Virtual loss mechanism (no longer needed without concurrency)
- Thread pool configuration
- Concurrent benchmarks and tests
- Dependencies: `rayon` and `parking_lot`

### Added
- Initial implementation of Monte Carlo Tree Search (MCTS) library in Rust
- Memory-safe implementation using Rust's ownership system
- Generic trait-based design supporting any two-player game
- UCB1 (Upper Confidence Bound) selection strategy
- Configurable exploration constant and iteration limits
- Example implementation: Tic-Tac-Toe
- Comprehensive test suite
- Performance benchmarks