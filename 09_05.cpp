// g++ -std=c++23 -Wall -Wextra -o 09_05 09_05.cpp && ./09_05


#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

template < typename T > class List
{
private :

    struct Node
    {
        T x = T();

        std::shared_ptr < Node > next;
        std::weak_ptr   < Node > prev;
    };

public :

    class Iterator
    {
    public :

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        Iterator
        (
            std::shared_ptr < Node > node = nullptr,
            List const * owner = nullptr
        )
            : m_node(node), m_owner(owner) {}

        auto const operator++(int)
        {
            auto x = *this;

            m_node = m_node->next;

            return x;
        }

        auto & operator++()
        {
            m_node = m_node->next;

            return *this;
        }

        auto const operator--(int)
        {
            auto x = *this;

            m_node = m_node->prev.lock();

            return x;
        }

        auto & operator--()
        {
            if (m_node)
            {
                m_node = m_node->prev.lock();
            }
            else if (m_owner)
            {
                m_node = m_owner->m_tail;
            }

            return *this;
        }

        auto & operator* () const { return  m_node->x; }

        auto   operator->() const { return &m_node->x; }

        friend auto operator==(Iterator const & lhs, Iterator const & rhs)
        {
            return lhs.m_node == rhs.m_node && lhs.m_owner == rhs.m_owner;
        }

    private :

        std::shared_ptr < Node > m_node;
        List const * m_owner = nullptr;
    };

    auto begin() const { return Iterator(m_head, this); }

    auto end  () const { return Iterator(nullptr, this); }

    auto last () const { return Iterator(m_tail, this); }

    void push_back(T x)
    {
        auto node = std::make_shared < Node > ();

        node->x = x;

        if (m_tail)
        {
            node->prev = m_tail;
            m_tail->next = node;
            m_tail = node;
        }
        else
        {
            m_head = node;
            m_tail = node;
        }
    }

private :

    std::shared_ptr < Node > m_head;

    std::shared_ptr < Node > m_tail;
};

int main()
{
    List < int > list;

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    {
        std::vector < int > forward;

        for (auto it = std::begin(list); it != std::end(list); ++it)
        {
            forward.push_back(*it);
        }

        assert((forward == std::vector < int > { 1, 2, 3 }));
    }

    {
        std::vector < int > range;

        for (auto element : list)
        {
            range.push_back(element);
        }

        assert((range == std::vector < int > { 1, 2, 3 }));
    }

    {
        std::vector < int > backward;

        for (auto it = std::prev(std::end(list)); ; --it)
        {
            backward.push_back(*it);
            if (it == std::begin(list)) break;
        }

        assert((backward == std::vector < int > { 3, 2, 1 }));
    }

    {
        auto it = std::begin(list);

        auto old = it++;
        assert(*old == 1);
        assert(*it == 2);

        auto old2 = it--;
        assert(*old2 == 2);
        assert(*it == 1);
    }

    std::cout << "forward  : ";
    for (auto it = std::begin(list); it != std::end(list); ++it) std::cout << *it << " ";
    std::cout << "\n";

    std::cout << "backward : ";
    for (auto it = list.last(); it != std::end(list); --it) std::cout << *it << " ";
    std::cout << "\n";
    std::cout << "All tests passed successfully.\n";
}
