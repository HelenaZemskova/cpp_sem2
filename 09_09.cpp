// g++ -std=c++23 -O3 -o 09_09 09_09.cpp -I/usr/local/include -L/usr/local/lib -lbenchmark -lpthread && ./09_09


#include <cassert>
#include <cstddef>
#include <memory>
#include <new>
#include <print>
#include <random>
#include <utility>
#include <vector>

#include <boost/noncopyable.hpp>
#include <benchmark/benchmark.h>

enum class Policy { first_fit, best_fit };

class Allocator : private boost::noncopyable
{
public :

    Allocator(std::size_t size, Policy policy = Policy::first_fit)
        : m_size(size), m_policy(policy)
    {
        assert(m_size >= sizeof(Node) + 1);

        m_begin = operator new(m_size, std::align_val_t(s_alignment));

        m_head = get_node(m_begin);

        m_head->size = m_size - sizeof(Header);

        m_head->next = nullptr;
    }

   ~Allocator()
    {
        operator delete(m_begin, m_size, std::align_val_t(s_alignment));
    }

    auto allocate(std::size_t size) -> void *
    {
        void * end = get_byte(m_begin) + sizeof(Header) + size, * next = end;

        auto free = 2 * alignof(Header);

        if (next = std::align(alignof(Header), sizeof(Header), next, free); next)
        {
            auto padding = get_byte(next) - get_byte(end);

            if (auto [current, previous] = find(size + padding); current)
            {
                if (current->size >= size + padding + sizeof(Node) + 1)
                {
                    auto step = sizeof(Header) + size + padding;

                    auto node = get_node(get_byte(current) + step);

                    node->size = current->size - step;

                    node->next = current->next;

                    current->next = node;
                }
                else
                {
                    padding += current->size - size - padding;
                }

                if (!previous)
                {
                    m_head = current->next;
                }
                else
                {
                    previous->next = current->next;
                }

                auto header = get_header(current);

                header->size = size + padding;

                return get_byte(current) + sizeof(Header);
            }
        }

        return nullptr;
    }

    void deallocate(void * x)
    {
        auto node = get_node(get_byte(x) - sizeof(Header));

        Node * previous = nullptr, * current = m_head;

        while (current && current < node)
        {
            previous = current;
            current  = current->next;
        }

        node->next = current;

        if (!previous)
        {
            m_head = node;
        }
        else
        {
            previous->next = node;
        }

        merge(previous, node);
    }

    void show() const
    {
        std::print
        (
            "Allocator::show : m_size = {} m_begin = {} m_head = {} ",

            m_size, m_begin, static_cast < void * > (m_head)
        );

        if (m_head->next)
        {
            std::print("m_head->next = {}\n", static_cast < void * > (m_head->next));
        }
        else
        {
            std::print("\n");
        }
    }

private :

    struct Node
    {
        std::size_t size = 0;

        Node * next = nullptr;
    };

    struct alignas(std::max_align_t) Header
    {
        std::size_t size = 0;
    };

    auto get_byte(void * x) const -> std::byte *
    {
        return static_cast < std::byte * > (x);
    }

    auto get_node(void * x) const -> Node *
    {
        return static_cast < Node * > (x);
    }

    auto get_header(void * x) const -> Header *
    {
        return static_cast < Header * > (x);
    }

    auto find(std::size_t size) const -> std::pair < Node *, Node * >
    {
        if (m_policy == Policy::first_fit)
        {
            return find_first(size);
        }
        else
        {
            return find_best(size);
        }
    }

    auto find_first(std::size_t size) const -> std::pair < Node *, Node * >
    {
        Node * current = m_head, * previous = nullptr;

        while (current && size > current->size)
        {
            previous = current;

            current  = current->next;
        }

        return std::make_pair(current, previous);
    }

    auto find_best(std::size_t size) const -> std::pair < Node *, Node * >
    {
        Node * best = nullptr, * best_prev = nullptr;

        Node * current = m_head, * previous = nullptr;

        while (current)
        {
            if (current->size >= size)
            {
                if (!best || current->size < best->size)
                {
                    best      = current;
                    best_prev = previous;
                }
            }

            previous = current;

            current = current->next;
        }

        return std::make_pair(best, best_prev);
    }

    void merge(Node * previous, Node * node) const
    {
        if (node->next && get_byte(node) + sizeof(Header) + node->size == get_byte(node->next))
        {
            node->size += sizeof(Header) + node->next->size;

            node->next = node->next->next;
        }

        if (previous && get_byte(previous) + sizeof(Header) + previous->size == get_byte(node))
        {
            previous->size += sizeof(Header) + node->size;

            previous->next = node->next;
        }
    }

    std::size_t m_size = 0;

    void * m_begin = nullptr;

    Node * m_head = nullptr;

    Policy m_policy = Policy::first_fit;

    static inline auto s_alignment = alignof(std::max_align_t);
};

template < Policy policy > void run(benchmark::State & state)
{
    auto kb = 1uz << 10, mb = 1uz << 20, gb = 1uz << 30;

    std::uniform_int_distribution distribution(1, 16);

    std::default_random_engine engine;

    std::vector < void * > vector(kb, nullptr);

    for ([[maybe_unused]] auto _ : state)
    {
        Allocator allocator(16 * gb, policy);

        for (auto i = 0uz; i < kb; ++i)
        {
            vector[i] = allocator.allocate(distribution(engine) * mb);
        }

        for (auto i = 0uz; i < kb; i += 32)
        {
            allocator.deallocate(vector[i]);
        }

        for (auto i = 0uz; i < kb; i += 32)
        {
            vector[i] = allocator.allocate(distribution(engine) * mb);
        }

        for (auto i = 0uz; i < kb; ++i)
        {
            allocator.deallocate(vector[i]);
        }

        benchmark::DoNotOptimize(vector);
    }
}

void test_first_fit(benchmark::State & state) { run < Policy::first_fit > (state); }

void test_best_fit (benchmark::State & state) { run < Policy::best_fit  > (state); }

BENCHMARK(test_first_fit);

BENCHMARK(test_best_fit);

int main()
{
    Allocator allocator(1 << 10);

    allocator.show(); allocator.allocate(16);

    allocator.show(); auto x = allocator.allocate(16);

    allocator.show(); auto y = allocator.allocate(16);

    allocator.show(); allocator.allocate(16);

    allocator.show(); allocator.deallocate(y);

    allocator.show(); allocator.deallocate(x);

    allocator.show(); auto z = allocator.allocate(32);

    allocator.show();

    assert(z == x);

    benchmark::RunSpecifiedBenchmarks();
    std::print("All tests passed successfully.\n");
}
