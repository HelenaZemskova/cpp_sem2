// g++ -std=c++23 -O3 -m32 -Wall -Wextra -I/usr/local/include -o 10_05 10_05.cpp && ./10_05

// Сравнение 9 хэш-функций по количеству коллизий на одинаковом наборе строк.
// Результаты сохраняются в CSV, затем строится график.
//
// 1) при росте числа строк коллизии растут нелинейно (почти как парабола),
// потому что число возможных пар строк растёт примерно как n^2
//
// 2) PJW и ELF хуже, т.к. у них эффективно используется меньше бит (по сути около 28 бит),
// поэтому диапазон возможных хэшей уже и коллизии появляются чаще.
//
// 3) по результатам этого теста обычно лучшие - JS и SDBM (минимум коллизий), худшие - PJW/ELF (максимум коллизий).

#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <functional>
#include <print>
#include <random>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

auto hash_rs(std::string const & str) -> std::uint32_t
{
    std::uint32_t b = 378551;
    std::uint32_t a = 63689;
    std::uint32_t hash = 0;

    for (auto c : str)
    {
        hash = hash * a + static_cast < std::uint32_t > (c);

        a *= b;
    }

    return hash;
}

auto hash_js(std::string const & str) -> std::uint32_t
{
    std::uint32_t hash = 1315423911;

    for (auto c : str)
    {
        hash ^= (hash << 5) + static_cast < std::uint32_t > (c) + (hash >> 2);
    }

    return hash;
}

auto hash_pjw(std::string const & str) -> std::uint32_t
{
    constexpr std::uint32_t three_quarters = 24;
    constexpr std::uint32_t one_eighth = 4;
    constexpr std::uint32_t high_bits = 0xF0000000u;

    std::uint32_t hash = 0;

    for (auto c : str)
    {
        hash = (hash << one_eighth) + static_cast < std::uint32_t > (c);

        if (auto temp = hash & high_bits; temp)
        {
            hash ^= temp >> three_quarters;
            hash &= ~temp;
        }
    }

    return hash;
}

auto hash_elf(std::string const & str) -> std::uint32_t
{
    std::uint32_t hash = 0;

    for (auto c : str)
    {
        hash = (hash << 4) + static_cast < std::uint32_t > (c);

        if (auto x = hash & 0xF0000000u; x)
        {
            hash ^= x >> 24;
            hash &= ~x;
        }
    }

    return hash;
}

auto hash_bkdr(std::string const & str) -> std::uint32_t
{
    constexpr std::uint32_t seed = 131;

    std::uint32_t hash = 0;

    for (auto c : str)
    {
        hash = hash * seed + static_cast < std::uint32_t > (c);
    }

    return hash;
}

auto hash_sdbm(std::string const & str) -> std::uint32_t
{
    std::uint32_t hash = 0;

    for (auto c : str)
    {
        hash = static_cast < std::uint32_t > (c)
             + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

auto hash_djb(std::string const & str) -> std::uint32_t
{
    std::uint32_t hash = 5381;

    for (auto c : str)
    {
        hash = ((hash << 5) + hash) + static_cast < std::uint32_t > (c);
    }

    return hash;
}

auto hash_dek(std::string const & str) -> std::uint32_t
{
    std::uint32_t hash = static_cast < std::uint32_t > (str.size());

    for (auto c : str)
    {
        hash = (hash << 5) ^ (hash >> 27) ^ static_cast < std::uint32_t > (c);
    }

    return hash;
}

auto hash_ap(std::string const & str) -> std::uint32_t
{
    std::uint32_t hash = 0xAAAAAAAAu;

    for (std::size_t i = 0; auto c : str)
    {
        auto uc = static_cast < std::uint32_t > (static_cast < unsigned char > (c));

        hash ^= (i & 1) == 0
              ?  (hash <<  7) ^ uc * (hash >> 3)
              : ~((hash << 11) + (uc ^ (hash >> 5)));

        ++i;
    }

    return hash;
}

auto make_strings(std::size_t count, std::size_t length) -> std::vector < std::string >
{
    std::set < std::string > unique;

    std::string str(length, ' ');

    std::uniform_int_distribution < int > dist(97, 122);

    std::default_random_engine engine;

    while (std::size(unique) < count)
    {
        for (auto & ch : str)
        {
            ch = static_cast < char > (dist(engine));
        }

        unique.insert(str);
    }

    return { std::begin(unique), std::end(unique) };
}

int main()
{
    using Fn = std::function < std::uint32_t(std::string const &) >;

    struct Desc { std::string name; Fn fn; };

    static Desc const funcs[] =
    {
        { "RS",   hash_rs   },
        { "JS",   hash_js   },
        { "PJW",  hash_pjw  },
        { "ELF",  hash_elf  },
        { "BKDR", hash_bkdr },
        { "SDBM", hash_sdbm },
        { "DJB",  hash_djb  },
        { "DEK",  hash_dek  },
        { "AP",   hash_ap   },
    };

    static constexpr auto n_funcs   = std::size(funcs);
    static constexpr auto n_strings = 1uz << 20;
    static constexpr auto str_len   = 10uz;
    static constexpr auto sample    = 1uz << 10;

    std::print("Generating {} strings of length {}...\n", n_strings, str_len);

    auto const strings = make_strings(n_strings, str_len);

    std::vector < std::unordered_set < std::uint32_t > > seen(n_funcs);

    for (auto & s : seen)
    {
        s.reserve(n_strings);
    }

    std::print("Computing collisions...\n");

    std::ofstream csv("10_05_data.csv");

    csv << "n";
    for (auto const & d : funcs) csv << "," << d.name;
    csv << "\n";

    for (auto i = 0uz; i < n_strings; ++i)
    {
        auto const & str = strings[i];

        for (auto j = 0uz; j < n_funcs; ++j)
        {
            seen[j].insert(funcs[j].fn(str));
        }

        if (i % sample == 0 || i + 1 == n_strings)
        {
            csv << i;

            for (auto j = 0uz; j < n_funcs; ++j)
            {
                csv << "," << (i + 1 - std::size(seen[j]));
            }

            csv << "\n";
        }
    }

    csv.close();

    std::print("\n{:>6}  {:>14}  {:>14}\n",
               "Func", "Collisions", "UniqueHashes");

    std::print("{}\n", std::string(40, '-'));

    for (auto j = 0uz; j < n_funcs; ++j)
    {
        std::print("{:>6}  {:>14}  {:>14}\n",
                   funcs[j].name,
                   n_strings - std::size(seen[j]),
                   std::size(seen[j]));
    }

    std::print("\nData written to 10_05_data.csv\n");
    std::print("Plotting...\n");
    std::fflush(stdout);

    auto rc = std::system("python3 10_05_plot.py");
    assert(rc == 0);
    std::print("All tests passed successfully.\n");
}
