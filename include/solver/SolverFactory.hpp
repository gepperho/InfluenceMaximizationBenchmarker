
#pragma once

#include <Graph.hpp>
#include <memory>
#include <solver/SolverInterface.hpp>

class SolverFactory
{
public:
    /**
     * Takes the algorithm arguments, creates the solvers and benchmarks them.
     * @param arguments vertex list holding the algorithms (and their parameters)
     * @param graph
     * @param bm Benchmarker object
     * @return
     */
    static auto benchmarkAll(const std::vector<std::string>& arguments, Graph& graph, Benchmarker& bm) noexcept
        -> void;
};
