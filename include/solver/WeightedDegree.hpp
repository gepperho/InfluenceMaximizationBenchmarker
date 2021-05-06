#pragma once

#include <solver/SolverInterface.hpp>
#include <Graph.hpp>

class WeightedDegree : public SolverInterface
{
public:
    WeightedDegree(const Graph& graph) noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> override;

    auto name() const noexcept
        -> std::string override;


protected:
    const Graph& graph_;
    std::vector<float> avg_;
};
