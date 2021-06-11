#include <Parser.hpp>
#include <cmath>
#include <fmt/core.h>
#include <fstream>
#include <iostream>

namespace {

//extracts and erases the next integer from a string view
auto extractNextInteger(std::string_view& line) noexcept
    -> NodeId
{
    if(auto remove = line.find_first_of("0123456789");
       remove != std::string_view::npos) {
        line.remove_prefix(remove);
    }

    NodeId node = 0;
    auto remove_counter = 0;

    // this can be optimized much more
    for(char digit : line) {
        if(digit == ':' || digit == ',' || std::isspace(digit)) {
            break;
        }

        node *= 10;
        node += digit - '0';
        remove_counter++;
    }
    line.remove_prefix(remove_counter);

    return node;
}

auto parseVertexListLine(std::string_view line)
    -> std::tuple<NodeId, std::vector<Edge>>
{
    NodeId node = extractNextInteger(line);

    std::vector<Edge> edges;

    while(!line.empty()) {

        if(auto remove = line.find_first_of("0123456789");
           remove != std::string_view::npos) {
            line.remove_prefix(remove);
        } else {
            break;
        }


        NodeId neighbour = extractNextInteger(line);
        edges.emplace_back(neighbour, std::numeric_limits<float>::quiet_NaN());
    }

    return {node, edges};
}

auto parseEdgeListLine(std::string_view line)
    -> std::tuple<NodeId, NodeId, float>
{
    NodeId from = extractNextInteger(line);
    NodeId to = extractNextInteger(line);

    char* end;

    float weight = std::strtof(line.data(), &end);

    if(line.data() == end) {
        weight = NAN;
    }

    return std::tuple{from, to, weight};
}

} // namespace


auto parseVertexListFile(std::string_view path, bool inverse, bool contains_meta_data, bool should_log)
    -> Graph
{
    Graph graph{path.data()};

    std::ifstream input_file(path.data(), std::ios::in);

    if(!input_file) {
        fmt::print("File {} not found\n", path);
        std::exit(1);
    }

    std::string line;
    bool first = true;
    NodeId expected_node = 0;
    std::size_t missing = 0;

    while(std::getline(input_file, line)) {

        if(contains_meta_data) {
            contains_meta_data = false;
            continue;
        }

        auto [node, edges] = parseVertexListLine(line);

        if(first and node != 0 and should_log) {
            fmt::print("the edgelist file started with node {}, which is strange\n", node);
        }

        first = false;

        if(node != expected_node) {
            missing += node - expected_node;
        }
        expected_node = node + 1;

        graph.appendNode(node, edges);
    }

    graph.calculateBackwardEdges();
    if(inverse) {
        graph.inverse();
    }

    graph.calculateEdgeWeights();

    if(missing != 0 and should_log) {
        fmt::print("while parsing the graph, {} nodes were missing\n", missing);
    }

    return graph;
}

auto parseEdgeListFile(std::string_view path, bool inverse, bool contains_meta_data, bool should_log)
    -> Graph
{
    std::unordered_map<NodeId, std::vector<Edge>> adj_list;

    std::ifstream input_file(path.data(), std::ios::in);

    if(!input_file) {
        fmt::print("File {} not found\n", path);
        std::exit(1);
    }

    std::string line;

    NodeId max{0};

    while(std::getline(input_file, line)) {
        if(line.rfind('#', 0) == 0
           or line.rfind('%', 0) == 0
           or contains_meta_data) {
            contains_meta_data = false;
            continue;
        }

        auto [from, to, weight] = parseEdgeListLine(line);

        adj_list[from].emplace_back(to, weight);

        max = std::max(max, from);
        max = std::max(max, to);
    }

    Graph graph{path.data()};
    for(NodeId n{0}; n <= max; n++) {
        if(adj_list.find(n) == adj_list.end()) {
            graph.appendNode(n, {});
        } else {
            graph.appendNode(n, std::move(adj_list[n]));
        }
    }


    graph.calculateBackwardEdges();
    if(inverse) {
        graph.inverse();
    }
    graph.calculateEdgeWeights();

    if(should_log) {
        auto isolated = std::count_if(std::begin(utils::range(max)),
                                      std::end(utils::range(max)),
                                      [&](auto n) {
                                          return graph.getEdgesOf(n).empty()
                                              and graph.getInverseEdgesOf(n).empty();
                                      });

        if(isolated != 0) {
            fmt::print("graph was build with {} isolated nodes\n", isolated);
        }
    }

    return graph;
}
