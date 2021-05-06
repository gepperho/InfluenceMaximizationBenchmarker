#include <Parser.hpp>
#include <fmt/core.h>
#include <fstream>
#include <sstream>

namespace {

auto parseVertexListLine(std::string_view line)
    -> std::tuple<NodeId, std::vector<Edge>>
{
    NodeId node;
    std::string neighbour_list;
    char separator;

    std::istringstream iss(line.data());
    iss >> node >> separator >> neighbour_list;

    // parse neighbours
    std::stringstream nss(neighbour_list);
    long neighbour;
    std::vector<Edge> edges;
    while(nss >> neighbour) {
        edges.emplace_back(neighbour);
        if(nss.peek() == ',') {
            nss.ignore();
        }
    }

    return {node, edges};
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
    bool skip = true;
    bool first = true;
    NodeId expected_node = 0;
    std::size_t missing = 0;

    while(std::getline(input_file, line)) {

        if(skip && contains_meta_data) {
            skip = false;
            continue;
        }

        auto [node, edges] = parseVertexListLine(line);

        if(first and node != 0 and should_log) {
            fmt::print("the edgelist file started with node {}, which is strange\n", node);
            first = false;
        }

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

    bool skip = true;
    bool first = true;

    NodeId last_node = 0;
    std::size_t missing = 0;

    while(std::getline(input_file, line)) {
        if(line.rfind('#', 0) == 0 || line.rfind('%', 0) == 0) {
            continue;
        }

        if(skip && contains_meta_data) {
            skip = false;
            continue;
        }


        NodeId from;
        NodeId to;

        std::istringstream iss(line);
        iss >> from >> to;

        if(first and from != 0 and should_log) {
            fmt::print("the edgelist file started with node {}, which is strange\n", from);
        }
        first = false;

        adj_list[from].emplace_back(to);

        // TODO fix: not every id appears on the left side (from)
        if(from > last_node + 1) {
            missing += from - (last_node + 1);
        }
        last_node = from;

        max = std::max(max, from);
        max = std::max(max, to);
    }

    if(missing != 0 and should_log) {
        fmt::print("while parsing the graph {} nodes were missing\n", missing);
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
    return graph;
}
