
#pragma once

#include <Edge.hpp>
#include <vector>

class DiffusionModelInterface
{
public:
    virtual auto cascadeForwardN(const std::vector<NodeId>& seeds,
                                 std::size_t n) const noexcept
        -> double = 0;


    virtual auto cascadeBackwardN(const std::vector<NodeId>& seeds,
                                  std::size_t n) const noexcept
        -> double = 0;

    virtual ~DiffusionModelInterface() = default;
};
