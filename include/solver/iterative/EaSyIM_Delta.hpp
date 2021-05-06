#pragma once

#include "Graph.hpp"
#include "solver/SolverInterface.hpp"
class EaSyIM_Delta final : public SolverInterface
{
public:
    EaSyIM_Delta(const Graph& graph, int iterations = 3) noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> final;

    auto name() const noexcept
        -> std::string final;


private:
    auto performWeightedDegreeIteration(const std::vector<NodeId>& nodes,
                                        int iteration) noexcept
        -> void;

    const Graph& graph_;
    const int iterations_;
    std::vector<float> weighted_degree_0_;
    std::vector<float> weighted_degree_1_;
};
