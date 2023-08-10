#pragma once

#include <Graph.hpp>
#include <fmt/core.h>
#include <unordered_set>
namespace util {

using NodeId = std::int_fast64_t;

enum class DiffusionModel : std::size_t {
    INDEPENDENT_CASCADE = 0,
    LINEAR_THRESHOLD = 1
};

inline auto diffusionModelToString(DiffusionModel m)
    //we can return an non owing string view here instead of an owing std::string
    //this is the case because the returned expressions are string literals which have static storrage duration
    //to read more about storrage duration have a look here: https://en.cppreference.com/w/cpp/language/storage_duration
    -> std::string_view
{
    switch(m) {
    case DiffusionModel::INDEPENDENT_CASCADE:
        return "Independent Cascade";
    case DiffusionModel::LINEAR_THRESHOLD:
        return "Linear Threshold";
    default:
        fmt::print("somehow an object of type DiffusionModel had a value which was not 0 or 1...\exiting...\n");
        std::exit(-1);
    }
}

inline auto isNumber(const std::string& s) -> bool
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
            //this fails at compiletime if T is a type we dont know how to extract
            static_assert("you are trying to extract an unknown parameter type");
        }

    } catch(...) {
        return std::nullopt;
    }
    return std::nullopt;
}

} // namespace util
