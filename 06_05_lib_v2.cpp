

#include <print>

#include <boost/config.hpp>

#define API extern "C" BOOST_SYMBOL_EXPORT

API void test()
{
    std::print("Library v2 :: test() :: second version of realisation\n");
}

