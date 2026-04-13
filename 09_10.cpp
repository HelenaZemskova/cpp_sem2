// g++ -std=c++23 -O3 -o 09_10 09_10.cpp -I/usr/local/include -L/usr/local/lib -lbenchmark -lpthread && ./09_10


#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <new>
#include <print>
#include <utility>
#include <vector>

#include <boost/noncopyable.hpp>
#include <benchmark/benchmark.h>

class Allocator : private boost::noncopyable
{
public :

    virtual ~Allocator() = default;

    virtual auto allocate(std::size_t size) -> void * = 0;

    virtual void deallocate(void * x) = 0;

protected :

    template < typename T >
    auto get(void * x) const -> T *
    {
        return static_cast < T * > (x);
    }

    static inline auto s_alignment = alignof(std::max_align_t);
};

class LinearAllocator : public Allocator
{
public :

    LinearAllocator(std::size_t size) : m_size(size)
    {
        m_begin = operator new(m_size, std::align_val_t(s_alignment));
    }

   ~LinearAllocator() override
    {
        operator delete(m_begin, m_size, std::align_val_t(s_alignment));
    }

    auto allocate(std::size_t size) -> void * override
    {
        void * begin = get < std::byte > (m_begin) + m_offset;

        auto free = m_size - m_offset;

        if (begin = std::align(s_alignment, size, begin, free); begin)
        {
            m_offset = m_size - free + size;

            return begin;
        }

        return nullptr;
    }

    void deallocate(void *) override {}

    void show() const
    {
        std::print
        (
            "LinearAllocator : m_size = {} m_begin = {} m_offset = {:04}\n",

            m_size, m_begin, m_offset
        );
    }

private :

    std::size_t m_size = 0, m_offset = 0;

    void * m_begin = nullptr;
};

class StackAllocator : public Allocator
{
public :

    StackAllocator(std::size_t size) : m_size(size)
    {
        m_begin = operator new(m_size, std::align_val_t(s_alignment));
    }

   ~StackAllocator() override
    {
        operator delete(m_begin, m_size, std::align_val_t(s_alignment));
    }

    auto allocate(std::size_t size) -> void * override
    {
        void * begin = get < std::byte > (m_begin) + m_offset + sizeof(header_t);

        auto free = m_size - m_offset - sizeof(header_t);

        if (begin = std::align(s_alignment, size, begin, free); begin)
        {
            auto header = get < header_t > (get < std::byte > (begin) - sizeof(header_t));

            *header = static_cast < header_t >
            (
                std::distance(get < std::byte > (m_begin) + m_offset, get < std::byte > (begin))
            );

            m_offset = get < std::byte > (begin) - get < std::byte > (m_begin) + size;

            return begin;
        }

        return nullptr;
    }

    void deallocate(void * x) override
    {
        auto header = get < header_t > (get < std::byte > (x) - sizeof(header_t));

        m_offset = get < std::byte > (x) - get < std::byte > (m_begin) - *header;
    }

    void show() const
    {
        std::print
        (
            "StackAllocator : m_size = {} m_begin = {} m_offset = {:04}\n",

            m_size, m_begin, m_offset
        );
    }

private :

    using header_t = std::uint8_t;

    std::size_t m_size = 0, m_offset = 0;

    void * m_begin = nullptr;
};

class PoolAllocator : public Allocator
{
public :

    PoolAllocator(std::size_t size, std::size_t step) : m_size(size), m_step(step)
    {
        assert(m_size % m_step == 0 && m_step >= sizeof(Node));

        make_list();

        m_begin = m_head;
    }

   ~PoolAllocator() override
    {
        for (auto list : m_lists)
        {
            operator delete(list, m_size, std::align_val_t(s_alignment));
        }
    }

    auto allocate(std::size_t) -> void * override
    {
        if (!m_head)
        {
            if (m_offset == std::size(m_lists))
                make_list();
            else
                m_head = get < Node > (m_lists[++m_offset - 1]);
        }

        auto node = m_head;

        if (!node->next)
        {
            auto next = get < std::byte > (node) + m_step;

            if (next != get < std::byte > (m_lists[m_offset - 1]) + m_size)
            {
                m_head = get < Node > (next);

                m_head->next = nullptr;
            }
            else
            {
                m_head = m_head->next;
            }
        }
        else
        {
            m_head = m_head->next;
        }

        return node;
    }

