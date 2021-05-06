#pragma once

#include <Graph.hpp>
#include <unordered_set>
namespace util {

using NodeId = std::int_fast64_t;

enum class DiffusionModel : std::size_t {
    INDEPENDENT_CASCADE = 0,
    LINEAR_THRESHOLD = 1
};

static auto diffusionModelToString(DiffusionModel m) -> std::string
{
    switch(m) {
    case DiffusionModel::INDEPENDENT_CASCADE:
        return "Independent Cascade";
    case DiffusionModel::LINEAR_THRESHOLD:
        return "Linear Threshold";
    default:
        // unknown type -> undefined behaviour
        exit(-1);
    }
}

static auto isNumber(const std::string& s) -> bool
{
    return std::all_of(std::begin(s),
                       std::end(s),
                       [](auto c) {
                           return std::isdigit(c);
                       });
}

template<class T>
auto extractParameter(std::string_view s) noexcept
    -> std::optional<T>
{
    try {

        if constexpr(std::is_same_v<T, int>) {
            return std::stoi(s.data());
        } else if constexpr(std::is_same_v<T, double>) {
            return std::stod(s.data());
        } else if constexpr(std::is_same_v<T, float>) {
            return std::stof(s.data());
        } else {
            static_assert("you are trying to extract an unknown parameter type");
        }

    } catch(...) {
        return std::nullopt;
    }
}

} // namespace util
