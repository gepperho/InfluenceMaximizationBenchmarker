#include "solver/WeightedDegree.hpp"
#include <execution>

WeightedDegree::WeightedDegree(const Graph& graph) noexcept
    : graph_(graph),
      avg_(graph.getNumberOfNodes(), 0.0f)
{}

auto WeightedDegree::solve(const std::size_t k) noexcept
    -> std::vector<NodeId>
{

    std::vector<NodeId> nodes(graph_.getNumberOfNodes());
    std::iota(std::begin(nodes),
              std::end(nodes),
              0);

    // calculate average neighbor success
    std::for_each(
        std::execution::par,
        std::begin(nodes),
        std::end(nodes),
        [&](auto current_node) {
            const auto& edges = graph_.getEdgesOf(current_node);
            avg_[current_node] = std::transform_reduce(
                std::execution::unseq,
                std::begin(edges),
                std::end(edges),
                0.0f,
                [](auto init, auto new_value) {
                    return init + new_value;
                },
                [](const auto& edge) {
                    return edge.getWeight();
                });
        });

    // find k best candidates
    auto end_iter = k > nodes.size()
        ? std::end(nodes)
        : std::begin(nodes) + k;

    std::partial_sort(std::begin(nodes),
                      end_iter,
                      std::end(nodes),
                      [&](auto lhs, auto rhs) {
                          return avg_[lhs] > avg_[rhs];
                      });

    return std::vector(std::begin(nodes),
                       end_iter);
}

auto WeightedDegree::name() const noexcept
    -> std::string
{
    return "WD";
}