    void deallocate(void * x) override
    {
        auto node = get < Node > (x);

        node->next = m_head;

        m_head = node;
    }

    void show() const
    {
        std::print
        (
            "PoolAllocator : m_size = {} m_step = {} m_begin = {} m_head = {} m_offset = {}\n",

            m_size, m_step, m_begin, static_cast < void * > (m_head), m_offset
        );
    }

private :

    struct Node { Node * next = nullptr; };

    void make_list()
    {
        m_head = get < Node > (operator new(m_size, std::align_val_t(s_alignment)));

        m_head->next = nullptr;

        ++m_offset;

        m_lists.push_back(m_head);
    }

    std::size_t m_size = 0, m_step = 0, m_offset = 0;

    void * m_begin = nullptr;

    Node * m_head  = nullptr;

    std::vector < void * > m_lists;
};

class FreeListAllocator : public Allocator
{
public :

    FreeListAllocator(std::size_t size) : m_size(size)
    {
        assert(m_size >= sizeof(Node) + 1);

        m_begin = operator new(m_size, std::align_val_t(s_alignment));

        m_head = get < Node > (m_begin);

        m_head->size = m_size - sizeof(Header);

        m_head->next = nullptr;
    }

   ~FreeListAllocator() override
    {
        operator delete(m_begin, m_size, std::align_val_t(s_alignment));
    }

    auto allocate(std::size_t size) -> void * override
    {
        void * end = get < std::byte > (m_begin) + sizeof(Header) + size, * next = end;

        auto free = 2 * alignof(Header);

        if (next = std::align(alignof(Header), sizeof(Header), next, free); next)
        {
            auto padding = get < std::byte > (next) - get < std::byte > (end);

            if (auto [current, previous] = find(size + padding); current)
            {
                if (current->size >= size + padding + sizeof(Node) + 1)
                {
                    auto step = sizeof(Header) + size + padding;

                    auto node = get < Node > (get < std::byte > (current) + step);

                    node->size = current->size - step;

                    node->next = current->next;

                    current->next = node;
                }
                else
                {
                    padding += current->size - size - padding;
                }

                if (!previous) m_head          = current->next;
                else           previous->next  = current->next;

                auto header = get < Header > (current);

                header->size = size + padding;

                return get < std::byte > (current) + sizeof(Header);
            }
        }

        return nullptr;
    }

    void deallocate(void * x) override
    {
        auto node = get < Node > (get < std::byte > (x) - sizeof(Header));

        Node * previous = nullptr, * current = m_head;

        while (current && current < node)
        {
            previous = current;
            current  = current->next;
        }

        node->next = current;

        if (!previous) m_head = node;
        else previous->next = node;

        merge(previous, node);
    }

    void show() const
    {
        std::print
        (
            "FreeListAllocator : m_size = {} m_begin = {} m_head = {}\n",

            m_size, m_begin, static_cast < void * > (m_head)
        );
    }

private :

    struct Node
    {
        std::size_t size = 0;

        Node * next = nullptr;
    };

    struct alignas(std::max_align_t) Header { std::size_t size = 0; };

    auto find(std::size_t size) const -> std::pair < Node *, Node * >
    {
        Node * current = m_head, * previous = nullptr;

        while (current && size > current->size)
        {
            previous = current;

            current = current->next;
        }

        return { current, previous };
    }

    void merge(Node * previous, Node * node) const
    {
        if (node->next &&
            get < std::byte > (node) + sizeof(Header) + node->size == get < std::byte > (node->next))
        {
            node->size += sizeof(Header) + node->next->size;

            node->next  = node->next->next;
        }

        if (previous &&
            get < std::byte > (previous) + sizeof(Header) + previous->size == get < std::byte > (node))
        {
            previous->size += sizeof(Header) + node->size;

            previous->next = node->next;
        }
    }

    std::size_t m_size = 0;

    void * m_begin = nullptr;

    Node * m_head = nullptr;
};

