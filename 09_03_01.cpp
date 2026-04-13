// g++ -std=c++23 -Wall -Wextra -o 09_03_01 09_03_01.cpp && ./09_03_01


#include <cassert>
#include <iostream>
#include <memory>

struct Entity
{
    int x = 0, y = 0;
};

class Builder
{
public :

    virtual ~Builder() = default;

    auto make_entity()
    {
        m_entity = std::make_unique < Entity > ();

        set_x();

        set_y();

        return std::move(m_entity);
    }

    virtual void set_x() const = 0;

    virtual void set_y() const = 0;

protected :

    std::unique_ptr < Entity > m_entity;
};

class Builder_Client : public Builder
{
public :

    void set_x() const override { m_entity->x = 1; }

    void set_y() const override { m_entity->y = 1; }
};

class Builder_Server : public Builder
{
public :

    void set_x() const override { m_entity->x = 2; }

    void set_y() const override { m_entity->y = 2; }
};

int main()
{
    std::unique_ptr < Builder > builder = std::make_unique < Builder_Client > ();

    auto entity = builder->make_entity();

    assert(entity != nullptr);
    assert(entity->x == 1 && entity->y == 1);

    builder = std::make_unique < Builder_Server > ();
    entity = builder->make_entity();

    assert(entity != nullptr);
    assert(entity->x == 2 && entity->y == 2);

    std::cout << "All tests passed successfully.\n";
}
