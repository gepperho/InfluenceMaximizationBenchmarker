#pragma once

#include "Graph.hpp"
#include "solver/SolverInterface.hpp"
#include <unordered_set>
class UpdateApproximation final : public SolverInterface
{
public:
    UpdateApproximation(const Graph& graph, int iterations = 3) noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> final;

    auto name() const noexcept
        -> std::string final;


private:
    /**
     * performs a single iteration of the score calculation for all nodes.
     * Depending on the iteration number, weighted_degree_odd_ or weighted_degree_even_ will be read/written.
     * For iteration % 2 == 0 weighted_degree_even_ will be used as lookup and weighted_degree_odd_ be written.
     * For iteration % 2 == 1 vice versa.
     * @param iteration define lookup/write vector
     */
    auto performWeightedDegreeIteration(int iteration) noexcept
        -> void;

    /**
     * calculates the weighted degree for the given node
     * @param node
     * @return
     */
    auto calculateWD(NodeId node) const noexcept
        -> float;

    /**
     * sets the scores of seed to 0
     * updates the wd score of the 1 hop neighbors
     * udpates the second last iteration score of 1 and 2 hop neighbors
     * @param seed
     */
    auto forwardUpdate(NodeId seed) noexcept
        -> void;

    /**
     * updates the last iteration score of the given node
     * @param node
     */
    auto backwardUpdate(NodeId node) noexcept
        -> void;

    const Graph& graph_;
    const int iterations_;
    std::vector<float> weighted_degree_even_;
    std::vector<float> weighted_degree_odd_;
    std::vector<float> weighted_degree_0_;
    std::unordered_set<NodeId> updated_;
};
