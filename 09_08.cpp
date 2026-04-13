// g++ -std=c++23 -Wall -Wextra -o 09_08 09_08.cpp && ./09_08


#include <cassert>
#include <cstddef>
#include <new>
#include <print>
#include <stdexcept>

template < typename D > class Entity
{
public :
    struct Stats
    {
        int new_count = 0;
        int delete_count = 0;
        int new_nothrow_count = 0;
        int delete_nothrow_count = 0;
        int new_array_count = 0;
        int delete_array_count = 0;
        int new_array_nothrow_count = 0;
        int delete_array_nothrow_count = 0;
    };

    static void reset_stats() { s_stats = {}; }

    static auto stats() -> Stats const & { return s_stats; }

    static auto operator new(std::size_t size) -> void *
    {
        ++s_stats.new_count;
        std::print("Entity::operator new\n");

        return ::operator new(size);
    }

    static void operator delete(void * x)
    {
        ++s_stats.delete_count;
        std::print("Entity::operator delete\n");

        ::operator delete(x);
    }

    static auto operator new(std::size_t size, std::nothrow_t const & nt) noexcept -> void *
    {
        ++s_stats.new_nothrow_count;
        std::print("Entity::operator new (nothrow)\n");

        return ::operator new(size, nt);
    }

    static void operator delete(void * x, std::nothrow_t const & nt) noexcept
    {
        ++s_stats.delete_nothrow_count;
        std::print("Entity::operator delete (nothrow)\n");

        ::operator delete(x, nt);
    }

    static auto operator new[](std::size_t size) -> void *
    {
        ++s_stats.new_array_count;
        std::print("Entity::operator new[]\n");

        return ::operator new[](size);
    }

    static void operator delete[](void * x)
    {
        ++s_stats.delete_array_count;
        std::print("Entity::operator delete[]\n");

        ::operator delete[](x);
    }

    static auto operator new[](std::size_t size, std::nothrow_t const & nt) noexcept -> void *
    {
        ++s_stats.new_array_nothrow_count;
        std::print("Entity::operator new[] (nothrow)\n");

        return ::operator new[](size, nt);
    }

    static void operator delete[](void * x, std::nothrow_t const & nt) noexcept
    {
        ++s_stats.delete_array_nothrow_count;
        std::print("Entity::operator delete[] (nothrow)\n");

        ::operator delete[](x, nt);
    }

protected :

    Entity() = default;

private :
    static inline Stats s_stats = {};
};

class Client : private Entity < Client >
{
public :

    Client()  { std::print("Client:: Client\n"); }

   ~Client()  { std::print("Client::~Client\n"); }

    using Entity < Client > ::operator new;

    using Entity < Client > ::operator delete;

    using Entity < Client > ::operator new[];

    using Entity < Client > ::operator delete[];
};

class ThrowingClient : private Entity < ThrowingClient >
{
public :
    ThrowingClient() { throw std::runtime_error("ctor fail"); }

    using Entity < ThrowingClient > ::operator new;
    using Entity < ThrowingClient > ::operator delete;
    using Entity < ThrowingClient > ::operator new[];
    using Entity < ThrowingClient > ::operator delete[];
};

int main()
{
    std::print("___ new / delete ___\n");

    {
        Entity < Client > ::reset_stats();

        auto * p = new Client;
        assert(p != nullptr);
        delete p;

        auto const & s = Entity < Client > ::stats();
        assert(s.new_count == 1);
        assert(s.delete_count == 1);
        assert(s.new_array_count == 0);
        assert(s.delete_array_count == 0);
    }

    std::print("\n___ new[] / delete[] ___\n");

    {
        Entity < Client > ::reset_stats();

        auto * p = new Client[2];
        assert(p != nullptr);
        delete[] p;

        auto const & s = Entity < Client > ::stats();
        assert(s.new_array_count == 1);
        assert(s.delete_array_count == 1);
    }

    std::print("\n___ new(nothrow) / delete ___\n");

    {
        Entity < Client > ::reset_stats();

        auto * p = new (std::nothrow) Client;
        assert(p != nullptr);
        delete p;

        auto const & s = Entity < Client > ::stats();
        assert(s.new_nothrow_count == 1);
        assert(s.delete_count == 1);
    }

    std::print("\n___ new[](nothrow) / delete[] ___\n");

    {
        Entity < Client > ::reset_stats();

        auto * p = new (std::nothrow) Client[2];
        assert(p != nullptr);
        delete[] p;

        auto const & s = Entity < Client > ::stats();
        assert(s.new_array_nothrow_count == 1);
        assert(s.delete_array_count == 1);
    }

    std::print("\n___ new(nothrow) ctor throw => delete(nothrow) ___\n");

    {
        Entity < ThrowingClient > ::reset_stats();

        try
        {
            [[maybe_unused]] auto * p = new (std::nothrow) ThrowingClient;
            assert(false);
        }
        catch (std::runtime_error const &)
        {
            auto const & s = Entity < ThrowingClient > ::stats();
            assert(s.new_nothrow_count == 1);
            assert(s.delete_nothrow_count == 1);
        }
    }

    std::print("\n___ new[](nothrow) ctor throw => delete[](nothrow) ___\n");

    {
        Entity < ThrowingClient > ::reset_stats();

        try
        {
            [[maybe_unused]] auto * p = new (std::nothrow) ThrowingClient[2];
            assert(false);
        }
        catch (std::runtime_error const &)
        {
            auto const & s = Entity < ThrowingClient > ::stats();
            assert(s.new_array_nothrow_count == 1);
            assert(s.delete_array_nothrow_count == 1);
        }
    }
    std::print("All tests passed successfully.\n");
}
