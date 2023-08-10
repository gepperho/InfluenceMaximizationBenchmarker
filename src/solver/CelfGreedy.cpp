#include <dSFMT.h>
#include <execution>
#include <queue>
#include <random>
#include <solver/CelfGreedy.hpp>
#include <unordered_set>

CelfGreedy::CelfGreedy(const Graph& graph, const int simulations) noexcept
    : graph_(graph),
      spread_delta_(graph.getNumberOfNodes()),
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
    double seed_set_spread = 0;

    std::for_each(std::execution::seq,
                  nodes.begin(),
                  nodes.end(),
                  [&](auto node) {
                      spread_delta_[node] = evaluateSpread(node, seed_set);
                  });
    std::unordered_map<NodeId, double> updated_with_total_spread;
    auto seed_comparison =
        [&](const auto& lhs, const auto& rhs) {
            if(spread_delta_[lhs] == spread_delta_[rhs]) {
                return updated_with_total_spread.find(rhs) != updated_with_total_spread.end();
            }
            return spread_delta_[lhs] < spread_delta_[rhs];
        };

    using SeedPriorityQueue = std::priority_queue<NodeId,
                                                  std::vector<NodeId>,
                                                  decltype(seed_comparison)>;
    SeedPriorityQueue priority_queue(seed_comparison, nodes);

    // greedy picking
    auto initial_top_node = priority_queue.top();
    priority_queue.pop();
    seed_set.reserve(k);
    seed_set.emplace_back(initial_top_node);

    while(seed_set.size() < k) {
        auto top_node = priority_queue.top();
        priority_queue.pop();

        if(updated_with_total_spread.find(top_node) != updated_with_total_spread.end()) {
            // current top node is up-to-date and thus can be added
            seed_set_spread = updated_with_total_spread.at(top_node);
            seed_set.emplace_back(top_node);
            updated_with_total_spread.clear();
            continue;
        }
        // current top node is outdated
        auto spread = evaluateSpread(top_node, seed_set);
        spread_delta_[top_node] = spread - seed_set_spread;

        if(spread_delta_[top_node] >= spread_delta_[priority_queue.top()]) {
            // add top_node
            seed_set_spread = spread;
            seed_set.emplace_back(top_node);
            updated_with_total_spread.clear();
        } else {
            updated_with_total_spread[top_node] = spread;
            priority_queue.push(top_node);
        }
    }

    return seed_set;
}

auto CelfGreedy::name() const noexcept
    -> std::string
{
    return "CELF-Greedy," + std::to_string(simulations_);
}
auto CelfGreedy::evaluateSpread(NodeId node, std::vector<NodeId>& seed_set) const noexcept
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
            return static_cast<double>(singleSimulation(node, seed_set))
                / static_cast<double>(simulations_);
        });
}
auto CelfGreedy::singleSimulation(NodeId node, std::vector<NodeId>& seed_set) const noexcept
    -> long
{
    std::vector activated(graph_.getNumberOfNodes(), false);
    for(const auto seed_node : seed_set) {
        activated[seed_node] = true;
    }
    activated[node] = true;

    dsfmt_t dsfmt;
    dsfmt_init_gen_rand(&dsfmt, rand());

    std::vector<NodeId> work_queue = seed_set;
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
