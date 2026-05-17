// g++ -std=c++23 -Wall -Wextra -pedantic 13_03.cpp -o 13_03

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <sstream>
#include <string>

void transform(std::string const & path_1, std::string const & path_2)
{
    auto string = (std::stringstream() << std::fstream(path_1, std::ios::in).rdbuf()).str();

    std::string output;
    std::string line_buffer;
    bool line_has_content = false;

    auto flush = [&](bool always)
    {
        if (always || line_has_content)
        {
            output += line_buffer + '\n';
        }

        line_buffer.clear();
        line_has_content = false;
    };

    for (auto it = std::begin(string); it != std::end(string); )
    {
        if (*it == '\'')
        {
            line_has_content = true;

            line_buffer += *it;

            do
            {
                ++it;

                line_buffer += *it;
            }
            while (!(*it == '\'' && *std::prev(it) != '\\'));

            ++it;

            continue;
        }

        if (*it == 'R' && std::next(it) != std::end(string) && *std::next(it) == '"')
        {
            line_has_content = true;

            line_buffer += *it++;
            line_buffer += *it++;

            std::string closing = ")";

            while (*it != '(')
            {
                closing += *it;
                line_buffer += *it++;
            }

            closing += '"';

            line_buffer += *it++;

            auto const close_pos = string.find(closing, std::distance(std::begin(string), it));

            while (std::distance(std::begin(string), it) < static_cast < std::ptrdiff_t > (close_pos + closing.size()))
            {
                if (*it == '\n')
                {
                    flush(true);
                }
                else
                {
                    if (!std::isspace(static_cast < unsigned char > (*it))) line_has_content = true;

                    line_buffer += *it;
                }

                ++it;
            }

            continue;
        }

        if (*it == '"')
        {
            line_has_content = true;

            line_buffer += *it;

            do
            {
                ++it;

                line_buffer += *it;
            }
            while (!(*it == '"' && *std::prev(it) != '\\'));

            ++it;

            continue;
        }

        if (*it == '/' && std::next(it) != std::end(string) && *std::next(it) == '/')
        {
            while (it != std::end(string) && *it != '\n')
            {
                ++it;
            }

            continue;
        }

        if (*it == '/' && std::next(it) != std::end(string) && *std::next(it) == '*')
        {
            auto end = std::next(it, 3);

            while (!(*end == '/' && *std::prev(end) == '*'))
            {
                ++end;
            }

            it = ++end;

            continue;
        }

        if (*it == '\n')
        {
            ++it;

            flush(false);

            continue;
        }

        if (!std::isspace(static_cast < unsigned char > (*it))) line_has_content = true;

        line_buffer += *it++;
    }

    if (line_has_content)
    {
        output += line_buffer;
    }

    std::fstream(path_2, std::ios::out) << output;
}

int main()
{
    auto const path_1 = std::string("13_03_input.cpp");
    auto const path_2 = std::string("13_03_output.cpp");

    {
        std::fstream stream(path_1, std::ios::out);

        stream <<
            "// full line comment\n"
            "int x = 5; // inline comment\n"
            "\n"
            "int y = /* block */ 10;\n"
            "   \n"
            "const char* s = \"hello // world\";\n"
            "const char* r = R\"(raw\n"
            "// preserved\n"
            "\n"
            "content)\";\n"
            "int z = 3;\n";
    }

    transform(path_1, path_2);

    auto output = (std::stringstream() << std::fstream(path_2, std::ios::in).rdbuf()).str();

    assert(output.find("// full line comment") == std::string::npos);
    assert(output.find("// inline comment") == std::string::npos);
    assert(output.find("/* block */") == std::string::npos);
    assert(output.find("int x = 5;") != std::string::npos);
    assert(output.find("int y =") != std::string::npos);
    assert(output.find("hello // world") != std::string::npos);
    assert(output.find("// preserved") != std::string::npos);
    assert(output.find("// preserved\n\ncontent)\"") != std::string::npos);
    assert(output.find("int z = 3;") != std::string::npos);

    std::filesystem::remove(path_1);
    std::filesystem::remove(path_2);

    std::print("All tests passed\n");
}
