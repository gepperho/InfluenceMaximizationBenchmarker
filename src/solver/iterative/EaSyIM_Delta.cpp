#include <execution>
#include <fmt/core.h>
#include <numeric>
#include <solver/iterative/EaSyIM_Delta.hpp>

EaSyIM_Delta::EaSyIM_Delta(const Graph& graph, int iterations) noexcept
    : graph_(graph),
      iterations_(iterations),
      weighted_degree_0_(graph_.getNumberOfNodes(), 0.0f),
      weighted_degree_1_(graph_.getNumberOfNodes(), 0.0f)

{}

auto EaSyIM_Delta::solve(const std::size_t k) noexcept
    -> std::vector<NodeId>
{

    std::vector<NodeId> nodes(graph_.getNumberOfNodes());
    std::iota(std::begin(nodes),
              std::end(nodes),
              0);

    // initial wd calculation (1 hop)
    std::for_each(std::execution::par,
                  std::begin(nodes),
                  std::end(nodes),
                  [&](auto& current_node) {
                      weighted_degree_0_[current_node] = std::transform_reduce(
                          std::execution::unseq,
                          std::begin(graph_.getEdgesOf(current_node)),
                          std::end(graph_.getEdgesOf(current_node)),
                          0.0f,
                          std::plus<>(),
                          [&](Edge edge) {
                              return edge.getWeight();
                          });
                  });

    for(int iteration = 1; iteration < iterations_; ++iteration) {
        performWeightedDegreeIteration(nodes, iteration);
    }

    const auto& weighted_degree = [&] {
        if(iterations_ % 2 == 0 && iterations_ > 1) {
            return weighted_degree_1_;
        }
        return weighted_degree_0_;
    }();

    // find k candidates, loop every possible node
    auto end_iter = k > nodes.size()
        ? std::end(nodes)
        : std::begin(nodes) + k;

    std::partial_sort(std::begin(nodes),
                      end_iter,
                      std::end(nodes),
                      [&](auto lhs, auto rhs) {
                          return weighted_degree[lhs] > weighted_degree[rhs];
                      });
    return std::vector(std::begin(nodes),
                       end_iter);
}

auto EaSyIM_Delta::performWeightedDegreeIteration(const std::vector<NodeId>& nodes,
                                                  const int iteration) noexcept
    -> void
{ // calculate average neighbor success

    auto& updated = iteration % 2 == 0 ? weighted_degree_0_ : weighted_degree_1_;
    const auto& lookup = iteration % 2 == 0 ? weighted_degree_1_ : weighted_degree_0_;


    std::for_each(std::execution::par,
                  nodes.begin(),
                  nodes.end(),
                  [&](auto current_node) {
                      // accumulate neighbor scores from the previous round
                      updated[current_node] = std::transform_reduce(
                          std::execution::unseq,
                          std::begin(graph_.getEdgesOf(current_node)),
                          std::end(graph_.getEdgesOf(current_node)),
                          lookup[current_node],
                          std::plus<>(),
                          [&](auto edge) {
                              // reduce neighbor score by the edge weight
                              auto destination = edge.getDestination();
                              return edge.getWeight() * lookup[destination];
                          });
                  });
}
auto EaSyIM_Delta::name() const noexcept
    -> std::string
{
    return "EaSyIM-Delta," + std::to_string(iterations_);
}
