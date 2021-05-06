#pragma once

#include <Graph.hpp>
#include <IMBProgramOptions.hpp>
#include <Timer.h>
#include <diffusion/DiffusionFactory.hpp>
#include <diffusion/DiffusionModelInterface.hpp>
#include <diffusion/IndependentCascade.hpp>
#include <diffusion/LinearThreshold.hpp>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <solver/SolverInterface.hpp>

class Benchmarker
{
public:
    Benchmarker(const Graph& graph,
                IMBProgramOptions& options,
                double file_reading_time)
        : graph_(graph),
          seeds_(options.getNumberOfSeeds()),
          reruns_(options.getReruns()),
          out_path_(options.getOutPath()),
          raw_output_(options.printRaw()),
          backwards_activation_(options.shouldPerformBackwardsActivation()),
          reading_time_(file_reading_time),
          threads_(options.getNumberOfThreads()),
          diffusion_model_(options.getDiffusionModel())
    {

        if(!raw_output_) {
            auto message =
                fmt::format("Graph: |V| = {}, |E| = {}, Seeds: {}, Simulation Reruns: {}",
                            graph_.getNumberOfNodes(),
                            graph_.getNumberOfEdges(),
                            seeds_,
                            reruns_);

            message = fmt::format(
                "┌{0:─^{2}}┐\n"
                "│{1: ^{2}}│\n"
                "└{0:─^{2}}┘\n",
                "",
                message,
                80);

            message =
                fmt::format("{}Diffusion Model: {}\n",
                            message,
                            util::diffusionModelToString(diffusion_model_));

            message = fmt::format("{}File reading time: {:.2f}s\n",
                                  message,
                                  reading_time_);

            fmt::print("{}", message);
        }
    }

    auto benchmark(std::unique_ptr<SolverInterface>& solver)
    {
        auto solver_name = solver->name();
        Timer t;
        auto seeds = solver->solve(seeds_);
        auto calculation_time = t.elapsed();

        if(!raw_output_) {
            auto message = fmt::format("({}) runtime: {:.3f}s\n",
                                       solver_name,
                                       calculation_time);
            fmt::print("{}", message);
        }

        if(auto path_opt = createOutPathFor(solver_name)) {
            auto path = std::move(path_opt.value());
            writeSeedsTo(path, seeds);
        }

        const auto diffusion_evaluator = DiffusionFactory::create(diffusion_model_, graph_);

        // cascade_timer
        t.reset();

        double forward_influence = diffusion_evaluator->cascadeForwardN(seeds, reruns_);
        double backward_influence = -1.0;

        if(backwards_activation_) {
            backward_influence = diffusion_evaluator->cascadeBackwardN(seeds, reruns_);
        }

        auto cascading_time = t.elapsed();

        if(!raw_output_) {

            auto message =
                fmt::format("({}) number of average forward activated nodes: {:.2f}\n",
                            solver_name,
                            forward_influence);

            message = fmt::format("{}({}) number of average backward activated nodes: {:.2f}\n",
                                  message,
                                  solver_name,
                                  backward_influence);

            message = fmt::format("{}({}) cascading time: {:.2f}s\n", message, solver_name, cascading_time);
            message = fmt::format("{}({}) number of threads: {}\n", message, solver_name, threads_);

            fmt::print("{}", message);
        } else {
            // raw output
            auto message = fmt::format("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n",
                                       graph_.getGraphName(),
                                       graph_.getNumberOfNodes(),
                                       graph_.getNumberOfEdges(),
                                       seeds_,
                                       reruns_,
                                       solver_name,
                                       calculation_time,
                                       forward_influence,
                                       backward_influence,
                                       reading_time_,
                                       cascading_time,
                                       threads_,
                                       diffusion_model_);

            fmt::print("{}", message);
            std::cout << std::flush;
        }
    }

private:
    auto
    createOutPathFor(std::string_view algo_name) const
        -> std::optional<std::string>
    {
        namespace fs = std::filesystem;

        if(!out_path_) {
            return std::nullopt;
        }

        const auto& base_path = out_path_.value();
        auto alg_path = fmt::format("{}/{}", base_path, algo_name);

        fs::create_directories(alg_path);

        auto min_filename = 0ul;
        for(const auto& entry : fs::directory_iterator(alg_path)) {

            if(!entry.is_regular_file()) {
                continue;
            }

            min_filename++;
        }

        return fmt::format("{}/result-{}.txt", alg_path, ++min_filename);
    }

    auto writeSeedsTo(std::string_view path,
                      const std::vector<NodeId>& seeds) const
        -> void
    {
        std::ofstream out_file{path.data()};
        for(auto node : seeds) {
            out_file << node << "\n";
        }
    }

    const Graph& graph_;
    std::size_t seeds_;
    std::size_t reruns_;
    std::optional<std::string> out_path_;
    bool raw_output_;
    bool backwards_activation_;
    double reading_time_;
    int threads_;
    DiffusionModel diffusion_model_;
};
