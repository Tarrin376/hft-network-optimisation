#include <gtest/gtest.h>
#include <vector>
#include <cstdint>

#include "solvers/ga_solver.h"

#include "types/config.h"
#include "types/expected_requests.h"
#include "types/graph.h"

class GASolverTest : public GASolver, public testing::Test {
protected:
    GASolverTest() : GASolver{ graph, requests, HFT::GAConfig{}, 100 } {}

    double get_random_double(double min, double max) override {
        return m_rand_double_prob;
    }

    void set_random_double_prob(double prob) {
        m_rand_double_prob = prob;
    }

    void TearDown() override {
        set_random_double_prob(0.0);
    }

private:
    double m_rand_double_prob{};
    const HFT::Graph graph{ 1, 1 };
    const HFT::ExpectedRequests requests{};
};

TEST_F(GASolverTest, MutationDoesNothingWhenRandomValueIsHigh) {
    Chromosome c{ 0b1011101101ULL }; 
    uint64_t original_val{ c[0] };

    set_random_double_prob(0.9);
    mutate(c);

    EXPECT_EQ(c[0], original_val);
}

TEST_F(GASolverTest, MutationFlipsAllBitsWhenRandomValueIsLow) {
    Chromosome c{ 0ULL }; 
    mutate(c);

    uint64_t expected{ ~0ULL };
    EXPECT_EQ(c[0], expected);
}

TEST_F(GASolverTest, CrossoverSwapsMiddleSegment) {
    Chromosome p1{ 0, ~0ULL, 0 };
    Chromosome p2{ ~0ULL, 0, ~0ULL };

    Chromosome expected_p1{ 0, 0, 0 };
    Chromosome expected_p2{ ~0ULL, ~0ULL, ~0ULL };

    crossover(p1, p2, 64, 127);

    EXPECT_EQ(p1, expected_p1);
    EXPECT_EQ(p2, expected_p2);
}

TEST_F(GASolverTest, CrossoverSwapsSingleBits) {
    Chromosome p1{ 1 };
    Chromosome p2{ 0 };

    Chromosome expected_p1{ 0 };
    Chromosome expected_p2{ 1 };

    crossover(p1, p2, 0, 0);

    EXPECT_EQ(p1, expected_p1);
    EXPECT_EQ(p2, expected_p2);
}

TEST_F(GASolverTest, CrossoverSwapsAllBits) {
    Chromosome p1{ ~0ULL, ~0ULL, ~0ULL };
    Chromosome p2{ 0, 0, 0 };

    Chromosome expected_p1{ 0, 0, 0 };
    Chromosome expected_p2{ ~0ULL, ~0ULL, ~0ULL };

    crossover(p1, p2, 0, 191);

    EXPECT_EQ(p1, expected_p1);
    EXPECT_EQ(p2, expected_p2);
}

TEST_F(GASolverTest, StochasticUniversalSamplingHandlesPositiveIntegerFitnessValues) {
    std::vector<double> population_fitness{ 6, 2, 2 };

    std::vector<std::size_t> expected_ans{ 0, 0, 0, 0, 1 };
    std::vector<std::size_t> ans{ stochastic_universal_sampling(population_fitness) };

    EXPECT_EQ(ans, expected_ans);
}

TEST_F(GASolverTest, StochasticUniversalSamplingHandlesZeroFitnessValues) {
    std::vector<double> population_fitness{ 6, 0, 4 };

    std::vector<std::size_t> expected_ans{ 0, 0, 0, 0, 2 };
    std::vector<std::size_t> ans{ stochastic_universal_sampling(population_fitness) };

    EXPECT_EQ(ans, expected_ans);
}

TEST_F(GASolverTest, StochasticUniversalSamplingHandlesDecimalFitnessValues) {
    std::vector<double> population_fitness{ 4.2, 3.6, 2.2 };

    std::vector<std::size_t> expected_ans{ 0, 0, 0, 1, 2 };
    std::vector<std::size_t> ans{ stochastic_universal_sampling(population_fitness) };

    EXPECT_EQ(ans, expected_ans);
}