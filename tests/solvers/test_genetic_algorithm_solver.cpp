#include <gtest/gtest.h>
#include <vector>
#include <cstdint>

#include "solvers/genetic_algorithm_solver.h"
#include "types/config.h"

struct MockGenerator {
    using result_type = std::uint64_t;
    
    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

    void set_probability(double val) {
        last_value = static_cast<result_type>(val * static_cast<double>(max()));
    }

    result_type last_value = 0;
    result_type operator()() { return last_value; }
};

class GeneticAlgorithmSolverTest : public GeneticAlgorithmSolver<MockGenerator>, public testing::Test {
protected:
    GeneticAlgorithmSolverTest() 
    : GeneticAlgorithmSolver{ 100, 100, GAConfig { 
        .population_size = 5,
        .generations = 2,
        .mutation_rate = 0.05,
        .crossover_rate = 0.8,
        .initial_bit_flip_rate = 0.01,
        .seed = 34,
    } } {}
};

TEST_F(GeneticAlgorithmSolverTest, MutationDoesNothingWhenRandomValueIsHigh) {
    Chromosome c{ 0b1011101101ULL }; 
    uint64_t original_val{ c[0] };

    m_gen.set_probability(0.9);
    mutate(c);

    EXPECT_EQ(c[0], original_val);
}

TEST_F(GeneticAlgorithmSolverTest, MutationFlipsAllBitsWhenRandomValueIsLow) {
    Chromosome c{ 0ULL }; 

    m_gen.set_probability(0.0); 
    mutate(c);

    uint64_t expected{ 0x7FFFFFFFFFFFFFFFULL };
    EXPECT_EQ(c[0], expected);
}

TEST_F(GeneticAlgorithmSolverTest, CrossoverSwapsMiddleSegment) {
    Chromosome p1{ 0, ~0ULL, 0 };
    Chromosome p2{ ~0ULL, 0, ~0ULL };

    Chromosome expected_p1{ 0, 0, 0 };
    Chromosome expected_p2{ ~0ULL, ~0ULL, ~0ULL };

    crossover(p1, p2, 64, 127);

    EXPECT_EQ(p1, expected_p1);
    EXPECT_EQ(p2, expected_p2);
}

TEST_F(GeneticAlgorithmSolverTest, CrossoverSwapsSingleBits) {
    Chromosome p1{ 1 };
    Chromosome p2{ 0 };

    Chromosome expected_p1{ 0 };
    Chromosome expected_p2{ 1 };

    crossover(p1, p2, 0, 0);

    EXPECT_EQ(p1, expected_p1);
    EXPECT_EQ(p2, expected_p2);
}

TEST_F(GeneticAlgorithmSolverTest, CrossoverSwapsAllBits) {
    Chromosome p1{ ~0ULL, ~0ULL, ~0ULL };
    Chromosome p2{ 0, 0, 0 };

    Chromosome expected_p1{ 0, 0, 0 };
    Chromosome expected_p2{ ~0ULL, ~0ULL, ~0ULL };

    crossover(p1, p2, 0, 191);

    EXPECT_EQ(p1, expected_p1);
    EXPECT_EQ(p2, expected_p2);
}

TEST_F(GeneticAlgorithmSolverTest, StochasticUniversalSamplingHandlesPositiveIntegerFitnessValues) {
    std::vector<double> population_fitness{ 6, 2, 2 };
    m_gen.set_probability(0.0);

    std::vector<std::size_t> expected_ans{ 0, 0, 0, 0, 1 };
    std::vector<std::size_t> ans{ stochastic_universal_sampling(population_fitness) };

    EXPECT_EQ(ans, expected_ans);
}

TEST_F(GeneticAlgorithmSolverTest, StochasticUniversalSamplingHandlesZeroFitnessValues) {
    std::vector<double> population_fitness{ 6, 0, 4 };
    m_gen.set_probability(0.0);

    std::vector<std::size_t> expected_ans{ 0, 0, 0, 0, 2 };
    std::vector<std::size_t> ans{ stochastic_universal_sampling(population_fitness) };

    EXPECT_EQ(ans, expected_ans);
}

TEST_F(GeneticAlgorithmSolverTest, StochasticUniversalSamplingHandlesDecimalFitnessValues) {
    std::vector<double> population_fitness{ 4.2, 3.6, 2.2 };
    m_gen.set_probability(0.0);

    std::vector<std::size_t> expected_ans{ 0, 0, 0, 1, 2 };
    std::vector<std::size_t> ans{ stochastic_universal_sampling(population_fitness) };

    EXPECT_EQ(ans, expected_ans);
}