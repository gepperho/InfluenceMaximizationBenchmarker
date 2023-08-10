#pragma once

#include <Parser.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <util/Definitions.hpp>
#include <vector>

using util::DiffusionModel;

class IMBProgramOptions
{
public:
    IMBProgramOptions(int reruns,
                      ParseMode parse_mode,
                      DiffusionModel diffusion_model,
                      bool print_raw,
                      int number_of_threads,
                      int number_of_seeds,
                      std::string path,
                      bool use_inverse_graph,
                      bool backwards_activation,
                      bool random_edge_weights,
                      std::optional<std::string> out_path,
                      std::vector<std::string> solver);
    IMBProgramOptions() = delete;
    IMBProgramOptions(const IMBProgramOptions&) = delete;
    IMBProgramOptions(IMBProgramOptions&&) = default;

    auto operator=(const IMBProgramOptions&)
        -> IMBProgramOptions& = delete;

    auto operator=(IMBProgramOptions&&)
        -> IMBProgramOptions& = default;

    auto getReruns() const
        -> std::size_t;

    auto getGraphPath() const
        -> const std::string&;

    auto printRaw() const
        -> bool;

    auto getNumberOfThreads() const
        -> int;

    auto getParseMode() const
        -> ParseMode;

    auto getNumberOfSeeds() const
        -> std::size_t;

    auto getOutPath() const
        -> const std::optional<std::string>&;

    auto getSolvers() const
        -> const std::vector<std::string>&;

    auto shouldUseInverseGraph() const
        -> bool;

    auto shouldPerformBackwardsActivation() const
        -> bool;

    auto shouldUseRandomEdgeWeights() const
        -> bool;

    auto getDiffusionModel() const
        -> DiffusionModel;

private:
    std::size_t reruns_;
    ParseMode parse_mode_;
    DiffusionModel diffusion_model_;
    bool print_raw_;
    int number_of_threads_;
    std::size_t number_of_seeds_;
    std::string graph_path_;
    bool use_inverse_graph_;
    bool backwards_activation_;
    bool random_edge_weights_;
    std::optional<std::string> out_path_;
    std::vector<std::string> solver_;
};

auto parseArguments(int argc, char* argv[])
    -> IMBProgramOptions;
