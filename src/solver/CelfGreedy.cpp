#include <dSFMT.h>
#include <execution>
#include <fmt/core.h>
#include <queue>
#include <random>
#include <solver/CelfGreedy.hpp>
#include <unordered_set>

CelfGreedy::CelfGreedy(const Graph& graph, const int simulations) noexcept
    : graph_(graph),
      spread_(graph.getNumberOfNodes()),
      simulations_(simulations)
{}
auto CelfGreedy::solve(std::size_t k) noexcept
    -> std::vector<NodeId>
{
    std::vector<NodeId> nodes(graph_.getNumberOfNodes());
    std::iota(std::begin(nodes),
              std::end(nodes),
              0);
    std::vector<NodeId> seed_set;

    std::for_each(std::execution::seq,
                  nodes.begin(),
                  nodes.end(),
                  [&](auto node) {
                      spread_[node] = evaluateSpread(node);
                  });
    std::unordered_set<NodeId> updated;
    auto seed_comparison =
        [&](const auto& lhs, const auto& rhs) {
            if(spread_[lhs] == spread_[rhs]) {
                return updated.find(rhs) != updated.end();
            }
            return spread_[lhs] < spread_[rhs];
        };

    using SeedPriorityQueue = std::priority_queue<NodeId,
                                                  std::vector<NodeId>,
                                                  decltype(seed_comparison)>;
    SeedPriorityQueue pq(seed_comparison, nodes);

    // greedy picking
    auto initial_top_node = pq.top();
    pq.pop();
    seed_set.reserve(k);
    seed_set.emplace_back(initial_top_node);

    while(seed_set.size() < k) {
        auto top_node = pq.top();
        pq.pop();

        if(updated.find(top_node) != updated.end()) {
            // current top node is up to date and thus can be added
            seed_set.emplace_back(top_node);
            updated.clear();
            continue;
        }
        // current top node is outdated
        spread_[top_node] = evaluateSpread(top_node);

        if(spread_[top_node] >= spread_[pq.top()]) {
            // add top_node
            seed_set.emplace_back(top_node);
        } else {
            updated.insert(top_node);
            pq.push(top_node);
        }
    }

    return seed_set;
}

auto CelfGreedy::name() const noexcept
    -> std::string
{
    return "CELF-Greedy," + std::to_string(simulations_);
}
auto CelfGreedy::evaluateSpread(NodeId node) const noexcept
    -> double
{
    return std::transform_reduce(
        std::execution::par,
        std::begin(utils::range(simulations_)),
        std::end(utils::range(simulations_)),
        0.,
        [](auto acc, auto current) {
            return acc + current;
        },
        [&](auto /*node*/) {
            return static_cast<double>(singleSimulation(node))
                / static_cast<double>(simulations_);
        });
}
auto CelfGreedy::singleSimulation(NodeId node) const noexcept
    -> long
{
    std::vector activated(graph_.getNumberOfNodes(), false);
    std::vector<NodeId> seeds;

    dsfmt_t dsfmt;
    dsfmt_init_gen_rand(&dsfmt, rand());

    std::vector<NodeId> work_queue;
    work_queue.emplace_back(node);

    while(!work_queue.empty()) {

        auto current_node = work_queue.back();
        work_queue.pop_back();

        for(auto neighbor_edge : graph_.getEdgesOf(current_node)) {
            if(activated[neighbor_edge.getDestination()]) {
                continue;
            }
            auto rnd = dsfmt_genrand_close_open(&dsfmt);
            if(rnd < neighbor_edge.getWeight()) {
                activated[neighbor_edge.getDestination()] = true;
                work_queue.emplace_back(neighbor_edge.getDestination());
            }
        }
    }

    return std::count_if(activated.begin(),
                         activated.end(),
                         [](auto x) { return x; });
}
