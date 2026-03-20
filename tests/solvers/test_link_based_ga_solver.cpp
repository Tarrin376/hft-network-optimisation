#include <vector>
#include <cstdint>

#include <gtest/gtest.h>

#include "solvers/link_based_ga_solver.h"
#include "types/config.h"
#include "types/expected_requests.h"
#include "types/graph.h"

class LinkBasedGASolverTest : public LinkBasedGASolver, public testing::Test {
protected:
    LinkBasedGASolverTest() : LinkBasedGASolver{ graph, requests, HFT::GAConfig{}, 100, false } {}

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

TEST_F(LinkBasedGASolverTest, MutationDoesNothingWhenRandomValueIsHigh) {
    Chromosome c{ 0b1011101101ULL }; 
    std::uint64_t original_val{ c[0] };

    set_random_double_prob(0.9);
    mutate(c);

    EXPECT_EQ(c[0], original_val);
}

TEST_F(LinkBasedGASolverTest, MutationFlipsAllBitsWhenRandomValueIsLow) {
    Chromosome c{ 0ULL }; 
    mutate(c);

    std::uint64_t expected{ ~0ULL };
    EXPECT_EQ(c[0], expected);
}

TEST_F(LinkBasedGASolverTest, CrossoverSwapsMiddleSegment) {
    Chromosome p1{ 0, ~0ULL, 0 };
    Chromosome p2{ ~0ULL, 0, ~0ULL };

    Chromosome expected_p1{ 0, 0, 0 };
    Chromosome expected_p2{ ~0ULL, ~0ULL, ~0ULL };

    crossover(p1, p2);

    EXPECT_EQ(p1, expected_p1);
    EXPECT_EQ(p2, expected_p2);
}

TEST_F(LinkBasedGASolverTest, CrossoverSwapsSingleBits) {
    Chromosome p1{ 1 };
    Chromosome p2{ 0 };

    Chromosome expected_p1{ 0 };
    Chromosome expected_p2{ 1 };

    crossover(p1, p2);

    EXPECT_EQ(p1, expected_p1);
    EXPECT_EQ(p2, expected_p2);
}

TEST_F(LinkBasedGASolverTest, CrossoverSwapsAllBits) {
    Chromosome p1{ ~0ULL, ~0ULL, ~0ULL };
    Chromosome p2{ 0, 0, 0 };

    Chromosome expected_p1{ 0, 0, 0 };
    Chromosome expected_p2{ ~0ULL, ~0ULL, ~0ULL };

    crossover(p1, p2);

    EXPECT_EQ(p1, expected_p1);
    EXPECT_EQ(p2, expected_p2);
}