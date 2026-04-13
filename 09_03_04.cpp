// g++ -std=c++23 -Wall -Wextra -o 09_03_04 09_03_04.cpp && ./09_03_04


#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

class Entity
{
public :

    virtual ~Entity() = default;

    virtual std::unique_ptr < Entity > copy() const = 0;
};

class Client : public Entity
{
public :

    std::unique_ptr < Entity > copy() const override
    {
        return std::make_unique < Client > (*this);
    }
};

class Server : public Entity
{
public :

    std::unique_ptr < Entity > copy() const override
    {
        return std::make_unique < Server > (*this);
    }
};

class Prototype
{
public :

    Prototype()
    {
        m_entities.push_back(std::make_unique < Client > ());

        m_entities.push_back(std::make_unique < Server > ());
    }

    auto make_client() { return m_entities.at(0)->copy(); }

    auto make_server() { return m_entities.at(1)->copy(); }

private :

    std::vector < std::unique_ptr < Entity > > m_entities;
};

int main()
{
    Prototype proto;

    auto client = proto.make_client();
    auto server = proto.make_server();

    assert(client != nullptr);
    assert(server != nullptr);

    assert(dynamic_cast < Client * > (client.get()) != nullptr);
    assert(dynamic_cast < Server * > (server.get()) != nullptr);

    auto client2 = proto.make_client();

    assert(client.get() != client2.get());

    std::cout << "All tests passed successfully.\n";
}