void bench_linear(benchmark::State & state)
{
    auto kb = 1uz << 10, mb = 1uz << 20, gb = 1uz << 30;

    std::vector < void * > vector(kb, nullptr);

    for ([[maybe_unused]] auto _ : state)
    {
        LinearAllocator allocator(gb);

        for (auto i = 0uz; i < kb; ++i) vector[i] = allocator.allocate(mb);

        benchmark::DoNotOptimize(vector);
    }
}

void bench_stack(benchmark::State & state)
{
    auto kb = 1uz << 10, mb = 1uz << 20, gb = 1uz << 30;

    std::vector < void * > vector(kb, nullptr);

    for ([[maybe_unused]] auto _ : state)
    {
        StackAllocator allocator(2 * gb);

        for (auto i = 0uz; i < kb; ++i) vector[i] = allocator.allocate(mb);

        for (auto i = 0uz; i < kb; ++i) allocator.deallocate(vector[kb - 1 - i]);

        benchmark::DoNotOptimize(vector);
    }
}

void bench_pool(benchmark::State & state)
{
    auto kb = 1uz << 10, mb = 1uz << 20, gb = 1uz << 30;

    std::vector < void * > vector(kb, nullptr);

    for ([[maybe_unused]] auto _ : state)
    {
        PoolAllocator allocator(gb, mb);

        for (auto i = 0uz; i < kb; ++i) vector[i] = allocator.allocate(0);

        for (auto i = 0uz; i < kb; i += 2) allocator.deallocate(vector[i]);

        for (auto i = 0uz; i < kb; i += 2) vector[i] = allocator.allocate(0);

        for (auto i = 0uz; i < kb; ++i) allocator.deallocate(vector[i]);

        benchmark::DoNotOptimize(vector);
    }
}

void bench_freelist(benchmark::State & state)
{
    auto kb = 1uz << 10, mb = 1uz << 20, gb = 1uz << 30;

    std::vector < void * > vector(kb, nullptr);

    for ([[maybe_unused]] auto _ : state)
    {
        FreeListAllocator allocator(16 * gb);

        for (auto i = 0uz; i < kb; ++i) vector[i] = allocator.allocate(mb);

        for (auto i = 0uz; i < kb; i += 32) allocator.deallocate(vector[i]);

        for (auto i = 0uz; i < kb; i += 32) vector[i] = allocator.allocate(mb);

        for (auto i = 0uz; i < kb; ++i) allocator.deallocate(vector[i]);

        benchmark::DoNotOptimize(vector);
    }
}

BENCHMARK(bench_linear);
BENCHMARK(bench_stack);
BENCHMARK(bench_pool);
BENCHMARK(bench_freelist);

void test_linear()
{
    std::print("test_linear : ");

    LinearAllocator a(256);

    auto p1 = a.allocate(16);
    assert(p1 != nullptr);

    auto p2 = a.allocate(32);
    assert(p2 != nullptr);

    assert(p1 != p2);
    assert(p1 < p2);

    assert(reinterpret_cast < std::uintptr_t > (p1) % alignof(std::max_align_t) == 0);
    assert(reinterpret_cast < std::uintptr_t > (p2) % alignof(std::max_align_t) == 0);

    a.deallocate(p1);
    auto p3 = a.allocate(16);
    assert(p3 != nullptr && p3 > p2);

    auto p_fail = a.allocate(1024);
    assert(p_fail == nullptr);

    std::print("ok\n");
}

void test_stack()
{
    std::print("test_stack : ");

    StackAllocator a(1 << 10);

    auto p1 = a.allocate(8);
    assert(p1 != nullptr);

    auto p2 = a.allocate(8);
    assert(p2 != nullptr && p2 != p1);

    auto p3 = a.allocate(8);
    assert(p3 != nullptr && p3 != p2);

    a.deallocate(p3);
    auto p3b = a.allocate(8);
    assert(p3b == p3);

    a.deallocate(p3b);
    a.deallocate(p2);
    auto p2b = a.allocate(8);
    assert(p2b == p2);

    a.deallocate(p2b);
    a.deallocate(p1);
    auto p1b = a.allocate(8);
    assert(p1b == p1);

    std::print("ok\n");
}

