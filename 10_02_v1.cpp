// g++ -std=c++23 -Wall -Wextra -o 10_02_v1 10_02_v1.cpp && ./10_02_v1


#include <algorithm>
#include <cassert>
#include <stack>
#include <utility>
#include <iostream>

template < typename T > class Stack
{
public :

    void push(T x)
    {
        m_stack.emplace(x, std::empty(m_stack) ? x : std::min(x, m_stack.top().second));
    }

    auto top() const
    {
        assert(!std::empty(m_stack));
        return m_stack.top().first;
    }

    void pop()
    {
        assert(!std::empty(m_stack));
        m_stack.pop();
    }

    auto min() const
    {
        assert(!std::empty(m_stack));
        return m_stack.top().second;
    }

    auto empty() const
    {
        return std::empty(m_stack);
    }

private :

    std::stack < std::pair < T, T > > m_stack;
};

int main()
{
    Stack < int > stack;

    stack.push(1); assert(stack.top() == 1 && stack.min() == 1);

    stack.push(3); assert(stack.top() == 3 && stack.min() == 1);

    stack.push(2); assert(stack.top() == 2 && stack.min() == 1);

                   assert(stack.top() == 2 && stack.min() == 1);

    stack.pop ( ); assert(stack.top() == 3 && stack.min() == 1);

    stack.pop ( ); assert(stack.top() == 1 && stack.min() == 1);

    stack.pop ( );

    stack.push(3); assert(stack.top() == 3 && stack.min() == 3);

    stack.push(1); assert(stack.top() == 1 && stack.min() == 1);

    stack.push(2); assert(stack.top() == 2 && stack.min() == 1);

    stack.pop ( ); assert(stack.top() == 1 && stack.min() == 1);

    stack.pop ( ); assert(stack.top() == 3 && stack.min() == 3);

    stack.pop ( );

    stack.push(5); assert(stack.min() == 5);

    stack.push(3); assert(stack.min() == 3);

    stack.push(1); assert(stack.min() == 1);

    stack.push(0); assert(stack.min() == 0);

    stack.pop ( ); assert(stack.min() == 1);

    stack.pop ( ); assert(stack.min() == 3);

    stack.pop ( ); assert(stack.min() == 5);

    stack.pop ( );

    stack.push(2); assert(stack.min() == 2);

    stack.push(2); assert(stack.min() == 2);

    stack.push(2); assert(stack.min() == 2);

    stack.pop ( ); assert(stack.min() == 2);

    stack.pop ( ); assert(stack.min() == 2);

    stack.pop ( );

    assert(stack.empty());
    std::cout << "All tests passed successfully.\n";
}
