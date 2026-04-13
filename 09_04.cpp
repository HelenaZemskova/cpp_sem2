// g++ -std=c++23 -Wall -Wextra -o 09_04 09_04.cpp && ./09_04


#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <vector>
#include <iostream>

template < typename It >
void order(It left, It right)
{
    if (std::distance(left, right) <= 1) return;

    for (auto i = std::next(left); i != right; ++i)
    {
        for (auto j = i; j != left; --j)
        {
            if (*std::prev(j) > *j)
                std::iter_swap(std::prev(j), j);
            else
                break;
        }
    }
}

template < typename It >
void place_median_to_end(It left, It right)
{
    auto last = std::prev(right);
    auto mid  = std::next(left, (std::distance(left, right) - 1) / 2);

    if (*left > *mid)  std::iter_swap(left, mid);
    if (*left > *last) std::iter_swap(left, last);
    if (*mid  > *last) std::iter_swap(mid,  last);

    std::iter_swap(mid, last);
}

template < typename It >
auto partition(It left, It right) -> It
{
    place_median_to_end(left, right);

    auto const last = std::prev(right);
    auto const pivot = *last;
    auto i = left;

    for (auto j = left; j != last; ++j)
    {
        if (*j < pivot)
        {
            std::iter_swap(i, j);
            std::advance(i, 1);
        }
    }

    std::iter_swap(i, last);
    return i;
}

template < typename It >
void quick_sort(It left, It right)
{
    if (std::distance(left, right) <= 1) return;

    if (std::distance(left, right) > 16)
    {
        auto pivot = partition(left, right);

        quick_sort(left, pivot);
        quick_sort(std::next(pivot), right);
    }
    else
    {
        order(left, right);
    }
}

template < typename It >
void sort_range(It first, It last)
{
    quick_sort(first, last);
}

int main()
{
    auto size = 1'000uz;

    std::vector < int > vector_int(size, 0);

    for (auto i = 0uz; i < size; ++i)
    {
        vector_int[i] = static_cast < int > (size - i);
    }

    sort_range(std::begin(vector_int), std::end(vector_int));

    assert(std::ranges::is_sorted(vector_int));

    std::vector < double > vector_double = {5.5, 2.2, 8.8, 1.1, 9.9, 3.3};

    sort_range(std::begin(vector_double), std::end(vector_double));

    assert(std::ranges::is_sorted(vector_double));

    std::vector < char > vector_char = {'z', 'a', 'm', 'b', 'y', 'c'};

    sort_range(std::begin(vector_char), std::end(vector_char));

    assert(std::ranges::is_sorted(vector_char));

    std::vector < std::string > vector_string = {"zebra", "apple", "mango", "banana"};

    sort_range(std::begin(vector_string), std::end(vector_string));

    assert(std::ranges::is_sorted(vector_string));

    std::vector < int > empty;
    sort_range(std::begin(empty), std::end(empty));
    assert(std::ranges::is_sorted(empty));

    std::vector < int > one = {42};
    sort_range(std::begin(one), std::end(one));
    assert((one == std::vector < int > {42}));

    std::vector < int > duplicates(40, 5);
    sort_range(std::begin(duplicates), std::end(duplicates));
    assert(std::ranges::is_sorted(duplicates));
    assert((duplicates == std::vector < int > (40, 5)));

    std::vector < int > mix = {3, 1, 3, 2, 3, 0, 3, 2, 1, 3};
    sort_range(std::begin(mix), std::end(mix));
    assert(std::ranges::is_sorted(mix));
    std::cout << "All tests passed successfully.\n";
}
