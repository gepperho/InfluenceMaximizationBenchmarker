#include <Benchmarker.hpp>
#include <solver/CelfGreedy.hpp>
#include <solver/IMM.hpp>
#include <solver/IPA.hpp>
#include <solver/SolverFactory.hpp>
#include <solver/WeightedDegree.hpp>
#include <solver/iterative/EaSyIM.hpp>
#include <solver/iterative/EaSyIM_Delta.hpp>
#include <solver/iterative/UpdateApproximation.hpp>
#include <solver/simple/DegreeDiscountSolver.hpp>
#include <solver/simple/HighDegree.hpp>
#include <solver/simple/PageRankSolver.hpp>
#include <solver/simple/Random.hpp>
#include <solver/simple/WeightedPageRank.hpp>


auto SolverFactory::benchmarkAll(const std::vector<std::string>& arguments, Graph& graph, Benchmarker& bm) noexcept
    -> void
{
    int argument_count = 0;
    for(std::size_t index{0}; index < arguments.size(); ++index) {

        if(argument_count > 0) {
            --argument_count;
            continue;
        }

        auto solver_arg = arguments[index];
        std::transform(solver_arg.begin(),
                       solver_arg.end(),
                       solver_arg.begin(),
                       [](char c) {
                           return ::tolower(c);
                       });

        auto solver = [&]() -> std::unique_ptr<SolverInterface> {
            try {
                if(solver_arg == "pr") {
                    if(arguments.size() > index + 1 && util::isNumber(arguments[index + 1])) {
                        auto iterations = util::extractParameter<int>(arguments[index + 1]);
                        argument_count = 1;
                        return std::make_unique<PageRankSolver>(graph, iterations.value());
                    }
                    return std::make_unique<PageRankSolver>(graph);
                }

                if(solver_arg == "wpr") {
                    return std::make_unique<WeightedPageRank>(graph);
                }

                if(solver_arg == "degree") {
                    return std::make_unique<DegreeDiscountSolver>(graph);
                }

                if(solver_arg == "highdegree" or solver_arg == "high_degree") {
                    return std::make_unique<HighDegree>(graph);
                }

                if(solver_arg == "random" or solver_arg == "rnd") {
                    return std::make_unique<Random>(graph);
                }

                if(solver_arg == "celf" or solver_arg == "celfgreedy") {
                    if(arguments.size() > index + 1 && util::isNumber(arguments[index + 1])) {
                        auto mc_simulations = util::extractParameter<int>(arguments[index + 1]);
                        argument_count = 1;
                        return std::make_unique<CelfGreedy>(graph, mc_simulations.value());
                    }

                    return std::make_unique<CelfGreedy>(graph);
                }

                if(solver_arg == "wd" or solver_arg == "weighteddegree") {
                    return std::make_unique<WeightedDegree>(graph);
                }

                if(solver_arg == "ipa") {
                    if(arguments.size() > index + 1 && util::isNumber(arguments[index + 1])) {
                        auto threshold_divider = util::extractParameter<int>(arguments[index + 1]);
                        argument_count = 1;
                        return std::make_unique<IPASolver>(graph, threshold_divider.value());
                    }
                    return std::make_unique<IPASolver>(graph);
                }

                if(solver_arg == "easyim-delta") {
                    if(arguments.size() > index + 1 && util::isNumber(arguments[index + 1])) {
                        auto recursion_depth = util::extractParameter<int>(arguments[index + 1]);
                        argument_count = 1;
                        return std::make_unique<EasyImDelta>(graph, recursion_depth.value());
                    }
                    return std::make_unique<EasyImDelta>(graph);
                }

                if(solver_arg == "easyim") {
                    if(arguments.size() > index + 1 && util::isNumber(arguments[index + 1])) {
                        auto recursion_depth = util::extractParameter<int>(arguments[index + 1]);
                        argument_count = 1;
                        return std::make_unique<EaSyIM>(graph, recursion_depth.value());
                    }
                    return std::make_unique<EaSyIM>(graph);
                }

                if(solver_arg == "ua") {
                    if(arguments.size() > index + 1 && util::isNumber(arguments[index + 1])) {
                        auto recursion_depth = util::extractParameter<int>(arguments[index + 1]);
                        argument_count = 1;
                        return std::make_unique<UpdateApproximation>(graph, recursion_depth.value());
                    }
                    return std::make_unique<UpdateApproximation>(graph);
                }

                if(solver_arg == "imm") {
                    return std::make_unique<IMM>(graph);
                }

                // "Default" case
                fmt::print("unknown solver{}\n", solver_arg);

            } catch(const std::invalid_argument& ia) {
                std::cerr << "Invalid argument: "
                          << ia.what()
                          << " for algorithm: "
                          << solver_arg << '\n';
            }
            return nullptr;
        }();

        if(solver) {
            bm.benchmark(solver);
        }
    }
}
