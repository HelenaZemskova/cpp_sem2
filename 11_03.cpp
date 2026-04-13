// g++ -std=c++23 -Wall -Wextra -o 11_03 11_03.cpp && ./11_03


#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cmath>
#include <functional>
#include <iterator>
#include <print>
#include <string>
#include <vector>

template < typename It, typename Cmp >
void order(It left, It right, Cmp cmp)
{
    if (std::distance(left, right) <= 1) return;

    for (auto i = std::next(left); i != right; ++i)
    {
        for (auto j = i; j != left; --j)
        {
            if (cmp(*j, *std::prev(j)))
                std::iter_swap(std::prev(j), j);
            else
                break;
        }
    }
}

template < typename It, typename Cmp >
void median_of_three(It left, It right, Cmp cmp)
{
    auto last = std::prev(right);
    auto mid  = std::next(left, (std::distance(left, right) - 1) / 2);

    if (cmp(*mid,  *left)) std::iter_swap(left, mid);
    if (cmp(*last, *left)) std::iter_swap(left, last);
    if (cmp(*last, *mid)) std::iter_swap(mid,  last);

    std::iter_swap(mid, last);
}

template < typename It, typename Cmp >
auto partition_range(It left, It right, Cmp cmp) -> It
{
    median_of_three(left, right, cmp);
    auto const last = std::prev(right);
    auto const pivot = *last;
    auto i = left;

    for (auto j = left; j != last; ++j)
    {
        if (cmp(*j, pivot))
        {
            std::iter_swap(i, j);
            std::advance(i, 1);
        }
    }

    std::iter_swap(i, last);
    return i;
}

template < typename It, typename Cmp >
void quick_sort(It left, It right, Cmp cmp)
{
    if (std::distance(left, right) <= 1) return;

    if (std::distance(left, right) > 16)
    {
        auto pivot = partition_range(left, right, cmp);

        quick_sort(left,              pivot, cmp);
        quick_sort(std::next(pivot), right, cmp);
    }
    else
    {
        order(left, right, cmp);
    }
}

template < typename It, typename Cmp = std::less <> >
void sort_range(It first, It last, Cmp cmp = Cmp{})
{
    quick_sort(first, last, cmp);
}

bool compare_abs(int a, int b)
{
    return std::abs(a) < std::abs(b);
}

int main()
{

    {
        std::vector < int > v = { 5, 1, 4, 2, 3 };

        sort_range(std::begin(v), std::end(v));

        assert(std::ranges::is_sorted(v));

        std::print("default (std::less<>) : ");
        for (auto x : v) std::print("{} ", x);
        std::print("\n");
    }

    {
        std::vector < int > v = { -5, 3, -1, 4, -2 };

        sort_range(std::begin(v), std::end(v), compare_abs);

        std::print("free function (compare_abs) : ");
        for (auto x : v) std::print("{} ", x);
        std::print("\n");

        assert(std::ranges::is_sorted(v, compare_abs));
    }

    {
        std::vector < std::string > v = { "zebra", "apple", "mango", "banana" };

        sort_range(std::begin(v), std::end(v), std::less <> {});

        assert(std::ranges::is_sorted(v, std::less <> {}));

        std::print("std::less<> (strings ascending) : ");
        for (auto const & s : v) std::print("{} ", s);
        std::print("\n");
    }

    {
        std::vector < double > v = { 5.5, 2.2, 8.8, 1.1, 9.9, 3.3 };

        sort_range(std::begin(v), std::end(v), std::greater <> {});

        assert(std::ranges::is_sorted(v, std::greater <> {}));

        std::print("std::greater<> (doubles descending) : ");
        for (auto x : v) std::print("{} ", x);
        std::print("\n");
    }

    {
        std::vector < std::string > v =
        {
            "zebra", "ox", "elephant", "cat", "dog", "ant"
        };

        auto by_length_desc = [](std::string const & a, std::string const & b)
        {
            if (a.size() != b.size())
                return a.size() > b.size();

            return a < b;
        };

        sort_range(std::begin(v), std::end(v), by_length_desc);

        assert(std::ranges::is_sorted(v, by_length_desc));

        std::print("lambda (by length desc, alpha tie) : ");
        for (auto const & s : v) std::print("{} ", s);
        std::print("\n");
    }

    {
        std::vector < char > v = { 'z', 'a', 'm', 'b', 'y', 'c' };

        sort_range(std::begin(v), std::end(v), [](char a, char b) { return a > b; });

        assert(std::ranges::is_sorted(v, std::greater < char > {}));

        std::print("lambda (chars descending) : ");
        for (auto x : v) std::print("{} ", x);
        std::print("\n");
    }

    {
        auto size = 1'000uz;

        std::vector < int > v(size);

        for (auto i = 0uz; i < size; ++i) v[i] = static_cast < int > (size - i);

        sort_range(std::begin(v), std::end(v));
        assert(std::ranges::is_sorted(v));

        sort_range(std::begin(v), std::end(v), std::greater <> {});
        assert(std::ranges::is_sorted(v, std::greater <> {}));

        std::print("large vector (1000 elements) : ok\n");
    }

    {
        std::vector < int > v(40, 5);
        sort_range(std::begin(v), std::end(v));
        assert(std::ranges::is_sorted(v));
    }
    std::print("All tests passed successfully.\n");
}
