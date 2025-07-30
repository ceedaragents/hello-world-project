# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Initial implementation of Monte Carlo Tree Search (MCTS) library in Rust
- Memory-safe implementation using Rust's ownership system
- Concurrent MCTS with thread pool support using Rayon
- Virtual loss implementation for better load balancing
- Generic trait-based design supporting any two-player game
- UCB1 (Upper Confidence Bound) selection strategy
- Configurable exploration constant and iteration limits
- Thread-safe node structure using Arc, RwLock, and Mutex
- Example implementation: Tic-Tac-Toe
- Comprehensive test suite including concurrency tests
- Performance benchmarks comparing single vs multi-threaded execution