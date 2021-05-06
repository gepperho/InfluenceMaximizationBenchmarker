#include <Graph.hpp>

Graph::Graph(std::string path)
    : graph_name_(std::move(path))
{
    forward_offset_array_.push_back(0);
    backward_offset_array_.push_back(0);
}

auto Graph::getEdgesOf(NodeId node) const
    -> nonstd::span<const Edge>
{
    auto start_offset = forward_offset_array_[node];
    auto end_offset = forward_offset_array_.at(static_cast<unsigned long>(node + 1));
    const auto* start = &forward_edges_[start_offset];
    const auto* end = &forward_edges_[end_offset];

    return {start, end};
}

auto Graph::getInverseEdgesOf(NodeId node) const
    -> nonstd::span<const Edge>
{
    auto start_offset = backward_offset_array_[node];
    auto end_offset = backward_offset_array_.at(static_cast<unsigned long>(node + 1));
    const auto* start = &backward_edges_[start_offset];
    const auto* end = &backward_edges_[end_offset];

    return {start, end};
}

auto Graph::getOutDegreeOf(NodeId node) const
    -> std::int64_t
{
    return getEdgesOf(node).size();
}

auto Graph::getInDegreeOf(NodeId node) const
    -> std::int64_t
{
    return getInverseEdgesOf(node).size();
}

auto Graph::getNumberOfNodes() const
    -> std::int64_t
{
    return forward_offset_array_.size() - 1;
}

auto Graph::getNumberOfEdges() const
    -> std::int64_t
{
    return forward_edges_.size();
}

auto Graph::getGraphName() const -> std::string_view
{
    return graph_name_;
}

auto Graph::appendNode(NodeId /*node*/, std::vector<Edge> edges)
    -> void
{
    std::move(std::begin(edges),
              std::end(edges),
              std::back_inserter(forward_edges_));

    forward_offset_array_.push_back(forward_edges_.size());
}


auto Graph::calculateBackwardEdges()
    -> void
{
    auto number_of_nodes = getNumberOfNodes();

    std::vector<std::vector<Edge>> adjacency_list(static_cast<unsigned long>(number_of_nodes));

    // calculate the back-edge adjacency list
    for(NodeId from{0}; from < number_of_nodes; from++) {
        auto edges = getEdgesOf(from);
        for(auto e : edges) {
            auto to = e.getDestination();
            Edge back_e{from};
            adjacency_list[to].push_back(back_e);
        }
    }

    // adjacency list to offset array
    for(NodeId n{0}; n < number_of_nodes; n++) {
        auto edges = std::move(adjacency_list[n]);

        std::move(std::begin(edges),
                  std::end(edges),
                  std::back_inserter(backward_edges_));

        backward_offset_array_.push_back(backward_edges_.size());
    }
}

auto Graph::inverse()
    -> void
{
    //activate ADL, this is best practice when using swap
    using std::swap;

    swap(forward_offset_array_,
         backward_offset_array_);

    swap(forward_edges_,
         backward_edges_);
}


auto Graph::getNodes() const
    -> utils::impl::RangeWrapper<std::int64_t, true>
{
    return utils::range(getNumberOfNodes());
}


auto Graph::calculateEdgeWeights() -> void
{
    // todo implement different diffusion probabilities
    // iterate edges for the forward edge list
    for(auto& edge : forward_edges_) {
        edge.setWeight(1.0f / getInDegreeOf(edge.getDestination()));
    }

    // iterate nodes and insert their weight into their backwards edges
    // size -1 because the offset array needs an additional "end element"
    for(NodeId current_node = 0; current_node < static_cast<NodeId>(backward_offset_array_.size() - 1); ++current_node) {
        auto weight = 1.0f / getInDegreeOf(current_node);
        auto edge_pointer = backward_offset_array_[current_node];
        auto edge_end = backward_offset_array_[current_node + 1];
        for(; edge_pointer < edge_end; ++edge_pointer) {
            backward_edges_[edge_pointer].setWeight(weight);
        }
    }
}
