// g++ -std=c++23 -Wall -Wextra -o 09_03_09 09_03_09.cpp && ./09_03_09


#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

class Entity
{
public :

    virtual ~Entity() = default;

    virtual int test() const = 0;
};

class Client : public Entity
{
public :

    int test() const override { return 1; }
};

class Server : public Entity
{
public :

    int test() const override { return 2; }
};

class Composite : public Entity
{
public :

    void add(std::unique_ptr < Entity > entity)
    {
        m_entities.push_back(std::move(entity));
    }

    int test() const override
    {
        auto x = 0;

        for (auto const & entity : m_entities)
        {
            if (entity)
            {
                x += entity->test();
            }
        }

        return x;
    }

private :

    std::vector < std::unique_ptr < Entity > > m_entities;
};

auto make_composite(std::size_t size_1, std::size_t size_2) -> std::unique_ptr < Entity >
{
    auto composite = std::make_unique < Composite > ();

    for (auto i = 0uz; i < size_1; ++i) { composite->add(std::make_unique < Client > ()); }

    for (auto i = 0uz; i < size_2; ++i) { composite->add(std::make_unique < Server > ()); }

    return composite;
}

int main()
{
    auto composite = std::make_unique < Composite > ();

    for (auto i = 0uz; i < 5; ++i)
    {
        composite->add(make_composite(1, 1));
    }

    std::unique_ptr < Entity > entity = std::move(composite);

    assert(entity->test() == 15);

    std::cout << "All tests passed successfully.\n";
}
