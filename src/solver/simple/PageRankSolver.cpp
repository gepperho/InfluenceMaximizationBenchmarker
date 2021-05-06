#include <Edge.hpp>
#include <algorithm>
#include <execution>
#include <numeric>
#include <solver/simple/PageRankSolver.hpp>

PageRankSolver::PageRankSolver(const Graph& graph) noexcept
    : graph_(graph),
      page_rank_(graph_.getNumberOfNodes(), 1.0f) {}

auto PageRankSolver::solve(const std::size_t k) noexcept
  -> std::vector<NodeId>
{
    // initial page rank calculation
    // TODO evaluate the number of iterations
    pageRankIteration(10);

    return getKMaxNodes(k);
}



auto PageRankSolver::name() const noexcept
  -> std::string
{
    return "PageRank";
}

auto PageRankSolver::pageRankIteration(const int iterations) noexcept
  -> void
{
    auto dumping_factor = 0.85;

    auto nodes = graph_.getNodes();
    std::vector<float> temp_page_rank(graph_.getNumberOfNodes());

    // run several iterations
    for(int i = 0; i < iterations; ++i) {
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
                                  // use in-degree since the in-degree represents the chances that this neighbor is later activated by the current node
                                  return static_cast<double>(page_rank_[target])
                                      / static_cast<double>(graph_.getInDegreeOf(target));
                              });

                          temp_page_rank[current_node] = (1 - dumping_factor) + dumping_factor * sum;
                      });
        page_rank_ = temp_page_rank;
    }
}

auto PageRankSolver::getKMaxNodes(const std::size_t k) const noexcept
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
