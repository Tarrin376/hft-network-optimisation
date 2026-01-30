# hft-network-optimization

## Summary

### Paper Title

Balancing Cost, Latency, and Capacity: Network Optimisation for High-Frequency Trading

### Abstract

This work studies a network design problem in high-frequency trading (HFT),
where a trading firm must decide which cross-connects to lease between its
servers and colocated exchanges in order to route orders with minimal latency
and maximal expected profit. Orders may be sent either directly to an ex-
change or indirectly via other exchanges, subject to latency-dependent profit
and rate-limit constraints on each link. We formulate this task as a multi-
commodity capacitated fixed-charge network flow problem (MCND), which is
NP-hard due to the combinatorial decision of leasing edges with fixed costs. To
address the resulting computational challenges, we develop a suite of heuristic
and metaheuristic algorithms such as greedy improvement methods, simulated
annealing, and genetic algorithms, combined with fast shortest-path based flow
assignment. We implement both single-threaded and OpenMP-parallel versions
to enable scalable exploration of the solution space. Alongside this, we formu-
late the problem as a mixed-integer linear program (MILP) and solve it using
the Gurobi MILP solver. Empirical evaluation demonstrates that these ap-
proaches efficiently produce high-quality solutions for large network instances
where exact optimisation is computationally intractable.

## Requirements
- C++20 or newer
- CMake 3.28.3

## Installation

```bash
# Check out the repo
> git clone https://github.com/Tarrin376/hft-network-optimization.git
# Go to the root directory
> cd hft-network-optimization
# Create build files using cmake
> cmake -B build
# Build the repo. **optional but recommended: Include -j<num_processes> to allocate more processor cores to speed up build times.
> cmake --build build
```

## Running tests using the Catch2 library
```bash
# Check out the build directory
> cd build
# Run the 'my_tests' executable to run tests
> ./my_tests
```

## Running benchmarks using the Google Benchmark library
```bash
# Check out the build directory
> cd build
# Run the 'my_bench' executable to run benchmarks
> ./my_bench
```