#pragma once

#include <Graph.hpp>
#include <string_view>

enum class ParseMode : std::size_t {
    VERTEX_LIST = 0,
    EDGE_LIST = 1
};

/**
 * reads a file and creates a graph of it
 * @param path path to the graph file in vertex list format
 * @param inverse if true, the edge directions are inverse
 * @param contains_meta_data if true, the first line (not containing starting with a comment sign) is skipped
 * @return graph
 */
auto parseVertexListFile(std::string_view path, bool inverse, bool contains_meta_data, bool should_log)
    -> Graph;

/**
 * reads a file and creates a graph of it
 * @param path path to the graph file in edge list format
 * @param inverse if true, the edge directions are inverse
 * @param contains_meta_data if true, the first line (not containing starting with a comment sign) is skipped
 * @return graph
 */
auto parseEdgeListFile(std::string_view path, bool inverse, bool contains_meta_data, bool should_log)
    -> Graph;
