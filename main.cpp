//this avoids all the warnings from dSFMT
#define DSFMT_MEXP 19937

#include <Benchmarker.hpp>
#include <random>
#include <solver/SolverFactory.hpp>
#include <tbb/tbb.h>

auto main(int argc, char* argv[])
    -> int
{
    auto options = parseArguments(argc, argv);
    auto graph_file = options.getGraphPath();
    auto number_of_threads = options.getNumberOfThreads();
    auto parse_mode = options.getParseMode();
    auto use_inverse = options.shouldUseInverseGraph();
    auto random_edge_weights = options.shouldUseRandomEdgeWeights();
    const auto& strategies = options.getSolvers();
    tbb::task_scheduler_init init(number_of_threads);

    Timer file_reading_timer = Timer();
    auto graph = [&] {
        switch(parse_mode) {
        case ParseMode::VERTEX_LIST:
            return parseVertexListFile(graph_file, use_inverse, false, random_edge_weights, !options.printRaw());
        case ParseMode::EDGE_LIST:
            return parseEdgeListFile(graph_file, use_inverse, false, random_edge_weights, !options.printRaw());
        default:
            fmt::print("unknown parse mode\n");
            std::exit(-1);
        }
    }();
    auto reading_time = file_reading_timer.elapsed();

    // create random state for our "seed generator"
    // use rd to make sure the seed is different in every execution
    // using time could result in the same seed, if the program is started multiple times within one second
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, RAND_MAX);
    srand(dis(gen));

    Benchmarker bm{graph, options, reading_time};
    SolverFactory::benchmarkAll(strategies, graph, bm);
}
