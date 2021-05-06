#include <fmt/core.h>
#include <iostream>
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
#include <util/Definitions.hpp>


auto SolverFactory::create(const std::vector<std::string>& arguments, Graph& graph) noexcept -> std::vector<std::unique_ptr<SolverInterface>>
{
    // create a vector of solvers
    std::vector<std::unique_ptr<SolverInterface>> solvers;

    int argument_count = 0;
    for(std::size_t index{0}; index < arguments.size(); ++index) {

        if(argument_count > 0) {
            --argument_count;
            continue;
        }

        auto solver = arguments[index];
        std::transform(solver.begin(),
                       solver.end(),
                       solver.begin(),
                       [](char c) {
                           return ::tolower(c);
                       });

        try {
            if(solver == "pr") {
                solvers.emplace_back(std::make_unique<PageRankSolver>(graph));

            } else if(solver == "wpr") {
                solvers.emplace_back(std::make_unique<WeightedPageRank>(graph));

            } else if(solver == "degree") {
                solvers.emplace_back(std::make_unique<DegreeDiscountSolver>(graph));

            } else if(solver == "highdegree" or solver == "high_degree") {
                solvers.emplace_back(std::make_unique<HighDegree>(graph));

            } else if(solver == "random" or solver == "rnd") {
                solvers.emplace_back(std::make_unique<Random>(graph));

            } else if(solver == "celf" or solver == "celfgreedy") {
                if(arguments.size() > index + 1 && util::isNumber(arguments[index + 1])) {
                    auto mc_simulations = util::extractParameter<int>(arguments[index + 1]);
                    argument_count = 1;
                    solvers.emplace_back(std::make_unique<CelfGreedy>(graph, mc_simulations.value()));
                } else {
                    solvers.emplace_back(std::make_unique<CelfGreedy>(graph));
                }

            } else if(solver == "wd" or solver == "weighteddegree") {
                solvers.emplace_back(std::make_unique<WeightedDegree>(graph));

            } else if(solver == "ipa") {
                if(arguments.size() > index + 1 && util::isNumber(arguments[index + 1])) {
                    auto threshold_divider = util::extractParameter<int>(arguments[index + 1]);
                    argument_count = 1;
                    solvers.emplace_back(std::make_unique<IPASolver>(graph, threshold_divider.value()));
                } else {
                    solvers.emplace_back(std::make_unique<IPASolver>(graph));
                }


            } else if(solver == "easyim-delta") {
                if(arguments.size() > index + 1 && util::isNumber(arguments[index + 1])) {
                    auto recursion_depth = util::extractParameter<int>(arguments[index + 1]);
                    argument_count = 1;
                    solvers.emplace_back(std::make_unique<EaSyIM_Delta>(graph, recursion_depth.value()));
                } else {
                    solvers.emplace_back(std::make_unique<EaSyIM_Delta>(graph));
                }

            } else if(solver == "easyim" or solver == "EASYIM" or solver == "EaSyIM") {
                if(arguments.size() > index + 1 && util::isNumber(arguments[index + 1])) {
                    auto recursion_depth = util::extractParameter<int>(arguments[index + 1]);
                    argument_count = 1;
                    solvers.emplace_back(std::make_unique<EaSyIM>(graph, recursion_depth.value()));
                } else {
                    solvers.emplace_back(std::make_unique<EaSyIM>(graph));
                }

            } else if(solver == "ua" or solver == "UA") {
                if(arguments.size() > index + 1 && util::isNumber(arguments[index + 1])) {
                    auto recursion_depth = util::extractParameter<int>(arguments[index + 1]);
                    argument_count = 1;
                    solvers.emplace_back(std::make_unique<UpdateApproximation>(graph, recursion_depth.value()));
                } else {
                    solvers.emplace_back(std::make_unique<UpdateApproximation>(graph));
                }

            } else if(solver == "imm") {
                solvers.emplace_back(std::make_unique<IMM>(graph));

            } else {
                fmt::print("unknown solver {}\n", solver);
            }
        } catch(const std::invalid_argument& ia) {
            std::cerr << "Invalid argument: "
                      << ia.what()
                      << " for algorithm: "
                      << solver << '\n';
        }
    }
    return solvers;
}
