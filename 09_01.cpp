// g++ -std=c++23 -Wall -Wextra -o 09_01 09_01.cpp && ./09_01


#include <cassert>
#include <iostream>
#include <source_location>
#include <sstream>

class Tracer
{
public :

    Tracer(std::source_location location = std::source_location::current())
        : m_location(location)
    {
        std::cout << "[enter] " << m_location.function_name()
                  << " (" << m_location.file_name() << ":" << m_location.line() << ")\n";
    }

   ~Tracer()
    {
        std::cout << "[leave] " << m_location.function_name() << "\n";
    }

private :

    std::source_location m_location;
};

#ifndef NDEBUG
#   define trace(name) Tracer name(std::source_location::current())
#else
#   define trace(name) ((void)0)
#endif

void foo()
{
    trace(tracer);

    std::cout << "foo : doing work\n";
}

void bar()
{
    trace(tracer);

    foo();

    std::cout << "bar : doing work\n";
}

void test_raii()
{
    std::ostringstream oss;
    auto * old_buf = std::cout.rdbuf(oss.rdbuf());

    {
        trace(tracer);
    }

    std::cout.rdbuf(old_buf);

    auto const output = oss.str();

#ifndef NDEBUG
    assert(output.find("[enter]") != std::string::npos);
    assert(output.find("[leave]") != std::string::npos);
    assert(output.find("[enter]") < output.find("[leave]"));
#else

    assert(output.empty());
#endif
}

int main()
{
    test_raii();

    trace(tracer);

    bar();
}
