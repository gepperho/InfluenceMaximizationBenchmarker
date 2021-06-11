#include <Graph.hpp>
#include <diffusion/IndependentCascade.hpp>
#include <execution>

using utils::range;


IndependentCascader::IndependentCascader(const Graph& graph) noexcept
    : graph_(graph) {}

auto IndependentCascader::cascadeForward(std::vector<NodeId> seeds, dsfmt_t dsfmt) const noexcept
    -> std::size_t
{
    auto work_queue = std::move(seeds);
    std::size_t activated_counter = work_queue.size();

    std::vector activated(static_cast<unsigned long>(graph_.getNumberOfNodes()),
                          false);

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

            auto prob = e.getWeight();

            if(prob > dsfmt_genrand_close_open(&dsfmt)) {
                activated[destination] = true;
                ++activated_counter;
                work_queue.emplace_back(destination);
            }
        }
    }

    return activated_counter;
}

auto IndependentCascader::cascadeBackward(std::vector<NodeId> seeds, dsfmt_t dsfmt) const noexcept
    -> std::size_t
{
    auto work_queue = std::move(seeds);
    std::size_t activated_counter = work_queue.size();

    std::vector activated(static_cast<unsigned long>(graph_.getNumberOfNodes()),
                          false);

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

            auto prob = e.getWeight();

            if(prob > dsfmt_genrand_close_open(&dsfmt)) {
                activated[destination] = true;
                ++activated_counter;
                work_queue.emplace_back(destination);
            }
        }
    }

    return activated_counter;
}

auto IndependentCascader::cascadeForwardN(const std::vector<NodeId>& seeds,
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
            return cascadeForward(seeds, dsfmt)
                / static_cast<double>(n);
        });
}

auto IndependentCascader::cascadeBackwardN(const std::vector<NodeId>& seeds,
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
            return cascadeBackward(seeds, dsfmt)
                / static_cast<double>(n);
        });
}
