cmake_minimum_required(VERSION 3.28.3)
project(HFT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Catch2
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.12.0
)
FetchContent_MakeAvailable(Catch2)

# Google Benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.5
)
FetchContent_MakeAvailable(benchmark)

# App executable
add_executable(hft src/main.cpp)
target_link_libraries(hft PRIVATE)

# Unit tests with Catch2
add_executable(my_tests tests/test_main.cpp)
target_link_libraries(my_tests PRIVATE Catch2::Catch2WithMain)

enable_testing()
add_test(NAME my_tests COMMAND my_tests)

# Benchmarks with Google Benchmark
add_executable(my_bench benchmarks/bench_main.cpp)
target_link_libraries(my_bench PRIVATE benchmark::benchmark)