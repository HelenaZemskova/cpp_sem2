// g++ -std=c++23 -Wall -Wextra -o 08_01 08_01.cpp && ./08_01

#include <bit>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <print>

class Entity_v1
{
public:
    void show() const
    {
        std::print("Entity_v1::m_value = {}\n", m_value);
    }

private:
    int m_value = 0;
};


class Entity_v2
{
public:
    int m_value = 0;
};

void method_reinterpret_cast()
{
    std::print("\n Method 1: reinterpret_cast<Entity_v2&>\n");

    Entity_v1 e;
    e.show();

    reinterpret_cast < Entity_v2 & > (e).m_value = 111;

    e.show();
}

void method_pointer_arithmetic()
{
    std::print("\n Method 2: reinterpret_cast<int*>(&entity)\n");

    Entity_v1 e;
    e.show();

    *reinterpret_cast < int * > (&e) = 222;

    e.show();
}

void method_bit_cast()
{
    std::print("\n Method 3: std::bit_cast + std::memcpy\n");

    Entity_v1 e;
    e.show();

    auto copy = std::bit_cast < Entity_v2 > (e);

    copy.m_value = 333;

    std::memcpy(&e, &copy, sizeof(Entity_v1));

    e.show();
}

int main()
{
    static_assert(sizeof(Entity_v1) == sizeof(Entity_v2),
        "Entity_v1 and Entity_v2 must have the same size");

    static_assert(sizeof(Entity_v1) == sizeof(int),
        "Entity_v1 must consist of exactly one int");

    method_reinterpret_cast();
    method_pointer_arithmetic();
    method_bit_cast();
}
