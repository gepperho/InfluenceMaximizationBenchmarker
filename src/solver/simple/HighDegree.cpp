#include "solver/simple/HighDegree.hpp"
#include <execution>

HighDegree::HighDegree(const Graph& graph) noexcept
    : graph_(graph),
      degree_(graph.getNumberOfNodes())
{}

auto HighDegree::solve(std::size_t k) noexcept
  -> std::vector<NodeId>
{
    std::vector<NodeId> nodes(graph_.getNumberOfNodes());
    std::iota(std::begin(nodes),
              std::end(nodes),
              0);


    std::for_each(std::execution::par,
                  nodes.begin(),
                  nodes.end(),
                  [&](auto vertex) {
                      degree_[vertex] = graph_.getOutDegreeOf(vertex);
                  });


    // find k best candidates
    auto end_iter = k > nodes.size()
        ? std::end(nodes)
        : std::begin(nodes) + k;

    std::partial_sort(std::begin(nodes),
                      end_iter,
                      std::end(nodes),
                      [&](auto lhs, auto rhs) {
                          return degree_[lhs] > degree_[rhs];
                      });

    return std::vector(std::begin(nodes),
                       end_iter);
}
auto HighDegree::name() const noexcept
  -> std::string
{
    return "HighDegree";
}
