#pragma once

#include <Graph.hpp>
#include <solver/SolverInterface.hpp>

class WeightedDegree : public SolverInterface
{
public:
    WeightedDegree(const Graph& graph) noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> override;

    auto name() const noexcept
        -> std::string override;


private:
    const Graph& graph_;
    std::vector<float> avg_;
};
