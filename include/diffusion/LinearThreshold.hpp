#pragma once

#include <Graph.hpp>
#include <diffusion/DiffusionModelInterface.hpp>
#include <dSFMT.h>

class LinearThresholdEvaluation final : public DiffusionModelInterface
{

public:
    LinearThresholdEvaluation(const Graph& graph) noexcept;


    auto cascadeForwardN(const std::vector<NodeId>& seeds,
                         std::size_t n) const noexcept
        -> double final;

    auto cascadeBackwardN(const std::vector<NodeId>& seeds,
                          std::size_t n) const noexcept
        -> double final;


private:
  auto forwardInfluence(std::vector<NodeId> seeds, dsfmt_t dsfmt) const noexcept
        -> std::size_t;

  auto backwardInfluence(std::vector<NodeId> seeds, dsfmt_t dsfmt) const noexcept
        -> std::size_t;

    const Graph& graph_;
};
