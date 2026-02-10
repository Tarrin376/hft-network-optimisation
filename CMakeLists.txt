cmake_minimum_required(VERSION 3.28.3)
project(HFT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# HFT Library
file(GLOB_RECURSE LIB_SOURCES src/*.cpp)
list(FILTER LIB_SOURCES EXCLUDE REGEX "src/main.cpp") 

add_library(hft_lib ${LIB_SOURCES})
target_include_directories(hft_lib PUBLIC include)

# Main App
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE hft_lib)

# OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(hft_lib PUBLIC OpenMP::OpenMP_CXX)
endif()

# Google Test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)
add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE GTest::gtest_main hft_lib)

# Google Benchmark
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.1
)
FetchContent_MakeAvailable(benchmark)

file(GLOB_RECURSE BENCH_SOURCES benchmarks/*.cpp)
add_executable(bench ${BENCH_SOURCES})
target_link_libraries(bench PRIVATE benchmark::benchmark hft_lib)