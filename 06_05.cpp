// usage : run from the build output directory
//
//  enter library file path: ./libtest_v1.so or ./libtest_v2.so 
// or enter q to exit 

////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <iostream>
#include <print>
#include <string>
#include <boost/dll.hpp>
#include <boost/filesystem.hpp>

////////////////////////////////////////////////////////////////////////////////

int main()
{
    std::string path;


    while (std::print("Enter library file path (or 'q' to quit): "),
           std::cin >> path && path != "q")
    {
        try
        {
            std::function < void() > test =
                boost::dll::import_symbol < void() > (
                    boost::filesystem::path{path}, "test");

            test();
        }
        catch (std::exception const & e)
        {
            std::print("Error loading library: {}\n", e.what());
        }
    }
}

