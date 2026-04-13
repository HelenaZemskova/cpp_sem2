// g++ -std=c++23 -Wall -Wextra -o 09_03_03 09_03_03.cpp && ./09_03_03


#include <cassert>
#include <iostream>
#include <memory>

class Entity
{
public :

    virtual ~Entity() = default;
};

class Client : public Entity {};

class Server : public Entity {};

class Factory
{
public :

    virtual ~Factory() = default;

    virtual std::unique_ptr < Entity > make_entity() const = 0;
};

class Factory_Client : public Factory
{
public :

    std::unique_ptr < Entity > make_entity() const override
    {
        return std::make_unique < Client > ();
    }
};

class Factory_Server : public Factory
{
public :

    std::unique_ptr < Entity > make_entity() const override
    {
        return std::make_unique < Server > ();
    }
};

int main()
{
    std::unique_ptr < Factory > factory = std::make_unique < Factory_Client > ();

    auto entity = factory->make_entity();

    assert(entity != nullptr);
    assert(dynamic_cast < Client * > (entity.get()) != nullptr);
    assert(dynamic_cast < Server * > (entity.get()) == nullptr);

    factory = std::make_unique < Factory_Server > ();

    entity = factory->make_entity();

    assert(entity != nullptr);
    assert(dynamic_cast < Server * > (entity.get()) != nullptr);
    assert(dynamic_cast < Client * > (entity.get()) == nullptr);

    std::cout << "All tests passed successfully.\n";
}
