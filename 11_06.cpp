// g++ -std=c++23 -Wall -Wextra -pedantic 11_06.cpp -o 11_06

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

#include <boost/graph/adjacency_matrix.hpp>

constexpr auto n = 10;

using graph_t = boost::adjacency_matrix
<
    boost::undirectedS,
    boost::no_property,
    boost::property < boost::edge_weight_t, int >
>;

int main()
{
    std::random_device device;

    std::default_random_engine engine(device());

    std::uniform_int_distribution < int > distribution(1, 10);

    graph_t graph(n);
    std::vector < std::pair < int, int > > edges;

    for (auto i = 0; i < n; ++i)
        for (auto j = i + 1; j < n; ++j)
        {
            boost::add_edge(i, j, distribution(engine), graph);
            edges.emplace_back(i, j);
        }

    auto weights = boost::get(boost::edge_weight, graph);

    std::cout << "Incidence matrix:\n\n";

    for (auto e = 0uz; e < edges.size(); ++e)
        std::cout << std::setw(2) << e << " ";

    std::cout << "\n\n";

    for (auto i = 0; i < n; ++i)
    {
        std::cout << std::setw(3) << i << " ";

        for (auto const & [from, to] : edges)
        {
            auto const incidence = (from == i || to == i) ? 1 : 0;
            std::cout << std::setw(2) << incidence << " ";
        }

        std::cout << "\n";
    }

    std::cout << "\nAdjacency matrix:\n\n";

    for (auto j = 0; j < n; ++j)
        std::cout << std::setw(3) << j;

    std::cout << "\n\n";

    for (auto i = 0; i < n; ++i)
    {
        std::cout << std::setw(3) << i;

        for (auto j = 0; j < n; ++j)
        {
            if (i == j)
                std::cout << std::setw(3) << 0;
            else
                std::cout << std::setw(3) << weights[boost::edge(i, j, graph).first];
        }

        std::cout << "\n";
    }

    std::vector < int > tour(n - 1);
    std::iota(std::begin(tour), std::end(tour), 1);

    auto best_cost = std::numeric_limits < int > ::max();
    auto best_tour = tour;

    do
    {
        auto cost = 0;
        auto prev = 0;

        for (auto v : tour)
        {
            cost += weights[boost::edge(prev, v, graph).first];
            prev = v;
        }

        cost += weights[boost::edge(prev, 0, graph).first];

        if (cost < best_cost)
        {
            best_cost = cost;
            best_tour = tour;
        }
    }
    while (std::next_permutation(std::begin(tour), std::end(tour)));


    assert(boost::num_edges(graph) == static_cast < std::size_t > (n * (n - 1) / 2));

    assert(best_cost > 0 && best_cost < std::numeric_limits < int > ::max());

    {
        auto sorted = best_tour;
        std::ranges::sort(sorted);
        assert((sorted == std::vector < int > { 1, 2, 3, 4, 5, 6, 7, 8, 9 }));
    }

    {
        auto cost = 0;
        auto prev = 0;

        for (auto v : best_tour)
        {
            cost += weights[boost::edge(prev, v, graph).first];
            prev = v;
        }

        cost += weights[boost::edge(prev, 0, graph).first];

        assert(cost == best_cost);
    }

    std::cout << "\nOptimal tour: 0";
    for (auto v : best_tour) std::cout << " -> " << v;
    std::cout << " -> 0\n";

    std::cout << "Total cost: " << best_cost << "\n";
    std::cout << "All tests passed\n";
}
