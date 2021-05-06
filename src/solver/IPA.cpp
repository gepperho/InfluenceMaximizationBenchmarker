#include <fmt/core.h>
#include <numeric>
#include <queue>
#include <solver/IPA.hpp>

IPASolver::IPASolver(const Graph& graph, int divider) noexcept
    : graph_(graph),
      spread_(graph.getNumberOfNodes()),
      divider_(divider)
{}


auto IPASolver::name() const noexcept
    -> std::string
{
    return "IPA";
}
auto IPASolver::solve(std::size_t k) noexcept
    -> std::vector<NodeId>
{
    std::vector<NodeId> nodes(graph_.getNumberOfNodes());
    std::iota(std::begin(nodes),
              std::end(nodes),
              0);

    double threshold = 1.0 / divider_;

    for(auto& current_node : nodes) {
        auto root = buildPathTree(current_node, threshold);
        spread_[current_node] = calculatePathTreeInfluence(root);
    }

    auto seed_comparison =
        [&](const auto& lhs, const auto& rhs) {
            if(spread_[lhs] == spread_[rhs]) {
                return updated_.find(rhs) != updated_.end();
            }
            return spread_[lhs] < spread_[rhs];
        };

    using SeedPriorityQueue = std::priority_queue<NodeId,
                                                  std::vector<NodeId>,
                                                  decltype(seed_comparison)>;

    SeedPriorityQueue pq(seed_comparison, nodes);

    // take first node, since currently all data is up to date and it really is the best one
    auto initial_top_node = pq.top();
    pq.pop();
    selected_seeds_.reserve(k);
    selected_seeds_.insert(initial_top_node);

    while(selected_seeds_.size() < k) {
        auto top_node = pq.top();
        pq.pop();

        if(updated_.find(top_node) != updated_.end()) {
            // current top node is up to date and thus can be added
            selected_seeds_.insert(top_node);
            updated_.clear();
            continue;
        }
        // current top node is outdated

        //auto n_tree = buildPathTree(top_node, threshold);
        auto original_influence = calculatePathForestInfluence(top_node, false, threshold);
        auto new_influence = calculatePathForestInfluence(top_node, true, threshold);
        double n_inc = new_influence - original_influence;
        spread_[top_node] = n_inc;
        updated_.insert(top_node);

        if(n_inc >= spread_[pq.top()]) {
            // add top_node
            selected_seeds_.insert(top_node);
            updated_.clear();
        } else {
            pq.push(top_node);
        }
    }

    std::vector<NodeId> result;
    result.reserve(k);
    std::copy(std::begin(selected_seeds_),
              std::end(selected_seeds_),
              std::back_inserter(result));

    return result;
}

auto IPASolver::calculatePathTreeInfluence(path_element& node) noexcept
    -> double
{
    // use a hashmap to 'build' the probabilities per end node
    std::unordered_map<NodeId, double> probabilities;
    traversePathTree(node, probabilities);
    // sum finalized probabilities: Sigma (1 - (1-p(v_1)) * (1-p(v_2)...)
    // the traversePathTree method yielded the product part for every reached node.
    return std::transform_reduce(
        std::cbegin(probabilities),
        std::cend(probabilities),
        0.0,
        [](auto acc, auto current) {
            // take the inverse probability to complete the formula (see above)
            return acc + 1 - current;
        },
        [](auto elem) {
            return elem.second;
        });
}

auto IPASolver::traversePathTree(const path_element& current_element,
                                 std::unordered_map<NodeId, double>& probabilities) noexcept
    -> void
{
    // probabilities are passes as reference, since this method is called recursively

    if(probabilities.find(current_element.element) != probabilities.end()) {
        // source has already an entry
        probabilities[current_element.element] *= (1. - current_element.probability);
    } else {
        // source has no entry yet, insert new
        probabilities[current_element.element] = 1. - current_element.probability;
    }

    // traverse children
    for(const auto& children : current_element.children) {
        traversePathTree(children, probabilities);
    }
}

auto IPASolver::buildPathTree(NodeId starting_node, double threshold) noexcept
    -> path_element
{
    path_element root_element = path_element(starting_node);
    root_element.parent = &root_element;

    expandPathTree(root_element, threshold, selected_seeds_);

    return root_element;
}

auto IPASolver::expandPathTree(path_element& current_path,
                               double threshold,
                               std::unordered_set<NodeId>& selected_seeds) noexcept
    -> void
{
    // TODO improve performance
    for(const auto& edge : graph_.getEdgesOf(current_path.element)) {
        auto next = edge.getDestination();

        if(selected_seeds.find(next) != selected_seeds.end()) {
            // blocker node reached
            continue;
        }

        if(edge.getWeight() * current_path.probability < threshold) {
            // threshold reached
            continue;
        }
        if(current_path.checkForCircle(next)) {
            // circle detected
            continue;
        }

        path_element child = path_element(next);
        child.parent = &current_path;
        child.probability = current_path.probability * edge.getWeight();
        expandPathTree(child, threshold, selected_seeds);
        current_path.children.emplace_back(child);
    }
}

auto IPASolver::calculatePathForestInfluence(NodeId additional_node,
											 bool use_additional,
											 double threshold) noexcept
  -> double
{
    std::unordered_set<NodeId> selected = selected_seeds_;
    if(use_additional) {
        selected.insert(additional_node);
    }

    return std::transform_reduce(
        std::begin(selected),
        std::end(selected),
        0.0,
        [](auto acc, auto current) {
            return acc + current;
        },
        [&](auto node) {
            path_element root_element = path_element(node);
            root_element.parent = &root_element;
            expandPathTree(root_element, threshold, selected);
            return calculatePathTreeInfluence(root_element);
        });
}
