#include <execution>
#include <fmt/ranges.h>
#include <solver/iterative/EaSyIM.hpp>

EaSyIM::EaSyIM(const Graph& graph, int iterations) noexcept
    : graph_(graph),
      iterations_(iterations),
      weighted_degree_even_(graph_.getNumberOfNodes(), 0.0f),
      weighted_degree_odd_(graph_.getNumberOfNodes(), 0.0f),
      weighted_degree_0_(graph_.getNumberOfNodes(), 0.0f)
{}

auto EaSyIM::solve(const std::size_t k) noexcept
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
                  [&](auto current_node) {
                      auto temp = calculateWD(current_node);
                      weighted_degree_odd_[current_node] = temp;
                      weighted_degree_0_[current_node] = temp;
                  });

    std::vector<NodeId> seed_set;

    // perform k EaSyIM calculations and pick the best node every time
    while(seed_set.size() < k) {

        for(int iteration = 2; iteration <= iterations_; ++iteration) {
            performWeightedDegreeIteration(iteration);
        }

        const auto& weighted_degree = iterations_ <= 1
            ? weighted_degree_0_
            : iterations_ % 2 == 0
            ? weighted_degree_even_
            : weighted_degree_odd_;

        auto best = *std::max_element(
            std::begin(nodes),
            std::end(nodes),
            [&](auto lhs, auto rhs) {
                return weighted_degree[lhs] < weighted_degree[rhs];
            });

        seed_set.emplace_back(best);
        selected_nodes_.emplace(best);
        weighted_degree_0_[best] = 0.f;

        resetDegrees(nodes);
    }

    return seed_set;
}

auto EaSyIM::performWeightedDegreeIteration(const int iteration) noexcept
    -> void
{ // calculate average neighbor success

    auto& updated = iteration % 2 == 0 ? weighted_degree_even_ : weighted_degree_odd_;
    const auto& lookup = iteration % 2 == 0 ? weighted_degree_odd_ : weighted_degree_even_;


    std::for_each(std::execution::par,
                  graph_.getNodes().begin(),
                  graph_.getNodes().end(),
                  [&](auto current_node) {
                      if(selected_nodes_.count(current_node) != 0) {
                          return;
                      }

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

                              //ignore selected nodes
                              if(selected_nodes_.count(destination) != 0) {
                                  return 0.0f;
                              }

                              return edge.getWeight() * lookup[destination];
                          });
                  });
}

auto EaSyIM::name() const noexcept
    -> std::string
{
    return "EaSyIM," + std::to_string(iterations_);
}


auto EaSyIM::calculateWD(const NodeId node) const noexcept
    -> float
{
    return std::transform_reduce(
        graph_.getEdgesOf(node).begin(),
        graph_.getEdgesOf(node).end(),
        0.0f,
        std::plus<>(),
        [](Edge current_edge) {
            return current_edge.getWeight();
        });
}

auto EaSyIM::resetDegrees(const std::vector<NodeId>& nodes) noexcept
    -> void
{
    std::for_each(std::execution::par,
                  std::begin(nodes),
                  std::end(nodes),
                  [&](auto current_node) {
                      if(selected_nodes_.count(current_node) != 0) {
                          weighted_degree_odd_[current_node] = 0.0f;
                      } else {
                          auto degree = calculateWD(current_node);
                          weighted_degree_odd_[current_node] = degree;
                      }
                  });

    std::fill(std::begin(weighted_degree_even_),
              std::end(weighted_degree_even_),
              0.0f);
}
