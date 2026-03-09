
#include <print>

#include <boost/config.hpp>

#define API extern "C" BOOST_SYMBOL_EXPORT


API void test()
{
    std::print("Library v1 :: test() :: first version of realisation\n");
}

