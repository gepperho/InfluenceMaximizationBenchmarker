#pragma once

#include "solver/SolverInterface.hpp"
#include <Graph.hpp>

/**
 * Modified PageRank, that calculates the pr, assuming the rank is forwarded in opposite direction of the influence diffusion.
 * This way, nodes with a high information diffusion have a high PageRank.
 */
class WeightedPageRank final : public SolverInterface
{
public:
    WeightedPageRank(const Graph& graph) noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> final;

    auto name() const noexcept
        -> std::string final;

private:
    /**
     * runs 'iterations' number of page rank calculations, using the power method
     * @param number of iterations
     */
    auto weightedPageRankIterations(int iterations) noexcept
        -> void;

    /**
     * runs PageRank iterations until the delta between two iterations is at most 0.0001
     */
    auto weightedPageRankDelta() noexcept
        -> void;

    /**
     * runs a single PageRAnk iteration
     */
    auto runSingleWeightedPageRankIteration() noexcept
        -> void;

    auto precalculateWeightedSums() noexcept
        -> void;

    auto getKMaxNodes(std::size_t k) const noexcept
        -> std::vector<NodeId>;

    const Graph& graph_;
    std::vector<float> page_rank_;
    std::vector<float> temp_page_rank_;
    std::vector<float> probability_sum_;
};
