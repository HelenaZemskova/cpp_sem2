// g++ -std=c++23 -Wall -Wextra -o 11_01 11_01.cpp && ./11_01


#include <cassert>
#include <iostream>

class Wrapper;

using FnPtr = Wrapper(*)();

class Wrapper
{
public :

    Wrapper(FnPtr fn = nullptr) : m_fn(fn) {}

    operator FnPtr() const
    {
        return m_fn;
    }

private :

    FnPtr m_fn;
};

static int g_call_count = 0;

Wrapper test()
{
    std::cout << "test : called\n";

    ++g_call_count;

    return Wrapper(test);
}

int main()
{
    Wrapper function = test();

    assert(g_call_count == 1);

    (*function)();

    assert(g_call_count == 2);

    FnPtr ptr = function;

    assert(ptr == test);

    Wrapper function2 = (*function)();

    assert(g_call_count == 3);

    FnPtr ptr2 = function2;

    assert(ptr2 == test);
    std::cout << "All tests passed successfully.\n";
}
