#include <Graph.hpp>
#include <execution>
#include <solver/simple/DegreeDiscountSolver.hpp>

DegreeDiscountSolver::DegreeDiscountSolver(const Graph& graph) noexcept
    : graph_(graph),
      degree_(graph_.getNumberOfNodes()) {}

auto DegreeDiscountSolver::solve(const std::size_t k) noexcept
    -> std::vector<NodeId>
{
    auto nodes = utils::range(graph_.getNumberOfNodes());

    std::vector<NodeId> seeds(k);

    std::for_each(std::execution::par,
                  nodes.begin(),
                  nodes.end(),
                  [&](auto vertex) {
                      degree_[vertex] = graph_.getOutDegreeOf(vertex);
                  });

    for(std::size_t i{0}; i < k; i++) {
        auto max_iter =
            std::max_element(std::execution::par,
                             std::cbegin(nodes),
                             std::cend(nodes),
                             [&](auto lhs, auto rhs) {
                                 return degree_[lhs]
                                     < degree_[rhs];
                             });
        seeds.emplace_back(*max_iter);
        degree_[*max_iter] = -1;
        for(auto neighbor : graph_.getInverseEdgesOf(*max_iter)) {
            --degree_[neighbor.getDestination()];
        }
    }

    return seeds;
}

auto DegreeDiscountSolver::name() const noexcept
    -> std::string
{
    return "degree discount";
}
