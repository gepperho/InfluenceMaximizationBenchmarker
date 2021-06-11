#include <Edge.hpp>
#include <algorithm>
#include <execution>
#include <solver/simple/PageRankSolver.hpp>

PageRankSolver::PageRankSolver(const Graph& graph) noexcept
    : graph_(graph),
      page_rank_(graph_.getNumberOfNodes(), 1.0f)
{}

PageRankSolver::PageRankSolver(const Graph& graph, int iterations) noexcept
    : graph_(graph),
      page_rank_(graph_.getNumberOfNodes(), 1.0f),
      iterations_(iterations)
{}

auto PageRankSolver::solve(const std::size_t k) noexcept
    -> std::vector<NodeId>
{

    if(iterations_ != 0) {
        // run several iterations
        for(int i = 0; i < iterations_; ++i) {
            pageRankIteration();
        }
    } else {
        // run until the pr converges
        pageRankConverge();
    }

    return getKMaxNodes(k);
}



auto PageRankSolver::name() const noexcept
    -> std::string
{
    if(iterations_ != 0) {
        return "PageRank," + std::to_string(iterations_);
    }
    return "PageRank";
}

auto PageRankSolver::pageRankIteration() noexcept
    -> void
{
    auto dumping_factor = 0.85;

    auto nodes = graph_.getNodes();
    temp_rank_ = page_rank_;


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

                      page_rank_[current_node] = (1 - dumping_factor) + dumping_factor * sum;
                  });
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

auto PageRankSolver::pageRankConverge() noexcept
    -> void
{
    auto nodes = graph_.getNodes();
    float max_diff;
    do {
        pageRankIteration();

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
                return abs(page_rank_[id] - temp_rank_[id]);
            });
    } while(max_diff > 0.0001);
}
