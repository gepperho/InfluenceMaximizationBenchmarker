#include <Benchmarker.hpp>
#include <CLI/CLI.hpp>
#include <tbb/tbb.h>
#include <thread>

struct arguments
{
    std::string graph_file_path;
    ParseMode parse_mode;
    bool skip_line;
    std::string seed_file_path;
    std::string solver_name;
    std::size_t simulations;
    DiffusionModel diffusion_model;
    int number_of_threads;
    bool raw_output; // TODO implement a non raw output
    bool inverse_graph;
    bool backwards_diffusion;
};

auto parseEvaluatorArguments(int argc, char* argv[])
    -> arguments
{
    CLI::App app{"Influence Maximization Evaluator"};

    std::string graph_path;
    std::string seed_set_path;
    std::string solver = "Unknown";
    int threads = static_cast<int>(std::thread::hardware_concurrency());
    std::size_t simulations = 20000;
    bool print_raw = false;
    bool use_inverse = false;
    bool backwards_activation = false;
    bool skip = false;
    ParseMode parse_mode = ParseMode::VERTEX_LIST;
    DiffusionModel diffusion_model = DiffusionModel::INDEPENDENT_CASCADE;

    app.add_option("-g,--graph",
                   graph_path,
                   "graph file which should be solved")
        ->required();

    app.add_option("-f,--format",
                   parse_mode,
                   "format of the graph file (0=vertex list, 1=edge list)")
        ->required();

    app.add_flag("-m , --meta-data",
                 skip,
                 "set, if the first line of the input graph contains the number of nodes and edges");

    app.add_option("-p,--seedSetPath",
                   seed_set_path,
                   "input file holding the ids of the seed set");

    app.add_option("-a,--algorithm",
                   solver,
                   "Algorithm name to be used in the output. Default: unknown");


    app.add_option("-s,--simulations",
                   simulations,
                   "number of simulations performed in the independent cascade model for verification",
                   true);

    app.add_option("-d,--diffusionModel",
                   diffusion_model,
                   "Diffusion model to be used (0=IndependentCascade, 1=LinearThreshold). Default: Independent Cascade");

    app.add_option("-t,--threads",
                   threads,
                   "number of threads used for the computations",
                   true);

    app.add_flag("-r, --print-raw",
                 print_raw,
                 "if set, the results will be printed non pretty and not formatted");

    app.add_flag("-i, --inverse",
                 use_inverse,
                 "if set, the inverse of the graph will be used");
    app.add_flag("-b , --backwards-activation",
                 backwards_activation,
                 "if set, the influence propagation will also be calculated in backwards direction");

    try {
        app.parse(argc, argv);
    } catch(const CLI::ParseError& e) {
        std::exit(app.exit(e));
    }

    return {
        std::move(graph_path),
        parse_mode,
        skip,
        std::move(seed_set_path),
        solver,
        simulations,
        diffusion_model,
        threads,
        print_raw,
        use_inverse,
        backwards_activation,
    };
}

auto read_seed_set(const std::string& seed_path) -> std::vector<NodeId>
{
    std::vector<NodeId> seed_set;

    std::ifstream input_file(seed_path, std::ios::in);

    if(!input_file) {
        fmt::print("Seed file {} not found\n", seed_path);
        std::exit(1);
    }

    std::string line;

    while(std::getline(input_file, line)) {

        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
        seed_set.emplace_back(std::stol(line));
    }

    return seed_set;
}

auto main(int argc, char* argv[])
    -> int
{

    auto args = parseEvaluatorArguments(argc, argv);

    tbb::task_scheduler_init init(args.number_of_threads);

    Timer t;
    auto graph = [&] {
        switch(args.parse_mode) {
        case ParseMode::VERTEX_LIST:
            return parseVertexListFile(args.graph_file_path, args.inverse_graph, args.skip_line, !args.raw_output);
        case ParseMode::EDGE_LIST:
            return parseEdgeListFile(args.graph_file_path, args.inverse_graph, args.skip_line, !args.raw_output);
        default:
            fmt::print("unknown parse mode\n");
            std::exit(-1);
        }
    }();
    auto reading_time = t.elapsed();

    auto result_set = read_seed_set(args.seed_file_path);

    double forward_influence;
    auto backward_influence = -1.0;
    t.reset();

    auto diffusion_evaluator = DiffusionFactory::create(args.diffusion_model, graph);

    forward_influence = diffusion_evaluator->cascadeForwardN(result_set, args.simulations);
    if(args.backwards_diffusion) {
        backward_influence = diffusion_evaluator->cascadeBackwardN(result_set, args.simulations);
    }

    auto cascading_time = t.elapsed();

    auto message = fmt::format("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n",
                               graph.getGraphName(),
                               graph.getNumberOfNodes() - args.skip_line, // if skip is true, there is no node 0
                               graph.getNumberOfEdges(),
                               result_set.size(),
                               args.simulations,
                               args.solver_name,
                               "$runtime",
                               forward_influence,
                               backward_influence,
                               reading_time,
                               cascading_time,
                               args.number_of_threads,
                               args.diffusion_model);

    fmt::print("{}", message);
    std::cout << std::flush;
}
