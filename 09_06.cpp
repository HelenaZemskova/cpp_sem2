// g++ -std=c++23 -Wall -Wextra -o 09_06 09_06.cpp && ./09_06


#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <iterator>

#include <boost/iterator/iterator_categories.hpp>
#include <boost/iterator/iterator_facade.hpp>

class Iterator_v1
{
public :

    using iterator_category = std::forward_iterator_tag;
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using pointer = int const *;
    using reference = int;

    Iterator_v1() : m_x(0), m_y(1) {}

    auto const operator++(int)
    {
        auto x = *this;

        step();

        return x;
    }

    auto & operator++()
    {
        step();

        return *this;
    }

    auto operator*() const { return m_y; }

    friend auto operator==(Iterator_v1 const & lhs, Iterator_v1 const & rhs)
    {
        return lhs.m_x == rhs.m_x && lhs.m_y == rhs.m_y;
    }

private :

    void step()
    {
        m_x += m_y;

        std::swap(m_x, m_y);
    }

    int m_x;
    int m_y;
};

class Iterator_v2 : public boost::iterator_facade
<
    Iterator_v2, int, boost::forward_traversal_tag, int const &
>
{
public :

    Iterator_v2() : m_x(0), m_y(1) {}

private :

    friend boost::iterator_core_access;

    void increment()
    {
        m_x += m_y;

        std::swap(m_x, m_y);
    }

    int const & dereference() const { return m_y; }

    bool equal(Iterator_v2 const & other) const
    {
        return m_x == other.m_x && m_y == other.m_y;
    }

    int m_x;
    int m_y;
};

int main()
{
    constexpr auto n = 10;

    constexpr std::array < int, 10 > expected = { 1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };

    {
        std::array < int, 10 > got = {};

        auto it = Iterator_v1();

        for (auto i = 0; i < n; ++i, ++it)
        {
            got[i] = *it;
        }

        assert(got == expected);
    }

    {
        std::array < int, 10 > got = {};

        auto it = Iterator_v2();

        for (auto i = 0; i < n; ++i, ++it)
        {
            got[i] = *it;
        }

        assert(got == expected);
    }

    {
        auto it = Iterator_v1();
        auto old = it++;

        assert(*old == 1);
        assert(*it == 1);
    }

    {
        std::array < int, 10 > via_v1 = {}, via_v2 = {};

        std::copy_n(Iterator_v1(), n, via_v1.begin());
        std::copy_n(Iterator_v2(), n, via_v2.begin());

        assert(via_v1 == via_v2);
    }

    auto const end_v1 = std::next(Iterator_v1(), n);
    auto const end_v2 = std::next(Iterator_v2(), n);

    std::cout << "Iterator_v1 : ";
    for (auto it = Iterator_v1(); it != end_v1; ++it) std::cout << *it << " ";
    std::cout << "\n";

    std::cout << "Iterator_v2 : ";
    for (auto it = Iterator_v2(); it != end_v2; ++it) std::cout << *it << " ";
    std::cout << "\n";

    std::cout << "copy_n      : ";
    std::copy_n(Iterator_v1(), n, std::ostream_iterator < int > (std::cout, " "));
    std::cout << "\n";
    std::cout << "All tests passed successfully.\n";
}
