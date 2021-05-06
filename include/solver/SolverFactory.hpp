
#pragma once

#include <solver/SolverInterface.hpp>

class SolverFactory
{
public:
    /**
     * Takes the algorithm argument and creates the according solvers.
     * @param arguments vertex list holding the algorithms (and their parameters) to be executed later.
     * @param graph
     * @return
     */
    static auto create(const std::vector<std::string>& arguments, Graph& graph) noexcept
        -> std::vector<std::unique_ptr<SolverInterface>>;
};
