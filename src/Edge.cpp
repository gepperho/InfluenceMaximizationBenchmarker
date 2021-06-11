#include <Edge.hpp>

Edge::Edge(NodeId destination)
    : destination_(destination) {}

Edge::Edge(NodeId destination,
           float weight)
    : destination_(destination),
      weight_(weight) {}


auto Edge::getDestination() const
    -> NodeId
{
    return destination_;
}

auto Edge::operator==(const Edge& rhs) const
    -> bool
{
    return destination_ == rhs.destination_;
}

auto Edge::operator!=(const Edge& rhs) const
    -> bool
{
    return !(*this == rhs);
}
auto Edge::getWeight() const -> float
{
    return weight_;
}
auto Edge::setWeight(float weight) -> void
{
    weight_ = weight;
}
