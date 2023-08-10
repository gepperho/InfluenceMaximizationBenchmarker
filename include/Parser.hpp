#pragma once

#include <Graph.hpp>
#include <string_view>

enum class ParseMode : std::size_t {
    VERTEX_LIST = 0,
    EDGE_LIST = 1
};

/**
 * reads a vertex list file and creates a graph of it
 * @param path path to the graph file in vertex list format
 * @param inverse if true, the edge directions are inverse
 * @param contains_meta_data if true, the first line (not containing starting with a comment sign) is skipped
 * @param random_edge_weights if true, the weights of the edges are randomly choosen from {0.1, 0.01, 0.001}
 * @param should_log if true, some information is printed during the parsing process
 * @return graph
 */
auto parseVertexListFile(std::string_view path,
                         bool inverse,
                         bool contains_meta_data,
                         bool random_edge_weights,
                         bool should_log)
    -> Graph;

/**
 * reads a edge list file and creates a graph of it
 * @param path path to the graph file in edge list format
 * @param inverse if true, the edge directions are inverse
 * @param contains_meta_data if true, the first line (not containing starting with a comment sign) is skipped
 * @param random_edge_weights if true, the weights of the edges are randomly choosen from {0.1, 0.01, 0.001}
 * @param should_log if true, some information is printed during the parsing process
 * @return graph
 */
auto parseEdgeListFile(std::string_view path,
                       bool inverse,
                       bool contains_meta_data,
                       bool random_edge_weights,
                       bool should_log)
    -> Graph;
