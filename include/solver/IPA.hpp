#pragma once

#include "SolverInterface.hpp"
#include <Graph.hpp>
#include <unordered_set>

struct path_element
{
    NodeId element;
    std::vector<path_element> children;
    double probability;
    path_element* parent;

    path_element(NodeId element)
    {
        this->element = element;
        this->probability = 1.0;
    }

    auto checkForCircle(NodeId node) const -> bool
    {
        if(node == element) {
            return true;
        }
        if(element == parent->element) {
            return false;
        }
        return parent->checkForCircle(node);
    }
};

class IPASolver final : public SolverInterface
{
public:
    IPASolver(const Graph& graph, int divider = 320) noexcept;

    auto solve(std::size_t k) noexcept
        -> std::vector<NodeId> final;

    auto name() const noexcept
        -> std::string final;

private:
    /**
     * delta function from the IPA paper
     * @param node
     * @return
     */
    auto calculatePathTreeInfluence(path_element& node) noexcept
        -> double;

    /**
     * creates the influence path tree with the starting_node as root
     * @param starting_node root node
     * @param threshold for influence probability
     * @return root_element
     */
    auto buildPathTree(NodeId starting_node, double threshold) noexcept
        -> path_element;

    /**
     * expands a influence path tree element
     * The element is passed as reference (and modified).
     * This method has side-effects!
     * @param current_path
     * @param threshold
     * @param selected_seeds blocker set
     */
    auto expandPathTree(path_element& current_path,
                        double threshold,
                        std::unordered_set<NodeId>& selected_seeds) noexcept
        -> void;

    /**
     * traverses the influence path tree and accumulates the probabilities
     * @param current_element
     * @param probabilities
     */
    auto traversePathTree(const path_element& current_element,
                          std::unordered_map<NodeId, double>& probabilities) noexcept
        -> void;

    /**
     * calculates and sums the influence of all selected_seeds_.
     * If set (use_additional) the given path tree is also added to the forest for the calculation.
     * delta function of the IPA paper for multiple seed nodes.
     * This method has no side-effects.
     *
     * @param additional_node
     * @param use_additional
     * @param threshold
     * @return
     */
    auto calculatePathForestInfluence(NodeId additional_node,
                                      bool use_additional,
                                      double threshold) noexcept
        -> double;

    const Graph& graph_;
    std::unordered_set<NodeId> selected_seeds_;
    std::vector<double> spread_;
    std::unordered_set<NodeId> updated_;
    int divider_;
};
