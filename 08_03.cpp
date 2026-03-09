// g++ -std=c++23 -Wall -Wextra -o 08_03 08_03.cpp && ./08_03

#include <cassert>
#include <climits>
#include <cmath>
#include <iostream>
#include <limits>
#include <print>

int log2_int(int n)
{
    auto x = static_cast < unsigned int > (n);

    int result = 0;

    while (x >>= 1u) ++result;

    return result;
}

int log2_float(float number)
{
    union { float f; unsigned int u; } bits;

    bits.f = number;

    unsigned int const u        =  bits.u;

    unsigned int const exponent = (u >> 23u) & 0xFFu;
    unsigned int const mantissa =  u         & 0x7FFFFFu;

    if (exponent == 0xFFu)
    {
        return INT_MIN;
    }
    else if (exponent > 0u)
    {
        return static_cast < int > (exponent) - 127;
    }
    else
    {
        int bit_pos = 0;

        unsigned int m = mantissa;

        while (m >>= 1u) ++bit_pos;

        return bit_pos - 149;
    }
}

int main()
{
    std::print("floor(log2) для int\n\n");

    assert(log2_int(1) == 0);
    assert(log2_int(2) == 1);
    assert(log2_int(4) == 2);
    assert(log2_int(8) == 3);
    assert(log2_int(1024) == 10);
    assert(log2_int(1 << 30) == 30);

    assert(log2_int(3) == 1);
    assert(log2_int(5) == 2);
    assert(log2_int(7) == 2);
    assert(log2_int(1000) == 9);
    assert(log2_int(INT_MAX) == 30);

    for (int i = 0; i <= 15; ++i)
        std::print("log2_int({:6}) = {}\n", (1 << i), log2_int(1 << i));

    std::print("\n floor(log2) для float \n\n");
    std::print("Нормализованные числа (0 < c < 255):\n");

    assert(log2_float(1.0f) == 0);
    assert(log2_float(2.0f) == 1);
    assert(log2_float(4.0f) == 2);
    assert(log2_float(0.5f) == -1);
    assert(log2_float(0.25f) == -2);
    assert(log2_float(1024.0f) ==  10);

    assert(log2_float(3.0f) == 1);
    assert(log2_float(0.75f) == -1);
    assert(log2_float(1000.0f) == 9);

    float const norm_min = std::numeric_limits<float>::min();

    assert(log2_float(norm_min) == -126);

    for (auto [v, expected] : std::initializer_list<std::pair<float,int>>{
            {0.125f, -3}, {0.5f, -1}, {1.0f, 0}, {2.0f, 1}, {4.0f, 2}, {1000.0f, 9}})
        std::print("log2_float({:10.4f}) = {:4}  (expected {})\n",
                   v, log2_float(v), expected);

    std::print("\nДенормализованные числа (c == 0):\n");

    float const denorm_min = std::numeric_limits<float>::denorm_min();

    assert(log2_float(denorm_min) == -149);

    float const denorm_2 = denorm_min * 2.0f;
    float const denorm_4 = denorm_min * 4.0f;
    float const denorm_hi = norm_min * 0.5f;

    assert(log2_float(denorm_2) == -148);
    assert(log2_float(denorm_4) == -147);
    assert(log2_float(denorm_hi) == -127);

    std::print("log2_float(denorm_min) = {} (2^-149)\n", log2_float(denorm_min));
    std::print("log2_float(2×denorm_min) = {} (2^-148)\n", log2_float(denorm_2));
    std::print("log2_float(4×denorm_min) = {} (2^-147)\n", log2_float(denorm_4));
    std::print("log2_float(norm_min/2) = {} (2^-127)\n", log2_float(denorm_hi));

    std::print("\nСпециальные значения (c == 255):\n");

    float const pos_inf = std::numeric_limits<float>::infinity();
    float const neg_inf = -std::numeric_limits<float>::infinity();
    float const nan_val = std::numeric_limits<float>::quiet_NaN();

    assert(log2_float(pos_inf) == INT_MIN);
    assert(log2_float(neg_inf) == INT_MIN);
    assert(log2_float(nan_val) == INT_MIN);

    std::print("log2_float(+inf) = {} (INT_MIN ошибка)\n", log2_float(pos_inf));
    std::print("log2_float(-inf) = {} (INT_MIN ошибка)\n", log2_float(neg_inf));
    std::print("log2_float( nan) = {} (INT_MIN ошибка)\n", log2_float(nan_val));

    std::print("\nAll tests passed successfully!\n");
}