void test_pool()
{
    std::print("test_pool : ");

    PoolAllocator a(64, 16);

    auto p1 = a.allocate(0);
    auto p2 = a.allocate(0);
    auto p3 = a.allocate(0);
    auto p4 = a.allocate(0);

    assert(p1 && p2 && p3 && p4);

    assert(p1 != p2 && p2 != p3 && p3 != p4);

    assert(reinterpret_cast<std::byte*>(p2) - reinterpret_cast<std::byte*>(p1) == 16);
    assert(reinterpret_cast<std::byte*>(p3) - reinterpret_cast<std::byte*>(p2) == 16);

    a.deallocate(p2);
    auto p2b = a.allocate(0);
    assert(p2b == p2);

    a.deallocate(p2b);
    a.deallocate(p4);
    a.deallocate(p3);

    auto q1 = a.allocate(0);
    auto q2 = a.allocate(0);
    assert(q1 == p3 && q2 == p4);

    a.deallocate(p1);
    a.deallocate(q1);
    a.deallocate(q2);

    std::print("ok\n");
}

void test_freelist()
{
    std::print("test_freelist : ");

    FreeListAllocator a(1 << 10);

    auto p1 = a.allocate(16);
    auto p2 = a.allocate(16);
    auto p3 = a.allocate(16);
    auto p4 = a.allocate(16);

    assert(p1 && p2 && p3 && p4);
    assert(p1 != p2 && p2 != p3 && p3 != p4);

    a.deallocate(p1);
    a.deallocate(p2);

    auto p12 = a.allocate(32);
    assert(p12 == p1);

    a.deallocate(p3);
    a.deallocate(p4);

    auto q = a.allocate(8);
    assert(q == p3);

    a.deallocate(p12);
    a.deallocate(q);

    auto big = a.allocate(512);
    assert(big != nullptr);

    a.deallocate(big);

    std::print("ok\n");
}

void test_polymorphic()
{
    std::print("test_polymorphic : ");

    auto test = [](Allocator & a, std::size_t sz)
    {
        auto x = a.allocate(sz);
        auto y = a.allocate(sz);

        assert(x != nullptr && y != nullptr && x != y);

        a.deallocate(y);
        a.deallocate(x);
    };

    LinearAllocator   linear  (1 << 14);
    StackAllocator    stack   (1 << 14);
    PoolAllocator     pool    (1 << 10, 64);
    FreeListAllocator freelist(1 << 14);

    test(linear,   32);
    test(stack,    32);
    test(pool,      0);
    test(freelist, 32);

    std::print("ok\n");
}

int main()
{
    test_linear();
    test_stack();
    test_pool();
    test_freelist();
    test_polymorphic();

    std::print("\n");

    std::print("____ LinearAllocator ____ \n");
    {
        LinearAllocator a(1 << 10);

        a.show(); a.allocate(1);
        a.show(); a.allocate(16);
        a.show(); a.allocate(32);
        a.show();
    }

    std::print("\n____ StackAllocator ____ \n");
    {
        StackAllocator a(1 << 10);

        a.show();          a.allocate(4);
        a.show(); auto x = a.allocate(8);
        a.show(); auto y = a.allocate(16);
        a.show();

        a.deallocate(y); a.show();
        a.deallocate(x); a.show();

        auto z = a.allocate(8); a.show();

        assert(z == x);
    }

    std::print("\n____ PoolAllocator ____ \n");
    {
        PoolAllocator a(32, 8);

        a.show();          a.allocate(0);
        a.show(); auto x = a.allocate(0);
        a.show(); auto y = a.allocate(0);
        a.show();

        a.deallocate(x); a.show();
        a.deallocate(y); a.show();

        auto z = a.allocate(0); a.show();

        assert(z == y);
    }

    std::print("\n____ FreeListAllocator ____ \n");
    {
        FreeListAllocator a(1 << 10);

        a.show();          a.allocate(16);
        a.show(); auto x = a.allocate(16);
        a.show(); auto y = a.allocate(16);
        a.show();          a.allocate(16);

        a.show(); a.deallocate(y);
        a.show(); a.deallocate(x);

        a.show(); auto z = a.allocate(32);
        a.show();

        assert(z == x);
    }

    benchmark::RunSpecifiedBenchmarks();
    std::print("All tests passed successfully.\n");
}
