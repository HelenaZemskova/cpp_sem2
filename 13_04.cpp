// g++ -std=c++23 -Wall -Wextra -pedantic 13_04.cpp -o 13_04
// printf "^13_0[1-4]\\.cpp$\n" | ./13_04

// grep удобен как внешняя утилита для фильтрации уже полученного текстового потока:
//      ls -1 | grep -E "<pattern>"
// std::regex + std::filesystem удобны внутри C++:
//    - можно работать не только с именами, но и с метаданными файлов
//    - легко комбинировать фильтрацию с дополнительной логикой программы
//    - решение переносимо и не требует внешних процессов

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <format>
#include <iostream>
#include <iterator>
#include <print>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

auto make_type(std::filesystem::file_status const & status)
{
    if (std::filesystem::is_directory (status)) { return 'd'; }

    if (std::filesystem::is_regular_file(status)) { return 'f'; }

    if (std::filesystem::is_symlink (status)) { return 'l'; }

    return '?';
}

auto make_permissions(std::filesystem::perms permissions) -> std::string
{
    auto lambda = [permissions](auto x, auto y)
    {
        return (permissions & x) == std::filesystem::perms::none ? '-' : y;
    };

    return
    {
        lambda(std::filesystem::perms::owner_read, 'r'),

        lambda(std::filesystem::perms::owner_write, 'w'),

        lambda(std::filesystem::perms::owner_exec, 'x')
    };
}

auto size(std::filesystem::path const & path)
{
	auto size = 0uz;

	if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
	{
		for (auto const & entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (!std::filesystem::is_directory(entry.status()))
			{
				size += std::filesystem::file_size(entry);
			}
		}
	}

	return size;
}

auto size(std::filesystem::directory_entry const & entry)
{
    auto size = 0uz;

    if (std::filesystem::is_regular_file(entry.status()))
    {
        size = std::filesystem::file_size(entry);
    }
    else
    {
        size = ::size(entry.path());
    }

    std::vector < char > vector = { 'B', 'K', 'M', 'G' };

    auto i = 0uz;

    while (i < std::size(vector) && size >= 1 << 10)
    {
        size /= (1 << 10);

        ++i;
    }

    return (std::stringstream() << std::format("{: >4} ({})", size, vector[i])).str();
}

auto collect(std::filesystem::path const & path, std::regex const & pattern) -> std::vector < std::string >
{
    std::vector < std::string > result;

    if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
    {
        for (auto const & entry : std::filesystem::directory_iterator(path))
        {
            auto const filename = entry.path().filename().string();

            if (std::regex_search(filename, pattern))
            {
                result.push_back(filename);
            }
        }
    }

    return result;
}

void show(std::filesystem::path const & path, std::regex const & pattern)
{
	if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
	{
        for (auto const & entry : std::filesystem::directory_iterator(path))
		{
            auto const filename = entry.path().filename().string();

            if (!std::regex_search(filename, pattern)) continue;

            std::print
            (
                "show : entry : {} | {} | {} | {} | {}\n",

                make_type(entry.status()),

                make_permissions(entry.status().permissions()),

                size(entry),

                std::chrono::floor < std::chrono::seconds >
                (
                    std::chrono::file_clock::to_sys(entry.last_write_time())
                ),

			    filename
            );
		}
	}
}

int main()
{
    auto const path = std::filesystem::current_path();

    {
        auto const files = collect(path, std::regex(R"(\.cpp$)"));

        assert(!files.empty());

        assert(std::ranges::all_of(files, [](auto const & f)
        {
            return f.size() >= 4 && f.substr(f.size() - 4) == ".cpp";
        }));
    }

    {
        auto const files = collect(path, std::regex(R"(^13_04\.cpp$)"));

        assert(files.size() == 1);

        assert(files.front() == "13_04.cpp");
    }

    {
        auto const files = collect(path, std::regex(R"(xyzzy_no_match_expected)"));

        assert(files.empty());
    }

    std::print("Enter regex pattern for directory entries: ");
    std::string pattern_text;
    std::getline(std::cin >> std::ws, pattern_text);
    std::print("grep equivalent: ls -1 | grep -E \"{}\"\n", pattern_text);

    try
    {
        auto const pattern = std::regex(pattern_text);
        show(path, pattern);
    }
    catch (std::regex_error const &)
    {
        std::print("Invalid regex pattern\n");
    }

    std::print("All tests passed\n");
}