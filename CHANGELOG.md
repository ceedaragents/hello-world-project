# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- **Concurrent Bounded FIFO Queue Implementation** - A thread-safe, bounded First-In-First-Out queue library for C
  - **Core Features:**
    - Thread-safe operations using POSIX threads (pthread)
    - Bounded capacity with configurable size
    - Circular buffer implementation for O(1) enqueue/dequeue operations
    - Multiple operation modes: blocking, non-blocking, and timeout-based
  
  - **API Functions:**
    - `queue_create()` - Create a new queue with specified capacity
    - `queue_destroy()` - Safely destroy queue and free resources
    - `queue_enqueue()` - Add elements (blocking)
    - `queue_dequeue()` - Remove elements (blocking)
    - `queue_try_enqueue()` - Non-blocking enqueue
    - `queue_try_dequeue()` - Non-blocking dequeue
    - `queue_enqueue_timeout()` - Enqueue with configurable timeout
    - `queue_dequeue_timeout()` - Dequeue with configurable timeout
    - Queue state queries: `queue_size()`, `queue_capacity()`, `queue_is_empty()`, `queue_is_full()`
    - Statistics tracking: `queue_get_stats()` for monitoring queue usage
  
  - **Synchronization:**
    - Mutex protection for thread-safe state access
    - Condition variables for efficient blocking on full/empty conditions
    - Support for graceful shutdown with proper cleanup
  
  - **Error Handling:**
    - Comprehensive error code system with descriptive error messages
    - Error codes for: null parameters, memory allocation, full/empty queue, timeouts, system errors
    - Human-readable error descriptions via `queue_error_string()`
  
  - **Performance & Monitoring:**
    - Statistics tracking: total enqueued/dequeued items, maximum size reached
    - Efficient circular buffer to minimize memory allocations
    - Zero-copy design - queue stores pointers to user data
  
  - **Testing:**
    - Comprehensive test suite with multi-threaded producer-consumer scenarios
    - Stress testing with multiple concurrent producers and consumers
    - Unit tests for individual queue operations
    - Example usage demonstrating producer-consumer pattern

### Technical Details
- **Language:** C (C99 compatible)
- **Dependencies:** POSIX threads (pthread)
- **Memory Model:** Dynamic allocation with proper cleanup
- **Thread Safety:** Full thread safety for all public APIs
- **Platform:** POSIX-compliant systems (Linux, macOS, Unix)

### Files Added
- `concurrent_queue.h` - Public API header
- `concurrent_queue.c` - Main implementation
- `concurrent_queue_design.h` - Detailed design specification
- `concurrent_queue_impl.c` - Additional implementation details
- `test_concurrent_queue.c` - Comprehensive test suite
- `example.c` - Usage examples
- `test_single.c` - Single-threaded unit tests