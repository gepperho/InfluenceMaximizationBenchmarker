
#pragma once

#include <IMBProgramOptions.hpp>
#include <diffusion/DiffusionModelInterface.hpp>
#include <diffusion/IndependentCascade.hpp>
#include <diffusion/LinearThreshold.hpp>

class DiffusionFactory
{
public:
    static auto create(DiffusionModel type, const Graph& graph) noexcept
        -> std::unique_ptr<DiffusionModelInterface>
    {
        switch(type) {
        case DiffusionModel::LINEAR_THRESHOLD:
            return std::make_unique<LinearThresholdEvaluation>(graph);
        case DiffusionModel::INDEPENDENT_CASCADE:
            return std::make_unique<IndependentCascader>(graph);
        default:
            return nullptr;
        }
    }
};
