
#pragma once

#include <Edge.hpp>
#include <string>
#include <vector>

class SolverInterface
{
public:
    virtual auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> = 0;

    virtual auto name() const noexcept
        -> std::string = 0;

    virtual ~SolverInterface() = default;
};
