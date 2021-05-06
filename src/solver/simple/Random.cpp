#include "solver/simple/Random.hpp"
#include <random>

Random::Random(const Graph& graph) noexcept
    : graph_(graph)
{}
auto Random::solve(std::size_t k) noexcept
    -> std::vector<NodeId>
{
    // TODO: use parallel RandomGenerator
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<NodeId> dist(0, graph_.getNumberOfNodes() - 1);

    std::vector<NodeId> seeds;

    while(seeds.size() < k) {
        auto rnd_node = dist(mt);

        if(std::find(seeds.begin(), seeds.end(), rnd_node) == seeds.end()) {
            seeds.emplace_back(rnd_node);
        }
    }

    return seeds;
}
auto Random::name() const noexcept
    -> std::string
{
    return "Random";
}
