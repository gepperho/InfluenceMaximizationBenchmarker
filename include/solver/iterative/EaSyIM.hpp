#pragma once

#include <Graph.hpp>
#include <solver/SolverInterface.hpp>
#include <unordered_set>

class EaSyIM final : public SolverInterface
{
public:
    EaSyIM(const Graph& graph, int iterations = 3) noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> final;

    auto name() const noexcept
        -> std::string final;


private:
    auto performWeightedDegreeIteration(int iteration) noexcept
        -> void;

    auto calculateWD(NodeId node) const noexcept
        -> float;

    auto resetDegrees(const std::vector<NodeId>& nodes) noexcept
        -> void;

    const Graph& graph_;
    const int iterations_;
    std::vector<float> weighted_degree_even_;
    std::vector<float> weighted_degree_odd_;
    std::vector<float> weighted_degree_0_;
    std::unordered_set<NodeId> selected_nodes_;
};
