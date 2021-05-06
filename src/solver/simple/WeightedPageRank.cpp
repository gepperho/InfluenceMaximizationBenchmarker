#include <Edge.hpp>
#include <algorithm>
#include <execution>
#include <iostream>
#include <numeric>
#include <solver/simple/WeightedPageRank.hpp>

WeightedPageRank::WeightedPageRank(const Graph& graph) noexcept
    : graph_(graph),
      page_rank_(graph_.getNumberOfNodes(), 1.0f),
      temp_page_rank_(graph_.getNumberOfNodes()),
      probability_sum_(graph_.getNumberOfNodes(), 0.0f)
{}

auto WeightedPageRank::solve(const std::size_t k) noexcept
    -> std::vector<NodeId>
{
    precalculateWeightedSums();

    // initial page rank calculation
    //weightedPageRankIterations(10);
    weightedPageRankDelta();

    return getKMaxNodes(k);
}

auto WeightedPageRank::name() const noexcept
    -> std::string
{
    return "WeightedPageRank";
}

auto WeightedPageRank::weightedPageRankIterations(const int iterations) noexcept
    -> void
{
    // run several iterations
    for(int i = 0; i < iterations; ++i) {
        runSingleWeightedPageRankIteration();
    }
}


auto WeightedPageRank::weightedPageRankDelta() noexcept
    -> void
{
    auto nodes = graph_.getNodes();
    float max_diff;
    do {
        runSingleWeightedPageRankIteration();

        // check difference
        max_diff = std::transform_reduce(
            std::execution::par,
            std::begin(nodes),
            std::end(nodes),
            0.0f,
            [](float init, float new_value) {
                return std::max(init, new_value);
            },
            [&](auto id) {
                return abs(page_rank_[id] - temp_page_rank_[id]);
            });
        page_rank_ = temp_page_rank_;
    } while(max_diff > 0.0001);
}

auto WeightedPageRank::runSingleWeightedPageRankIteration() noexcept
    -> void
{
    auto dumping_factor = 0.85;
    auto nodes = graph_.getNodes();

    std::for_each(std::execution::par,
                  std::begin(nodes),
                  std::end(nodes),
                  [&](auto current_node) {
                      auto out_neighbors = graph_.getEdgesOf(current_node);
                      auto sum = std::transform_reduce(
                          std::execution::unseq,
                          std::begin(out_neighbors),
                          std::end(out_neighbors),
                          0.0,
                          [&](auto init, auto new_value) {
                              return init + new_value;
                          },
                          [&](Edge in_edge) {
                              auto target = in_edge.getDestination();
                              // probability sum will be 1 in the WC model, however this way the code also works for other models later
                              auto edge_probability = in_edge.getWeight() / probability_sum_[target];
                              return static_cast<double>(page_rank_[target])
                                  * static_cast<double>(edge_probability);
                          });

                      temp_page_rank_[current_node] = (1 - dumping_factor) + dumping_factor * sum;
                  });
}

auto WeightedPageRank::precalculateWeightedSums() noexcept
    -> void
{
    auto nodes = graph_.getNodes();
    for(auto current_node : nodes) {
        auto in_edges = graph_.getInverseEdgesOf(current_node);
        probability_sum_[current_node] = std::transform_reduce(
            in_edges.begin(),
            in_edges.end(),
            0.0f,
            [](auto acc, auto current) {
                return acc + current;
            },
            [&](Edge in_edge) {
                return in_edge.getWeight();
            });
    }
}


auto WeightedPageRank::getKMaxNodes(const std::size_t k) const noexcept
    -> std::vector<NodeId>
{
    std::vector<NodeId> nodes(graph_.getNumberOfNodes());
    std::iota(std::begin(nodes),
              std::end(nodes),
              0);

    // find k candidates, loop every possible node
    auto end_iter = k > nodes.size()
        ? std::end(nodes)
        : std::begin(nodes) + k;

    std::partial_sort(std::begin(nodes),
                      end_iter,
                      std::end(nodes),
                      [&](auto lhs, auto rhs) {
                          return page_rank_[lhs] > page_rank_[rhs];
                      });

    return std::vector(std::begin(nodes),
                       end_iter);
}
