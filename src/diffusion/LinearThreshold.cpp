#include <dSFMT.h>
#include <diffusion/LinearThreshold.hpp>
#include <execution>

using utils::range;

LinearThresholdEvaluation::LinearThresholdEvaluation(const Graph& graph) noexcept
    : graph_(graph)
{
}

auto LinearThresholdEvaluation::cascadeForwardN(const std::vector<NodeId>& seeds,
                                                std::size_t n) const noexcept
    -> double
{
    return std::transform_reduce(
        std::execution::par,
        std::begin(range(n)),
        std::end(range(n)),
        0.0,
        [](auto acc, auto current) {
            return acc + current;
        },
        [&](auto /*node*/) {
            // generate new random sequence for every MC run
            dsfmt_t dsfmt;
            dsfmt_init_gen_rand(&dsfmt, rand());
            return forwardInfluence(seeds, dsfmt)
                / static_cast<double>(n);
        });
}

auto LinearThresholdEvaluation::cascadeBackwardN(const std::vector<NodeId>& seeds,
                                                 std::size_t n) const noexcept
    -> double
{
    return std::transform_reduce(
        std::execution::par,
        std::begin(range(n)),
        std::end(range(n)),
        0.0,
        [](auto acc, auto current) {
            return acc + current;
        },
        [&](auto /*node*/) {
            // generate new random sequence for every MC run
            dsfmt_t dsfmt;
            dsfmt_init_gen_rand(&dsfmt, rand());
            return backwardInfluence(seeds, dsfmt)
                / static_cast<double>(n);
        });
}

auto LinearThresholdEvaluation::forwardInfluence(std::vector<NodeId> seeds, dsfmt_t dsfmt) const noexcept
    -> std::size_t
{
    auto work_queue = std::move(seeds);
    std::size_t activated_counter = work_queue.size();

    std::vector activated(static_cast<unsigned long>(graph_.getNumberOfNodes()),
                          false);
    std::vector input(graph_.getNumberOfNodes(), 0.0f);
    std::vector threshold(graph_.getNumberOfNodes(), -1.0);


    for(auto n : work_queue) {
        activated[n] = true;
    }

    while(!work_queue.empty()) {
        auto current = work_queue.back();
        work_queue.pop_back();

        auto edges = graph_.getEdgesOf(current);

        for(const auto& e : edges) {
            auto destination = e.getDestination();

            if(activated[destination]) {
                continue;
            }
            if(threshold[destination] < 0) {
                // actually assign the threshold
                threshold[destination] = dsfmt_genrand_close_open(&dsfmt);
            }
            input[destination] += e.getWeight();

            if(threshold[destination] <= input[destination]) {
                activated[destination] = true;
                ++activated_counter;
                work_queue.emplace_back(destination);
            }
        }
    }

    return activated_counter;
}

auto LinearThresholdEvaluation::backwardInfluence(std::vector<NodeId> seeds, dsfmt_t dsfmt) const noexcept
    -> std::size_t
{
    auto work_queue = std::move(seeds);
    std::size_t activated_counter = work_queue.size();

    std::vector activated(static_cast<unsigned long>(graph_.getNumberOfNodes()),
                          false);
    std::vector input(graph_.getNumberOfNodes(), 0.0f);
    std::vector threshold(graph_.getNumberOfNodes(), -1.0);


    for(auto n : work_queue) {
        activated[n] = true;
    }

    while(!work_queue.empty()) {
        auto current = work_queue.back();
        work_queue.pop_back();

        auto edges = graph_.getInverseEdgesOf(current);

        for(const auto& e : edges) {
            auto destination = e.getDestination();

            if(activated[destination]) {
                continue;
            }
            if(threshold[destination] < 0) {
                // actually assign the threshold
                threshold[destination] = dsfmt_genrand_close_open(&dsfmt);
            }
            input[destination] += e.getWeight();

            if(threshold[destination] <= input[destination]) {
                activated[destination] = true;
                ++activated_counter;
                work_queue.emplace_back(destination);
            }
        }
    }

    return activated_counter;
}
