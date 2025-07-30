# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Changed
- Refactored Fibonacci sequence implementation to use an iterator pattern
  - Created a `Fibonacci` struct that implements the `Iterator` trait
  - Updated `fibonacci()` function to use the iterator internally
  - Updated `fibonacci_sequence()` function to use the iterator
  - Modified `main()` function to demonstrate direct iterator usage
  - Added comprehensive tests for the iterator implementation including:
    - Direct iterator usage tests
    - Iterator method tests (take, nth, skip)
    - Backwards compatibility tests for existing functions