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

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> final;

    auto name() const noexcept
        -> std::string final;

private:
    /**
     * runs 'iterations' number of page rank calculations, using the power method
     * @param in_edge
     */
    auto pageRankIteration(int iterations) noexcept
        -> void;

    auto getKMaxNodes(std::size_t k) const noexcept
        -> std::vector<NodeId>;

    const Graph& graph_;
    std::vector<float> page_rank_;
};
