#pragma once

#include <cstdint>

using NodeId = std::int_fast64_t;

class Edge
{
public:
    Edge(NodeId destination);
    Edge(NodeId destination, float weight);
    Edge() = delete;
    Edge(const Edge&) = default;
    Edge(Edge&&) = default;

    auto operator=(const Edge&) -> Edge& = default;
    auto operator=(Edge&&) -> Edge& = default;

    auto operator==(const Edge&) const -> bool;
    auto operator!=(const Edge&) const -> bool;

    auto getDestination() const
        -> NodeId;

    auto getWeight() const -> float;

    auto setWeight(float weight) -> void;

private:
    NodeId destination_;
    float weight_;
};
