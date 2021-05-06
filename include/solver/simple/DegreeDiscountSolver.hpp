#pragma once

#include "solver/SolverInterface.hpp"
#include <Graph.hpp>

class DegreeDiscountSolver final : public SolverInterface
{
public:
    DegreeDiscountSolver(const Graph& graph) noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> final;

    auto name() const noexcept
        -> std::string final;

private:
    const Graph& graph_;
    std::vector<int> degree_;
};
