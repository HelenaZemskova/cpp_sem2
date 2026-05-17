// g++ -std=c++23 -Wall -Wextra -pedantic 12_01.cpp -o 12_01

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <locale>
#include <optional>
#include <sstream>
#include <string>

constexpr auto exchange_rate = 72.77L; // 1 USD = 72.77 RUB

void replace_all(std::string & s, std::string const & from, std::string const & to)
{
    if (from.empty()) return;

    for (std::size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
        s.replace(pos, from.size(), to);
}

auto normalize_space_bytes(std::string s) -> std::string
{
    replace_all(s, "\xC2\xA0", " "); // UTF-8 no-break space
    replace_all(s, "\xE2\x80\xAF", " "); // UTF-8 narrow no-break space
    replace_all(s, "\xE2\x80\x89", " "); // UTF-8 thin space
    replace_all(s, "\xEF\xBF\xBD", " "); // UTF-8 replacement character
    std::replace(std::begin(s), std::end(s), '\xA0', ' '); // single-byte no-break space
    return s;
}

auto trim_spaces(std::string s) -> std::string
{
    s = normalize_space_bytes(std::move(s));

    auto is_space = [](unsigned char c)
    {
        return std::isspace(c);
    };

    while (!s.empty() && is_space(static_cast<unsigned char>(s.front()))) s.erase(std::begin(s));
    while (!s.empty() && is_space(static_cast<unsigned char>(s.back ()))) s.pop_back();

    return s;
}

auto normalize_nbsp_for_console(std::string s) -> std::string
{
    s = normalize_space_bytes(std::move(s));

    // терминал не поддерживает локальный разделитель тысяx - заменяем любой "нечисловой" разделитель между цифрами на обычный пробел
    for (std::size_t i = 0; i + 1 < s.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) continue;

        auto j = i + 1;
        while (j < s.size()
            && !std::isdigit(static_cast<unsigned char>(s[j]))
            && s[j] != ','
            && s[j] != '.')
        {
            ++j;
        }

        if (j > i + 1 && j < s.size() && std::isdigit(static_cast<unsigned char>(s[j])))
        {
            s.replace(i + 1, j - (i + 1), " ");
            i = i + 1;
        }
    }

    return trim_spaces(std::move(s));
}

auto parse_money(std::string const & text, std::locale const & locale, bool intl) -> std::optional<long double>
{
    std::array<std::string, 2> candidates = { text, text };
    std::replace(std::begin(candidates[1]), std::end(candidates[1]), ' ', '\xA0');

    for (auto const & candidate : candidates)
    {
        std::stringstream ss(candidate);
        ss.imbue(locale);

        long double amount = 0.0L;
        ss >> std::get_money(amount, intl);
        if (ss.fail()) continue;

        auto rest = std::string();
        std::getline(ss, rest, '\0');
        if (trim_spaces(rest).empty()) return amount;
    }

    return std::nullopt;
}

auto parse_plain_rubles(std::string text) -> std::optional<long double>
{
    text = normalize_space_bytes(std::move(text));
    text.erase(std::remove(std::begin(text), std::end(text), ' '), std::end(text));
    std::replace(std::begin(text), std::end(text), ',', '.');

    if (text.empty()) return std::nullopt;

    try
    {
        std::size_t parsed = 0;
        auto const rubles  = std::stold(text, &parsed);
        if (parsed != text.size()) return std::nullopt;

        return rubles * 100.0L;
    }
    catch (...)
    {
        return std::nullopt;
    }
}

auto parse_rub(std::string text, std::locale const & locale) -> std::optional<long double>
{
    text = trim_spaces(text);

    if (text.rfind("RUB", 0) == 0) text = trim_spaces(text.substr(3));
    if (text.size() >= 3 && text.substr(text.size() - 3) == "RUB")
        text = trim_spaces(text.substr(0, text.size() - 3));

    if (auto amount = parse_money(text, locale, false)) return amount;
    if (auto amount = parse_money(text, locale, true )) return amount;
    if (auto amount = parse_plain_rubles(text)) return amount;

    return std::nullopt;
}

int main()
{
    std::locale const rub_locale("ru_RU.utf8");
    std::locale const usd_locale("en_US.utf8");

    auto to_string = [](std::locale const & locale, long double amount, bool intl = false, bool show_base = false)
    {
        std::stringstream ss;
        ss.imbue(locale);
        if (show_base) ss << std::showbase;
        ss << std::put_money(amount, intl);
        return ss.str();
    };

    auto convert = [&](long double rub_minor, bool intl = false)
    {
        return to_string(usd_locale, rub_minor / exchange_rate, intl, true);
    };

    {
        auto rub_input = to_string(rub_locale, 900000.0L, false, false);
        auto rub_output = to_string(rub_locale, 900000.0L, false, true);
        auto amount = parse_money(rub_input, rub_locale, false);

        assert(amount);
        assert(std::abs(*amount - 900000.0L) < 1.0L);
        assert(std::abs(*amount / exchange_rate - (900000.0L / exchange_rate)) < 1.0L);

        std::cout << normalize_nbsp_for_console(rub_output) << " -> " << convert(*amount) << "\n";
    }

    {
        auto amount_1 = parse_rub("RUB 9000", rub_locale);
        auto amount_2 = parse_rub("9000 RUB", rub_locale);

        assert(amount_1 && amount_2);
        assert(std::abs(*amount_1 - 900000.0L) < 1.0L);
        assert(std::abs(*amount_2 - 900000.0L) < 1.0L);

        auto rub_1 = to_string(rub_locale, *amount_1, false, true);
        auto rub_2 = to_string(rub_locale, *amount_2, false, true);

        std::cout << "RUB 9000 (" << normalize_nbsp_for_console(rub_1) << ") -> " << convert(*amount_1) << "\n";
        std::cout << "9000 RUB (" << normalize_nbsp_for_console(rub_2) << ") -> " << convert(*amount_2) << "\n";
    }

    std::cout << "Enter RUB amount (e.g. \"RUB 180\" or \"180 RUB\"): ";

    std::string input;
    if (std::getline(std::cin >> std::ws, input))
    {
        if (auto amount = parse_rub(input, rub_locale))
            std::cout << normalize_nbsp_for_console(to_string(rub_locale, *amount, false, true))
                      << " -> " << convert(*amount) << "\n";
        else
            std::cout << "Cannot parse input as RUB amount\n";
    }

    std::cout << "All tests passed\n";
}
