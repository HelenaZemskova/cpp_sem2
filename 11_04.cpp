// g++ -std=c++23 -Wall -Wextra -pedantic 11_04.cpp -o 11_04

#include <cassert>
#include <cmath>
#include <iostream>
#include <optional>
#include <utility>
#include <variant>

constexpr auto epsilon = 1e-12;

using roots_t = std::variant < std::monostate, double, std::pair < double, double > >;

auto solve(double a, double b, double c) -> std::optional < roots_t >
{
    if (std::abs(a) < epsilon)
    {
        if (std::abs(b) < epsilon)
        {
            if (std::abs(c) < epsilon)
                return roots_t { std::monostate{} };
            else
                return std::nullopt;
        }
        else
        {
            return roots_t { -c / b };
        }
    }
    else
    {
        auto discriminant = b * b - 4.0 * a * c;

        if (discriminant > epsilon)
        {
            auto sqrtD = std::sqrt(discriminant);

            return roots_t { std::pair { (-b + sqrtD) / (2.0 * a),
                                         (-b - sqrtD) / (2.0 * a) } };
        }
        else if (discriminant < -epsilon)
        {
            return std::nullopt;
        }
        else
        {
            return roots_t { -b / (2.0 * a) };
        }
    }
}

class Visitor
{
public :

    void operator()(std::monostate) const
    {
        std::cout << "Infinite solutions" << std::endl;
    }

    void operator()(double root) const
    {
        std::cout << root << std::endl;
    }

    void operator()(std::pair < double, double > roots) const
    {
        std::cout << roots.first << " " << roots.second << std::endl;
    }
};

int main()
{
    {
        assert(!solve(1, 0, 1));

        assert(std::holds_alternative < std::monostate > (*solve(0, 0, 0)));

        assert(!solve(0, 0, 1));

        assert(std::holds_alternative < double > (*solve(0, 2, -4)));
        assert(std::abs(std::get < double > (*solve(0, 2, -4)) - 2.0) < epsilon);

        assert(std::holds_alternative < double > (*solve(1, 2, 1)));
        assert(std::abs(std::get < double > (*solve(1, 2, 1)) - (-1.0)) < epsilon);

        assert((std::holds_alternative < std::pair < double, double > > (*solve(1, -3, 2))));
        {
            auto [x1, x2] = std::get < std::pair < double, double > > (*solve(1, -3, 2));
            assert(std::abs(x1 - 2.0) < epsilon && std::abs(x2 - 1.0) < epsilon);
        }
    }

    if (auto result = solve(1.0, -3.0, 2.0); result)
    {
        std::visit(Visitor{}, *result);
    }

    std::cout << "All tests passed\n";
}
