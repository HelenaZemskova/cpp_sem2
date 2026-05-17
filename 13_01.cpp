// g++ -std=c++23 -Wall -Wextra -pedantic 13_01.cpp -o 13_01

#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


auto bytes_to_hex(std::vector < std::uint8_t > const & bytes) -> std::string
{
    std::stringstream stream;

    for (auto const byte : bytes)
    {
        stream << std::setw(2) << std::right << std::setfill('0') << std::hex
               << static_cast < unsigned int > (byte);
    }

    return stream.str();
}


auto hex_to_bytes(std::string const & hex) -> std::vector < std::uint8_t >
{
    std::vector < std::uint8_t > bytes;

    auto digit = [](char c) -> std::uint8_t
    {
        if (c <= '9') return static_cast < std::uint8_t > (c - '0');

        return static_cast < std::uint8_t > (c - 'a' + 10);
    };

    for (std::size_t i = 0; i < hex.size(); i += 2)
    {
        bytes.push_back(static_cast < std::uint8_t > ((digit(hex[i]) << 4) | digit(hex[i + 1])));
    }

    return bytes;
}


int main()
{
    assert(bytes_to_hex({}) == "");

    assert(bytes_to_hex({ 0x00 }) == "00");

    assert(bytes_to_hex({ 0xff }) == "ff");

    assert(bytes_to_hex({ 0xde, 0xad, 0xbe, 0xef }) == "deadbeef");

    assert(bytes_to_hex({ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }) == "0123456789abcdef");

    assert(hex_to_bytes("") == std::vector < std::uint8_t > {});

    assert(hex_to_bytes("00") == std::vector < std::uint8_t > { 0x00 });

    assert(hex_to_bytes("ff") == std::vector < std::uint8_t > { 0xff });

    assert((hex_to_bytes("deadbeef") == std::vector < std::uint8_t > { 0xde, 0xad, 0xbe, 0xef }));

    assert((hex_to_bytes("0123456789abcdef") == std::vector < std::uint8_t > { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }));

    auto const original = std::vector < std::uint8_t > { 0x01, 0xab, 0xff, 0x00, 0x42 };

    assert(hex_to_bytes(bytes_to_hex(original)) == original);

    std::cout << "All tests passed\n";
}
