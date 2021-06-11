#pragma once

#include <Edge.hpp>
#include <cstdint>
#include <dSFMT.h>
#include <nonstd/span.hpp>
#include <util/Range.hpp>
#include <vector>

using NodeId = std::int_fast64_t;

class Graph
{
public:
    Graph(Graph&&) = default;
    Graph(const Graph&) = delete;
    Graph(std::string path);

    auto operator=(Graph&&) -> Graph& = default;
    auto operator=(const Graph&) -> Graph& = default;

    auto getEdgesOf(NodeId node) const
        -> nonstd::span<const Edge>;

    auto getInverseEdgesOf(NodeId node) const
        -> nonstd::span<const Edge>;

    auto getOutDegreeOf(NodeId node) const
        -> std::int64_t;

    auto getInDegreeOf(NodeId node) const
        -> std::int64_t;

    auto getNumberOfNodes() const
        -> std::int64_t;

    auto getNumberOfEdges() const
        -> std::int64_t;

    auto getGraphName() const
        -> std::string_view;

    auto inverse()
        -> void;

    auto getNodes() const
        -> utils::impl::RangeWrapper<std::int64_t, true>;

    auto calculateEdgeWeights() -> void;

    auto getRandomNode() const noexcept
        -> NodeId;

private:
    friend auto parseVertexListFile(std::string_view path, bool inverse, bool contains_meta_data, bool should_log)
        -> Graph;

    friend auto parseEdgeListFile(std::string_view path, bool inverse, bool contains_meta_data, bool should_log)
        -> Graph;

    /**
     * THIS IS NOT A NORMAL ADD_NODE METHOD
     *
     * id muss be current_max_id + 1
     * otherwise everything goes to shit
     * don't use this method unless you are sure what you do
     *
     *
     * @param node next node with index: current_max_index + 1
     * @param edges adjacent edges
     */
    auto appendNode(NodeId node, std::vector<Edge> edges)
        -> void;

    /**
	 * This method needs to be called on a fully build graph in order
	 * have the backward edges available
	 */
    auto calculateBackwardEdges()
        -> void;


    mutable dsfmt_t dsfmt_;
    std::vector<std::size_t> forward_offset_array_;
    std::vector<std::size_t> backward_offset_array_;
    std::vector<Edge> forward_edges_;
    std::vector<Edge> backward_edges_;
    // todo store during parsing
    std::string graph_name_;
};
