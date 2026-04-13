// g++ -std=c++23 -Wall -Wextra -o 10_01 10_01.cpp && ./10_01


#include <cassert>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <print>
#include <vector>

void test_vector_growth()
{
    std::print("____ std::vector capacity growth ____ \n\n");

    std::vector < int > vector;

    std::size_t prev_capacity = 0;
    double first_growth_ratio = 0.0;
    bool ratio_initialized = false;

    std::print("{:>6} {:>10} {:>10} {:>8}\n", "size", "capacity", "previous", "ratio");
    std::print("{}\n", std::string(40, '-'));

    for (auto i = 0; i < 65; ++i)
    {
        vector.push_back(i);

        if (vector.capacity() != prev_capacity)
        {
            double ratio = prev_capacity > 0
                         ? static_cast < double > (vector.capacity()) / prev_capacity
                         : 0.0;

            std::print("{:>6} {:>10} {:>10} {:>8.2f}\n",
                       vector.size(), vector.capacity(), prev_capacity, ratio);

            if (prev_capacity >= 2)
            {
                if (!ratio_initialized)
                {
                    first_growth_ratio = ratio;
                    ratio_initialized = true;
                }
                else
                {
                    assert(ratio == first_growth_ratio);
                }
            }

            prev_capacity = vector.capacity();
        }
    }

    std::print("\n");

    {
        std::vector < int > v;

        std::size_t last = 0;

        for (auto i = 0; i < 1024; ++i)
        {
            v.push_back(i);

            if (v.capacity() != last && last >= 2 && ratio_initialized)
            {
                auto ratio = static_cast < double > (v.capacity()) / last;
                assert(ratio == first_growth_ratio);
            }

            last = v.capacity();
        }

        std::print("vector growth factor = {:.2f} : ok\n\n", first_growth_ratio);
    }

    {
        std::vector < int > v;
        v.reserve(1);
        v.push_back(1);

        auto const * old_data = v.data();

        v.push_back(2);

        assert(v.data() != old_data);

        std::print("reallocation moves buffer : ok\n\n");
    }

    {
        std::vector < int > v;
        v.reserve(64);

        auto const * data = v.data();

        for (auto i = 0; i < 64; ++i)
        {
            v.push_back(i);
            assert(v.data() == data);
        }

        std::print("reserve() prevents reallocation : ok\n\n");
    }
}

void test_deque_pages()
{
    std::print("____ std::deque page structure ____ \n\n");

    constexpr auto n = 512uz;

    std::deque < int > deque;

    for (auto i = 0uz; i < n; ++i)
    {
        deque.push_back(static_cast < int > (i));
    }

    std::size_t page_elements = 0;
    std::size_t breaks        = 0;

    std::print("{:>6}  {:>18}  {:>10}  {}\n",
               "index", "address", "diff (B)", "note");
    std::print("{}\n", std::string(55, '-'));

    for (auto i = 1uz; i < n; ++i)
    {
        auto curr = reinterpret_cast < std::uintptr_t > (&deque[i]);
        auto prev = reinterpret_cast < std::uintptr_t > (&deque[i - 1]);

        auto diff = static_cast < std::ptrdiff_t > (curr) -
                    static_cast < std::ptrdiff_t > (prev);

        if (diff != static_cast < std::ptrdiff_t > (sizeof(int)))
        {
            if (page_elements == 0)
            {
                page_elements = i;
            }

            if (breaks < 6)
            {
                std::print("{:>6}  {:>18}  {:>10}  << page break\n",
                           i, reinterpret_cast < void * > (&deque[i]), diff);
            }

            ++breaks;
        }
    }

    std::print("...\n\n");

    auto page_bytes = page_elements * sizeof(int);

    std::print("elements per page : {}\n", page_elements);
    std::print("page size (bytes) : {}\n", page_bytes);
    std::print("total pages used  : {}\n\n", breaks + 1);

    {
        assert(page_elements > 0);

        for (auto i = 1uz; i < n; ++i)
        {
            auto curr = reinterpret_cast < std::uintptr_t > (&deque[i]);
            auto prev = reinterpret_cast < std::uintptr_t > (&deque[i - 1]);

            auto diff = static_cast < std::ptrdiff_t > (curr) -
                        static_cast < std::ptrdiff_t > (prev);

            if (diff != static_cast < std::ptrdiff_t > (sizeof(int)))
            {

                assert(i % page_elements == 0);
            }
        }

        std::print("page breaks occur every {} elements : ok\n\n", page_elements);
    }

    {
        std::deque < int > d;

        for (auto i = 0; i < 32; ++i) d.push_back(i);

        auto const * p = &d[0];

        for (auto i = 0; i < 32; ++i) d.push_back(i);

        assert(&d[0] == p);

        std::print("push_back does not move existing elements : ok\n\n");
    }

    {
        std::deque < int > d;

        for (auto i = 0; i < 32; ++i) d.push_back(i);

        auto const * p = &d[0];

        for (auto i = 0; i < 32; ++i) d.push_front(i);

        assert(&d[32] == p);

        std::print("push_front does not move existing elements : ok\n\n");
    }
}

int main()
{
    test_vector_growth();
    test_deque_pages();
    std::print("All tests passed successfully.\n");
}
