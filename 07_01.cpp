#include <cmath>
#include <iostream>
#include <optional>
#include <utility>
#include <variant>

////////////////////////////////////////////////////////////////////////////////

constexpr auto epsilon = 1e-12;

////////////////////////////////////////////////////////////////////////////////

// std::monostate                           бесконечное количество корней
// double                                   один корень
// std::pair < double, double >             два корня
// std::nullopt                             нет корней

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

int main()
{
    double a, b, c;

    std::cin >> a >> b >> c;

    auto result = solve(a, b, c);

    if (!result)
    {
        std::cout << "No solution" << std::endl;
    }
    else if (std::holds_alternative < std::monostate > (*result))
    {
        std::cout << "Infinite solutions" << std::endl;
    }
    else if (std::holds_alternative < double > (*result))
    {
        std::cout << std::get < double > (*result) << std::endl;
    }
    else if (std::holds_alternative < std::pair < double, double > > (*result))
    {
        auto [x1, x2] = std::get < std::pair < double, double > > (*result);

        std::cout << x1 << " " << x2 << std::endl;
    }
}
