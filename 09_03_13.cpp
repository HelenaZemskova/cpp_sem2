// g++ -std=c++23 -Wall -Wextra -o 09_03_13 09_03_13.cpp && ./09_03_13


#include <cassert>
#include <memory>
#include <print>
#include <vector>

class Observer
{
public :

    virtual ~Observer() = default;

    virtual void test(int x) const = 0;
};

class Entity
{
public :

    void add(std::shared_ptr < Observer > observer)
    {
        m_observers.push_back(std::move(observer));
    }

    void set(int x)
    {
        m_x = x;

        notify_all();
    }

    void notify_all() const
    {
        for (auto const & observer : m_observers)
        {
            if (observer)
            {
                observer->test(m_x);
            }
        }
    }

private :

    int m_x = 0;

    std::vector < std::shared_ptr < Observer > > m_observers;
};

class Client : public Observer
{
public :

    void test(int x) const override
    {
        std::print("Client::test : x = {}\n", x);
    }
};

class Server : public Observer
{
public :

    void test(int x) const override
    {
        std::print("Server::test : x = {}\n", x);
    }
};

class Counter : public Observer
{
public :

    void test(int x) const override
    {
        m_last = x;
        ++m_count;
    }

    int last()  const { return m_last;  }
    int count() const { return m_count; }

private :

    mutable int m_last  = 0;
    mutable int m_count = 0;
};

int main()
{
    Entity entity;

    auto client  = std::make_shared < Client  > ();
    auto server  = std::make_shared < Server  > ();
    auto counter = std::make_shared < Counter > ();

    entity.add(client);
    entity.add(server);
    entity.add(counter);

    for (auto i = 0; i < 2; ++i)
    {
        entity.set(i + 1);
    }

    assert(counter->count() == 2);
    assert(counter->last()  == 2);

    std::print("All tests passed successfully.\n");
}
