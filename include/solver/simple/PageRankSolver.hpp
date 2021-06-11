#pragma once

#include "solver/SolverInterface.hpp"
#include <Graph.hpp>

/**
 * Modified PageRank, that calculates the pr, assuming the rank is forwarded in opposite direction of the influence diffusion.
 * This way, nodes with a high information diffusion have a high PageRank.
 */
class PageRankSolver final : public SolverInterface
{
public:
    PageRankSolver(const Graph& graph) noexcept;

    PageRankSolver(const Graph& graph, int iterations) noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> final;

    auto name() const noexcept
        -> std::string final;

private:
    /**
     * runs a single page rank iteration
     */
    auto pageRankIteration() noexcept
        -> void;

    auto getKMaxNodes(std::size_t k) const noexcept
        -> std::vector<NodeId>;

    /**
     * runs pageRankIteration until the delta is smaller than 0.001
     */
    auto pageRankConverge() noexcept
        -> void;


    const Graph& graph_;
    std::vector<float> page_rank_;
    std::vector<float> temp_rank_;
    int iterations_ = 0;
};
