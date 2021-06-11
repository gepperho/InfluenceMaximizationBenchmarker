
#pragma once

#include <Graph.hpp>
#include <dSFMT.h>
#include <mutex>
#include <random>
#include <solver/SolverInterface.hpp>
#include <unordered_set>

class IMM final : public SolverInterface
{
public:
    IMM(const Graph& graph)
    noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> final;

    auto name() const noexcept
        -> std::string final;

private:
    /**
     * creates and returns a random rr set
     * @return random rr set
     */
    auto createRrSet(dsfmt_t dsfmt) const noexcept
        -> std::vector<NodeId>;


    /**
    * calculates the log2(binomial coefficient(n.k))
    */
    auto logBinom(std::size_t n, std::size_t k) const noexcept
        -> double;

    /**
     * selects the k best nodes according to the previously sampled rr sets
     * @param k number of nodes to be selected
     * @return tuple: (vector of selected nodes, double value: fraction of rr sets that are covered by the selected nodes)
     */
    auto nodeSelection(std::size_t k) const noexcept
        -> std::pair<std::vector<NodeId>, double>;

    const Graph& graph_;
    std::vector<std::vector<NodeId>> rr_sets_;
};
