// g++ -std=c++23 -Wall -Wextra -o 10_02_v2 10_02_v2.cpp && ./10_02_v2


#include <cassert>
#include <stack>
#include <iostream>

template < typename T > class Stack
{
public :

    void push(T x)
    {
        if (std::empty(m_stack))
        {
            m_stack.push(x);

            m_min = x;
        }
        else if (x < m_min)
        {
            m_stack.push(2 * x - m_min);

            m_min = x;
        }
        else
        {
            m_stack.push(x);
        }
    }

    auto top() const
    {
        assert(!std::empty(m_stack));
        return m_stack.top() < m_min ? m_min : m_stack.top();
    }

    void pop()
    {
        assert(!std::empty(m_stack));
        if (auto t = m_stack.top(); t < m_min)
        {
            (m_min *= 2) -= t;
        }

        m_stack.pop();
    }

    auto min() const
    {
        assert(!std::empty(m_stack));
        return m_min;
    }

    auto empty() const
    {
        return std::empty(m_stack);
    }

private :

    std::stack < T > m_stack;

    T m_min = T();
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

    stack.push(10);
    stack.push(4);
    stack.push(7);

    assert(stack.top() == 7 && stack.min() == 4);

    stack.pop ( );

    assert(stack.top() == 4 && stack.min() == 4);

    stack.pop ( );

    assert(stack.top() == 10 && stack.min() == 10);

    stack.pop ( );

    assert(stack.empty());
    std::cout << "All tests passed successfully.\n";
}
