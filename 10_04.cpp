// g++ -std=c++23 -Wall -Wextra -o 10_04 10_04.cpp -I/usr/local/include && ./10_04

// Здесь считаем N-е число Фибоначчи через матрицу:
//   M = |1 1|
//       |1 0|
//
// Для этой матрицы работает формула:
//   M^n = |F(n+1) F(n)  |
//         |F(n)   F(n-1)|
//
// Поэтому берём элемент (0, 1) из M^n: F(n) = (M^n)(0, 1), а F(0) = 0.
//
// Степень считаем быстрым двоичным возведением.
// Это даёт сложность O(log N), потому что на каждом шаге степень делится пополам.
// Если сравнить с другими подходами: 
// наивная рекурсия: O(2^N), очень медленно; 
// обычный итеративный подсчёт: O(N), просто и нормально; 
// матричный метод: O(log N), быстро и точно; 
// формула Бине: O(1), но для больших N теряет точность из-за ошибок округления
// Для unsigned long long корректно считается до F(93)

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/operation.hpp>

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <print>
#include <string>

using value_t = unsigned long long int;
using Matrix  = boost::numeric::ublas::matrix < value_t >;


auto make_identity() -> Matrix
{
    Matrix m(2, 2, 0);

    m(0, 0) = 1;
    m(1, 1) = 1;

    return m;
}

auto make_base() -> Matrix
{
    Matrix m(2, 2);

    m(0, 0) = 1;  m(0, 1) = 1;
    m(1, 0) = 1;  m(1, 1) = 0;

    return m;
}

auto mat_pow(Matrix base, std::size_t n) -> Matrix
{
    auto result = make_identity();

    while (n > 0)
    {
        if (n & 1uz)
        {
            result = boost::numeric::ublas::prod(result, base);
        }

        base = boost::numeric::ublas::prod(base, base);

        n >>= 1;
    }

    return result;
}


auto fib_matrix(std::size_t n) -> value_t
{
    if (n == 0) return value_t{0};

    return mat_pow(make_base(), n)(0, 1);
}


auto fib_iterative(std::size_t n) -> value_t
{
    if (n == 0) return value_t{0};

    auto a = value_t{0};
    auto b = value_t{1};

    for (auto i = 1uz; i < n; ++i)
    {
        auto c = a + b;

        a = b;
        b = c;
    }

    return b;
}


auto fib_recursive(std::size_t n) -> value_t
{
    if (n <= 1) return static_cast < value_t > (n);

    return fib_recursive(n - 1) + fib_recursive(n - 2);
}


int main()
{

    std::print("____ Fibonacci: three algorithms, small N ____ \n\n");

    std::print("{:>4}  {:>20}  {:>20}  {:>14}\n",
               "N", "matrix O(log N)", "iterative O(N)", "recursive O(2^N)");

    std::print("{}\n", std::string(65, '-'));

    for (auto n = 0uz; n <= 20uz; ++n)
    {
        auto mat  = fib_matrix   (n);
        auto iter = fib_iterative(n);
        auto rec  = fib_recursive(n);

        std::print("{:>4}  {:>20}  {:>20}  {:>14}\n", n, mat, iter, rec);

        assert(mat == iter && iter == rec);
    }


    std::print("\n____ Large N (recursive is infeasible) ____ \n\n");

    std::print("{:>4}  {:>25}  {:>25}\n",
               "N", "matrix O(log N)", "iterative O(N)");

    std::print("{}\n", std::string(60, '-'));

    for (auto n : std::initializer_list < std::size_t >
         { 30, 40, 50, 60, 70, 80, 90, 93 })
    {
        auto mat  = fib_matrix   (n);
        auto iter = fib_iterative(n);

        std::print("{:>4}  {:>25}  {:>25}\n", n, mat, iter);

        assert(mat == iter);
    }

    std::print("\nAll assertions passed.\n");

    std::print("\n____ Matrix M^n (illustrating the identity) ____ \n\n");

    for (auto n = 1uz; n <= 6uz; ++n)
    {
        auto m = mat_pow(make_base(), n);

        std::print("M^{} = | {:>3}  {:>3} |   ->  F({}) = {}\n",
                   n, m(0, 0), m(0, 1), n, m(0, 1));

        std::print("       | {:>3}  {:>3} |\n\n",
                   m(1, 0), m(1, 1));
    }
    std::print("All tests passed successfully.\n");
}
