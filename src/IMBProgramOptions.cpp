#include <CLI/CLI.hpp>
#include <IMBProgramOptions.hpp>
#include <thread>

IMBProgramOptions::IMBProgramOptions(int reruns,
                                     ParseMode parse_mode,
                                     DiffusionModel diffusion_model,
                                     bool print_raw,
                                     int number_of_threads,
                                     int number_of_seeds,
                                     std::string path,
                                     bool use_inverse_graph,
                                     bool backwards_activation,
                                     std::optional<std::string> out_path,
                                     std::vector<std::string> solver)
    : reruns_(static_cast<size_t>(reruns)),
      parse_mode_(parse_mode),
      diffusion_model_(diffusion_model),
      print_raw_(print_raw),
      number_of_threads_(number_of_threads),
      number_of_seeds_(static_cast<size_t>(number_of_seeds)),
      graph_path_(std::move(path)),
      use_inverse_graph_(use_inverse_graph),
      backwards_activation_(backwards_activation),
      out_path_(std::move(out_path)),
      solver_(std::move(solver))
{}

auto IMBProgramOptions::getReruns() const
    -> std::size_t
{
    return reruns_;
}

auto IMBProgramOptions::getSolvers() const
    -> const std::vector<std::string>&
{
    return solver_;
}

auto IMBProgramOptions::getGraphPath() const
    -> const std::string&
{
    return graph_path_;
}


auto IMBProgramOptions::printRaw() const
    -> bool
{
    return print_raw_;
}

auto IMBProgramOptions::getNumberOfThreads() const
    -> int
{
    return number_of_threads_;
}

auto IMBProgramOptions::getOutPath() const
    -> const std::optional<std::string>&
{
    return out_path_;
}


auto IMBProgramOptions::getParseMode() const
    -> ParseMode
{
    return parse_mode_;
}

auto IMBProgramOptions::getNumberOfSeeds() const
    -> std::size_t
{
    return number_of_seeds_;
}

auto IMBProgramOptions::shouldUseInverseGraph() const
    -> bool
{
    return use_inverse_graph_;
}

auto IMBProgramOptions::shouldPerformBackwardsActivation()
    const -> bool
{
    return backwards_activation_;
}
auto IMBProgramOptions::getDiffusionModel() const -> DiffusionModel
{
    return diffusion_model_;
}

auto parseArguments(int argc, char* argv[])
    -> IMBProgramOptions
{
    CLI::App app{"Influence Maximization Benchmarks"};

    std::string path;
    std::optional<std::string> out_path = std::nullopt;
    int threads = static_cast<int>(std::thread::hardware_concurrency());
    int seeds = 50;
    int reruns = 20000;
    bool print_raw = false;
    bool use_inverse = false;
    bool backwards_activation = false;
    ParseMode parse_mode = ParseMode::VERTEX_LIST;
    DiffusionModel diffusion_model = DiffusionModel::INDEPENDENT_CASCADE;

    std::vector<std::string> solvers;

    app.add_option("-g,--graph",
                   path,
                   "graph file which should be solved")
        ->required();

    app.add_option("-f,--format",
                   parse_mode,
                   "format of the graph file (0=vertex list, 1=edge list)")
        ->required();

    app.add_option("-d,--diffusionModel",
                   diffusion_model,
                   "Diffusion model to be used (0=IndependentCascade, 1=LinearThreshold). Default: Independent Cascade");

    app.add_option("-o,--output",
                   out_path,
                   "output file to which the seed nodes will be written");


    app.add_option("-s,--simulations",
                   reruns,
                   "number of simulations performed in the independent cascade model for verification",
                   true);

    app.add_option("-t,--threads",
                   threads,
                   "number of threads used for the computations",
                   true);

    app.add_option("-k,--seeds",
                   seeds,
                   "number of seeds which maximize the influence",
                   true);

    app.add_option("-a,--algorithm",
                   solvers,
                   "algorithm/solvers used to solve the problem instance. \n"
                   "Multiple algorithms can be run (comma separate argument passing).\n"
                   "available are: \n"
                   "\t pr (PageRank)\n"
                   "\t wpr (WeightedPageRank)\n"
                   "\t degree (degree discount)\n"
                   "\t highdegree (high degree)\n"
                   "\t random\n"
                   "\t celf (celf greedy)\n"
                   "\t wd (weightedDegree) [recursion depth:int]\n"
                   "\t ipa (Influence Path Algorithm) [precision divider:int]\n"
                   "\t iterative-ua [recursion depth:int]\n")
        ->required()
        ->delimiter(',');

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

    return IMBProgramOptions{reruns,
                             parse_mode,
                             diffusion_model,
                             print_raw,
                             threads,
                             seeds,
                             std::move(path),
                             use_inverse,
                             backwards_activation,
                             std::move(out_path),
                             std::move(solvers)};
}
