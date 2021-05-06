#pragma once

#include "solver/SolverInterface.hpp"
#include <Graph.hpp>

class Random final : public SolverInterface
{
public:
    Random(const Graph& graph) noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> override;

    auto name() const noexcept
        -> std::string override;

private:
    const Graph& graph_;
    std::vector<int> degree_;
};
