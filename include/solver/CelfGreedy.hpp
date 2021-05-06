#pragma once

#include "SolverInterface.hpp"
#include <Graph.hpp>


class CelfGreedy : public SolverInterface
{
public:
    CelfGreedy(const Graph& graph, int simulations = 10000) noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> final;

    auto name() const noexcept
        -> std::string final;

private:
    auto evaluateSpread(NodeId node) const noexcept
        -> double;

    auto singleSimulation(NodeId node) const noexcept
        -> long;

    const Graph& graph_;
    std::vector<double> spread_;

    const int simulations_;
};
