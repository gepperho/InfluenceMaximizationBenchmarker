#pragma once

#include <Graph.hpp>
#include <dSFMT.h>
#include <diffusion/DiffusionModelInterface.hpp>

class IndependentCascader final : public DiffusionModelInterface
{
public:
    IndependentCascader(const Graph& graph) noexcept;


    auto cascadeForwardN(const std::vector<NodeId>& seeds,
                         std::size_t n) const noexcept
        -> double final;


    auto cascadeBackwardN(const std::vector<NodeId>& seeds,
                          std::size_t n) const noexcept
        -> double final;

private:
    auto cascadeForward(std::vector<NodeId> seeds, dsfmt_t dsfmt) const noexcept
        -> std::size_t;

    auto cascadeBackward(std::vector<NodeId> seeds, dsfmt_t dsfmt) const noexcept
        -> std::size_t;

    const Graph& graph_;
};
