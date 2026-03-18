# hft-network-optimization

## Summary

### Paper Title

Balancing Cost, Latency, and Capacity: Network Optimisation for High-Frequency Trading

### Abstract

This work studies a High-Frequency Trading (HFT) Network Design Problem, where a trading firm must decide which cross-connects to lease between its servers and colocated exchanges in order to route orders with minimal latency, therefore maximising returns. Orders may be sent either directly to an exchange or indirectly via other exchanges, subject to latency-dependent profit and rate-limit constraints on each link. We formulate this task as a Fixed-Charge Shortest Path Problem (FCSPP) with separable commodity flows, which is NP-hard due to the combinatorial decision of leasing edges with fixed costs. To address the resulting computational challenges, we develop a suite of exact and metaheuristic approaches, including exhaustive search, simulated annealing, and genetic algorithms, combined with efficient shortest-path flow assignment. We utilise OpenMP to allow for scalable exploration of the solution space through the use of parallelisation. Alongside this, we formulate the problem as a Mixed-Integer Linear Program (MILP) and solve it using the SCIP and CBC solvers implemented in the Google OR-Tools C++ library. Empirical evaluation demonstrates that these approaches efficiently produce high-quality solutions for large network instances where exact optimisation is computationally intractable.

## Requirements
- C++20 or newer
- CMake 3.28.3

## Installation

```bash
# Check out the repo
> git clone https://github.com/Tarrin376/hft-network-optimization.git
# Go to the root directory
> cd hft-network-optimization
# Create build directory
> mkdir build
# Create build files using 'release' mode and optimisation flags
> cmake -S . -B build -DBUILD_DEPS:BOOL=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-Wall -O3"
# Build the repo.
> cmake --build build -j<num_processes>
```

## Running tests using Google Test
```bash
# Check out the build directory
> cd build
# Run the 'unit_tests' executable to run tests
> ./unit_tests
```

## Running benchmarks using Google Benchmark
```bash
# Check out the build directory
> cd build
# Run the 'bench' executable to run benchmarks
> ./bench
```