// g++ -std=c++23 -Wall -Wextra -pedantic 12_03.cpp -o 12_03

#include <cassert>
#include <cstddef>
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <vector>

auto longest_palindrome(std::string_view s) -> std::string_view
{
    auto const n = s.size();

    if (n == 0) return s;

    std::vector < bool > dp(n * n, false);

    auto start = 0uz;
    auto max_len = 1uz;

    for (auto i = 0uz; i < n; ++i)
        dp[i * n + i] = true;

    for (auto i = 0uz; i + 1 < n; ++i)
    {
        if (s[i] == s[i + 1])
        {
            dp[i * n + i + 1] = true;
            start = i;
            max_len = 2;
        }
    }

    for (auto len = 3uz; len <= n; ++len)
    {
        for (auto i = 0uz; i + len <= n; ++i)
        {
            auto j = i + len - 1;

            if (s[i] == s[j] && dp[(i + 1) * n + (j - 1)])
            {
                dp[i * n + j] = true;

                if (len > max_len)
                {
                    start = i;
                    max_len = len;
                }
            }
        }
    }

    return s.substr(start, max_len);
}

auto is_palindrome(std::string_view s) -> bool
{
    if (s.empty()) return true;

    for (auto i = 0uz, j = s.size() - 1; i < j; ++i, --j)
        if (s[i] != s[j]) return false;

    return true;
}

int main()
{

    {
        auto r = longest_palindrome("racecar");
        assert(r == "racecar");
        assert(is_palindrome(r) && r.size() == 7);
    }

    {
        auto r = longest_palindrome("cbbd");
        assert(r == "bb");
        assert(is_palindrome(r) && r.size() == 2);
    }

    {
        auto r = longest_palindrome("babad");
        assert(is_palindrome(r) && r.size() == 3);
    }

    {
        auto r = longest_palindrome("abcba");
        assert(r == "abcba");
        assert(is_palindrome(r) && r.size() == 5);
    }

    {
        auto r = longest_palindrome("abacaba");
        assert(r == "abacaba");
        assert(is_palindrome(r) && r.size() == 7);
    }

    {
        auto r = longest_palindrome("aaaa");
        assert(r == "aaaa");
        assert(is_palindrome(r) && r.size() == 4);
    }

    {
        auto r = longest_palindrome("a");
        assert(is_palindrome(r) && r.size() == 1);
    }

    {
        assert(longest_palindrome("").empty());
    }

    for (auto test : std::initializer_list < std::string_view >
    {
        "racecar", "babad", "cbbd", "abcba", "abacaba", "aaaa", "a"
    })
    {
        std::print("{:10} -> {}\n", test, longest_palindrome(test));
    }

    std::print("Enter text for manual test (empty line to skip): ");
    std::string input;
    if (std::getline(std::cin, input) && !input.empty())
    {
        std::print("Longest palindrome: {}\n", longest_palindrome(input));
    }

    std::print("All tests passed\n");
}
