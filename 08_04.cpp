#include <algorithm>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <vector>


constexpr auto target = std::string_view { "methinksitislikeaweasel" };
constexpr auto copies = 100u;
constexpr auto mutation_rate = 0.05;


auto metric(std::string const & candidate) -> int
{
    int diff = 0;

    for (auto i = 0uz; i < std::size(target); ++i)
        if (candidate[i] != target[i]) ++diff;

    return diff;
}


auto generate(std::size_t n, auto & engine) -> std::string
{
    std::uniform_int_distribution < int > letter('a', 'z');

    std::string s(n, ' ');

    for (auto & c : s) c = static_cast < char > (letter(engine));

    return s;
}

auto mutate(std::string const & parent, auto & engine) -> std::string
{
    std::uniform_real_distribution < double > probability(0.0, 1.0);
    std::uniform_int_distribution  < int > letter('a', 'z');

    std::string child = parent;

    for (auto & c : child)
        if (probability(engine) < mutation_rate)
            c = static_cast < char > (letter(engine));

    return child;
}
int main()
{
    std::random_device device;

    std::default_random_engine engine(device());

    auto current = generate(std::size(target), engine);

    for (auto iteration = 0u; ; ++iteration)
    {
        std::cout << "iteration " << iteration
                  << " : " << current
                  << " : metric = " << metric(current) << '\n';

        std::vector < std::string > population(copies);

        for (auto & copy : population) copy = mutate(current, engine);

        auto const & best = *std::min_element
        (
            std::begin(population), std::end(population),
            [](auto const & a, auto const & b) { return metric(a) < metric(b); }
        );

        if (metric(best) == 0)
        {
            std::cout << "iteration " << iteration + 1
                      << " : " << best
                      << " : metric = 0 - done!\n";
            break;
        }

        current = best;
    }
}
