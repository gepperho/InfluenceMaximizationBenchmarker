#include <Benchmarker.hpp>
#include <execution>
#include <mutex>
#include <solver/IMM.hpp>

IMM::IMM(const Graph& graph) noexcept
    : graph_(graph) {}

auto IMM::solve(const std::size_t k) noexcept
    -> std::vector<NodeId>
{
    const int rr_batch_size = 1; // TODO change back to a higher number for runtime optimization after sufficient testing of the current implementation

    const double epsilon = 0.5; // epsilon and l are taken from the paper
    const double e = std::exp(1.);

    // Estimating Constants
    const auto n = graph_.getNumberOfNodes();
    const auto l = 1.; //(1. + std::log(2) / std::log(n));
    const auto epsilon_a = std::sqrt(2) * epsilon;
    const auto log_binom = logBinom(n, k);
    const auto log_n = std::log(n);
    const auto log_2n = std::log2(n);
    const auto log_2 = std::log(2);
    const auto alpha = std::sqrt(l * log_n + log_2); // eq 5
    const auto beta = std::sqrt((1 - 1 / e) * (log_binom + l * log_n + log_2)); // eq 5
    const auto lambda_star = 2. * n * std::pow(((1. - 1. / e) * alpha + beta), 2) / (epsilon * epsilon); // eq 6
    const auto lambda_a = (2. + 2. / 3. * epsilon_a)
        * (log_binom + l * log_n + std::log(log_2n))
        * n / (epsilon_a * epsilon_a); // eq 9


    dsfmt_t dsfmt;
    dsfmt_init_gen_rand(&dsfmt, rand());

    auto lower_bound = 1;
    double x;
    double theta_i;
    std::mutex mtx;

    // cache stuff
    std::vector<NodeId> result_set;
    double f_r;
    bool rr_sets_changed = false;

    // create rr sets and check for early termination
    for(int i = 1; i < log_2n - 1; ++i) {
        x = n / (std::pow(2., i));
        theta_i = lambda_a / x;
        rr_sets_.reserve(theta_i + rr_batch_size);
        auto size = rr_sets_.size();
        auto range = utils::range(size, size + 1 + (static_cast<std::int64_t>(theta_i) - size) / rr_batch_size);

        //could be flatted to only one loop i guess
        std::for_each(
            std::execution::par,
            std::begin(range),
            std::end(range),
            [&](auto /*y*/) {
                std::vector<std::vector<NodeId>> temp;
                temp.reserve(rr_batch_size);
                for(int j = 0; j < rr_batch_size; ++j) {
                    temp.emplace_back(createRrSet(dsfmt));
                }
                std::lock_guard lock{mtx};
                rr_sets_.insert(std::end(rr_sets_),
                                std::begin(temp),
                                std::end(temp));
            });

        // check early termination
        std::tie(result_set, f_r) = nodeSelection(k);
        if(n * f_r >= (1 + epsilon_a) * x) {
            lower_bound = n * f_r / (1 + epsilon_a);
            break;
        }
    }
    //fmt::print("Step 1 RR-sets: {}\n", rr_sets_.size());

    // create more rr sets, if necessary
    const auto theta = lambda_star / lower_bound;
    const auto size = rr_sets_.size();
    if(rr_sets_.size() < theta) {
        rr_sets_.reserve(theta + rr_batch_size);
        rr_sets_changed = true;
        auto range = utils::range(size, size + 1 + (static_cast<std::int64_t>(theta) - size) / rr_batch_size);

        std::for_each(
            std::execution::par,
            std::begin(range),
            std::end(range),
            [&](auto /*x*/) {
                std::vector<std::vector<NodeId>> temp;
                temp.reserve(rr_batch_size);
                for(int i = 0; i < rr_batch_size; ++i) {
                    temp.emplace_back(createRrSet(dsfmt));
                }

                std::lock_guard lock{mtx};
                rr_sets_.insert(rr_sets_.end(),
                                std::begin(temp),
                                std::end(temp));
            });
    }
    fmt::print("Step 2 RR-sets: {}\n", rr_sets_.size());

    // greedily select the k best nodes
    if(rr_sets_changed) {
        std::tie(result_set, f_r) = nodeSelection(k);
    }

    return result_set;
}

auto IMM::name() const noexcept
    -> std::string
{
    return "IMM";
}

auto IMM::createRrSet(dsfmt_t dsfmt) const noexcept
    -> std::vector<NodeId>
{
    //should be member
    std::vector visited(graph_.getNumberOfNodes(), false);

    const auto source_node = graph_.getRandomNode();
    std::vector<NodeId> rr_set;

    //this is named queue but is actually used as a stack
    //is this right?
    std::vector<NodeId> queue;
    queue.emplace_back(source_node);

    while(!queue.empty()) {
        const auto current_node = queue.back();
        queue.pop_back();
        if(visited[current_node]) {
            continue;
        }

        visited[current_node] = true;
        rr_set.emplace_back(current_node);
        for(auto edge : graph_.getInverseEdgesOf(current_node)) {

            auto rnd = dsfmt_genrand_close_open(&dsfmt);
            if(edge.getWeight() > rnd) {
                queue.emplace_back(edge.getDestination());
            }
        }
    }
    return rr_set;
}


//This seems to be used in multiple solvers it could be moved out as a util function
auto IMM::logBinom(const std::size_t n, const std::size_t k) const noexcept
    -> double
{
    double ans = 0;
    for(auto i = n - k + 1; i <= n; i++) {
        ans += log(i);
    }
    for(auto i = 1; i <= k; i++) {
        ans -= log(i);
    }
    return ans;
}

auto IMM::nodeSelection(std::size_t k) const noexcept
    -> std::pair<std::vector<NodeId>, double>
{
    //this should be a member of IMM with a reset function to only pay for what is used
    std::vector rr_set_already_covered(rr_sets_.size(), false);
    // coverage counter for the nodes
    std::vector<std::vector<std::size_t>> counter(graph_.getNumberOfNodes());
    std::vector<NodeId> result_set;

    // fill the coverage counter
    for(std::size_t i = 0; i < rr_sets_.size(); ++i) {
        for(auto current_node : rr_sets_[i]) {
            counter[current_node].emplace_back(i);
        }
    }

    // pick the best nodes
    while(result_set.size() < k) {

        // find max node
        NodeId max_id = 0;
        std::size_t max_size = 0;

        for(auto i = 0; i < graph_.getNumberOfNodes(); ++i) {
            if(counter[i].size() > max_size) {
                max_id = i;
                max_size = counter[i].size();
            }
        }

        // mark rr sets as covered
        for(auto rr_set_id : counter[max_id]) {
            rr_set_already_covered[rr_set_id] = true;

            // remove counter entries for covered rr sets
            for(auto member : rr_sets_[rr_set_id]) {
                //auto x = 0;
                if(member == max_id) {
                    continue;
                }
                counter[member].erase(
                    std::remove_if(counter[member].begin(),
                                   counter[member].end(),
                                   [rr_set_id](auto elem) {
                                       return elem == rr_set_id;
                                   }),
                    counter[member].end());
            }
        }
        counter[max_id].clear();
        result_set.emplace_back(max_id);
    }

    //why is the result of a count a double here?
    long influence = std::count_if(std::begin(rr_set_already_covered),
                                   std::end(rr_set_already_covered),
                                   [](auto x) { return x; });
    double f_r = static_cast<double>(influence) / static_cast<double>(rr_sets_.size());
    return {result_set, f_r};
}
