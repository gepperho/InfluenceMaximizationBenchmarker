#include "solver/iterative/UpdateApproximation.hpp"
#include <execution>
#include <queue>

UpdateApproximation::UpdateApproximation(const Graph& graph, int iterations) noexcept
    : graph_(graph),
      iterations_(iterations),
      weighted_degree_even_(graph.getNumberOfNodes(), 0.0f),
      weighted_degree_odd_(graph.getNumberOfNodes(), 0.0f),
      weighted_degree_0_(graph.getNumberOfNodes(), 0.0f)
{}

auto UpdateApproximation::solve(const std::size_t k) noexcept
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
                      auto temp = calculateWD(current_node);
                      weighted_degree_odd_[current_node] = temp;
                      weighted_degree_0_[current_node] = temp;
                  });

    for(int iteration = 2; iteration <= iterations_; ++iteration) {
        performWeightedDegreeIteration(iteration);
    }

    auto& weighted_degree = iterations_ <= 1
        ? weighted_degree_0_
        : iterations_ % 2 == 0 ? weighted_degree_even_
                               : weighted_degree_odd_;

    auto seed_comparison =
        [&](const auto& lhs, const auto& rhs) {
            if(weighted_degree[lhs] == weighted_degree[rhs]) {
                return updated_.find(rhs) != updated_.end();
            }
            return weighted_degree[lhs] < weighted_degree[rhs];
        };

    using SeedPriorityQueue = std::priority_queue<NodeId,
                                                  std::vector<NodeId>,
                                                  decltype(seed_comparison)>;

    SeedPriorityQueue pq(seed_comparison, nodes);

    // find k candidates, loop every possible node
    std::vector<NodeId> seed_set;
    seed_set.reserve(k);

    auto top_node = pq.top();
    seed_set.emplace_back(top_node);
    pq.pop();
    forwardUpdate(top_node);

    while(seed_set.size() < k) {
        top_node = pq.top();
        pq.pop();

        if(updated_.find(top_node) != updated_.end()) {
            // current top node is up to date and thus can be added
            seed_set.emplace_back(top_node);
            forwardUpdate(top_node);
            updated_.clear();
            continue;
        }

        // current top node is outdated
        backwardUpdate(top_node);

        if(weighted_degree[top_node] >= weighted_degree[pq.top()]) {
            // add top_node
            seed_set.emplace_back(top_node);
            forwardUpdate(top_node);
            updated_.clear();
        } else {
            updated_.insert(top_node);
            pq.push(top_node);
        }
    }

    return seed_set;
}

auto UpdateApproximation::performWeightedDegreeIteration(const int iteration) noexcept
    -> void
{ // calculate average neighbor success

    auto& updated = iteration % 2 == 0 ? weighted_degree_even_ : weighted_degree_odd_;
    const auto& lookup = iteration % 2 == 0 ? weighted_degree_odd_ : weighted_degree_even_;


    std::for_each(std::execution::par,
                  graph_.getNodes().begin(),
                  graph_.getNodes().end(),
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

auto UpdateApproximation::name() const noexcept
    -> std::string
{
    return "UA," + std::to_string(iterations_);
}

auto UpdateApproximation::calculateWD(const NodeId node) const noexcept
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

auto UpdateApproximation::forwardUpdate(NodeId seed) noexcept
    -> void
{
    // get the weighted_degree of the second last iteration
    auto& weighted_degree = iterations_ % 2 == 0 ? weighted_degree_odd_ : weighted_degree_even_;
    auto old_seed_wd = weighted_degree_0_[seed];

    // reset seed node
    weighted_degree_odd_[seed] = 0.f;
    weighted_degree_0_[seed] = 0.f;
    weighted_degree_even_[seed] = 0.f;

    // update first iteration (wd) and second last iteration for 1 hop neighbors
    for(auto edge : graph_.getInverseEdgesOf(seed)) {
        if(weighted_degree_0_[edge.getDestination()] == 0.f) {
            continue;
        }
        weighted_degree_0_[edge.getDestination()] -= edge.getWeight();

        if(iterations_ > 2) {
            // update second iteration iteration score forwarding
            // and carry over from weighted_degree_0_ to weighted_Degree
            weighted_degree[edge.getDestination()] -= edge.getWeight() * old_seed_wd + edge.getWeight();
        }
    }

    if(iterations_ <= 2) {
        return;
    }

    // update second last iteration of 2 hop neighbors
    std::for_each(std::execution::par,
                  graph_.getInverseEdgesOf(seed).begin(),
                  graph_.getInverseEdgesOf(seed).end(),
                  [&](auto neighbor_edge) {
                      if(weighted_degree_0_[neighbor_edge.getDestination()] == 0.f) {
                          return;
                      }
                      for(auto edge : graph_.getInverseEdgesOf(neighbor_edge.getDestination())) {
                          if(weighted_degree_0_[edge.getDestination()] == 0.f) {
                              return;
                          }
                          weighted_degree[edge.getDestination()] -= edge.getWeight() * neighbor_edge.getWeight();
                      }
                  });
}

auto UpdateApproximation::backwardUpdate(NodeId node) noexcept
    -> void
{
    if(iterations_ <= 1) {
        // update was already completed in the forwardUpdate
        return;
    }

    auto& weighted_degree = iterations_ % 2 == 0 ? weighted_degree_even_ : weighted_degree_odd_;
    const auto& weighted_degree_old = iterations_ % 2 == 0 ? iterations_ == 2 ? weighted_degree_0_ : weighted_degree_odd_ : weighted_degree_even_;

    // update last iteration of node
    weighted_degree[node] = std::transform_reduce(
        std::execution::seq,
        graph_.getEdgesOf(node).begin(),
        graph_.getEdgesOf(node).end(),
        weighted_degree_old[node],
        std::plus<>(),
        [&](Edge edge) {
            return weighted_degree_old[edge.getDestination()] * edge.getWeight();
        });
}
