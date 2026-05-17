// g++ -std=c++23 -Wall -Wextra -pedantic 11_05.cpp -o 11_05

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iterator>
#include <numeric>
#include <print>
#include <random>
#include <ranges>
#include <string>
#include <vector>

template < typename In, typename Out, typename Pred, typename Proj >
auto transform_if(In const & in, Out out, Pred pred, Proj proj)
{
    std::vector < std::ranges::range_value_t < In > > temp;

    std::ranges::copy_if(in, std::back_inserter(temp), pred);

    return std::ranges::transform(temp, out, proj);
}

auto mae(auto const & actual, auto const & predicted)
{
    return std::transform_reduce
    (
        std::begin(actual), std::end(actual),
        std::begin(predicted),
        0.0,
        std::plus < double > {},
        [](auto a, auto b){ return std::abs(a - b); }
    ) / std::ranges::size(actual);
}

auto mse(auto const & actual, auto const & predicted)
{
    return std::transform_reduce
    (
        std::begin(actual), std::end(actual),
        std::begin(predicted),
        0.0,
        std::plus < double > {},
        [](auto a, auto b){ auto d = a - b; return d * d; }
    ) / std::ranges::size(actual);
}

class Fibonacci : public std::ranges::view_interface < Fibonacci >
{
public :

    Fibonacci() = default;

    auto begin() const { return Iterator{}; }

    auto end () const { return std::unreachable_sentinel; }

private :

    class Iterator
    {
    public :

        using iterator_category = std::forward_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = int const *;
        using reference = int;


        Iterator() = default;

        auto const operator++(int)
        {
            auto prev = *this;

            step();

            return prev;
        }

        auto & operator++()
        {
            step();

            return *this;
        }

        auto operator*() const { return m_y; }

        friend auto operator==(Iterator const & lhs, Iterator const & rhs)
        {
            return lhs.m_x == rhs.m_x && lhs.m_y == rhs.m_y;
        }

    private :

        void step()
        {
            m_x += m_y;

            std::swap(m_x, m_y);
        }

        int m_x = 0;
        int m_y = 1;
    };
};

int main()
{

    {
        std::vector < int > v = { 1, 2, 3, 2, 1 };

        std::ranges::replace(v, 2, 99);

        assert((v == std::vector < int > { 1, 99, 3, 99, 1 }));

        std::print("replace: ");
        for (auto x : v) std::print("{} ", x);
        std::print("\n");
    }

    {
        std::vector < int > v(5, 0);

        std::ranges::fill(v, 42);

        assert(std::ranges::all_of(v, [](int x){ return x == 42; }));

        std::print("fill: ");
        for (auto x : v) std::print("{} ", x);
        std::print("\n");
    }

    {
        std::vector < int > v = { 1, 1, 2, 3, 3, 3, 4 };

        auto [first, last] = std::ranges::unique(v);

        v.erase(first, last);

        assert((v == std::vector < int > { 1, 2, 3, 4 }));

        std::print("unique: ");
        for (auto x : v) std::print("{} ", x);
        std::print("\n");
    }

    {
        std::vector < int > v = { 1, 2, 3, 4, 5 };

        std::ranges::rotate(v, std::next(std::begin(v), 2));

        assert((v == std::vector < int > { 3, 4, 5, 1, 2 }));

        std::print("rotate: ");
        for (auto x : v) std::print("{} ", x);
        std::print("\n");
    }


    {
        std::vector < int > v = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        std::vector < int > out;

        std::ranges::sample(v, std::back_inserter(out), 4, std::default_random_engine{ 42 });

        assert(std::size(out) == 4);

        std::print("sample: ");
        for (auto x : out) std::print("{} ", x);
        std::print("\n");
    }

    {
        std::vector < int > v = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

        std::vector < int > result;

        transform_if
        (
            v,
            std::back_inserter(result),
            [](int x){ return x % 2 == 0; },
            [](int x){ return x * x; }
        );

        assert((result == std::vector < int > { 4, 16, 36, 64, 100 }));

        std::print("transform_if: ");
        for (auto x : result) std::print("{} ", x);
        std::print("\n");
    }

    {
        std::vector < double > actual    = { 1.0, 2.0, 3.0, 4.0, 5.0 };
        std::vector < double > predicted = { 1.5, 2.5, 2.5, 4.5, 4.5 };

        auto mae_value = mae(actual, predicted);
        auto mse_value = mse(actual, predicted);

        assert(std::abs(mae_value - 0.5 ) < 1e-9);
        assert(std::abs(mse_value - 0.25) < 1e-9);

        std::print("mae: {}\n", mae_value);
        std::print("mse: {}\n", mse_value);
    }

    {
        std::vector < int > v = { 1, 2, 3, 4, 5, 6 };

        std::vector < int > result;

        for (auto x : v | std::views::filter([](int x){ return x % 2 != 0; }))
            result.push_back(x);

        assert((result == std::vector < int > { 1, 3, 5 }));

        std::print("filter: ");
        for (auto x : result) std::print("{} ", x);
        std::print("\n");
    }

    {
        std::vector < int > v = { 1, 2, 3, 4, 5 };

        std::vector < int > result;

        for (auto x : v | std::views::drop(2))
            result.push_back(x);

        assert((result == std::vector < int > { 3, 4, 5 }));

        std::print("drop: ");
        for (auto x : result) std::print("{} ", x);
        std::print("\n");
    }

    {
        std::vector < std::vector < int > > v = { { 1, 2 }, { 3, 4 }, { 5 } };

        std::vector < int > result;

        for (auto x : v | std::views::join)
            result.push_back(x);

        assert((result == std::vector < int > { 1, 2, 3, 4, 5 }));

        std::print("join: ");
        for (auto x : result) std::print("{} ", x);
        std::print("\n");
    }

    {
        std::vector < int > v1 = { 1, 2, 3 };
        std::vector < std::string > v2 = { "a", "b", "c" };

        std::vector < std::pair < int, std::string > > result;

        for (auto [x, y] : std::views::zip(v1, v2))
            result.emplace_back(x, y);

        assert(result.size() == 3);
        assert((result[0] == std::pair < int, std::string > { 1, "a" }));

        std::print("zip: ");
        for (auto const & [x, y] : result) std::print("({},{}) ", x, y);
        std::print("\n");
    }

    {
        std::vector < int > v = { 1, 2, 3, 4, 5, 6 };

        std::vector < int > result;

        for (auto x : v | std::views::stride(2))
            result.push_back(x);

        assert((result == std::vector < int > { 1, 3, 5 }));

        std::print("stride: ");
        for (auto x : result) std::print("{} ", x);
        std::print("\n");
    }

    {
        constexpr auto n = 10;

        constexpr std::array < int, 10 > expected = { 1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };

        std::vector < int > result;

        for (auto x : Fibonacci{} | std::views::take(n))
            result.push_back(x);

        assert(std::ranges::equal(result, expected));

        std::print("fibonacci: ");
        for (auto x : result) std::print("{} ", x);
        std::print("\n");
    }

    std::print("All tests passed\n");
}
