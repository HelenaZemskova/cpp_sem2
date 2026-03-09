#include <cassert>
#include <cmath>
#include <compare>
#include <iostream>
#include <istream>
#include <limits>
#include <numeric>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

class Exception : public std::exception
{
public:
    explicit Exception(std::string message) : m_message(std::move(message)) {}

    const char * what() const noexcept override
    {
        return m_message.c_str();
    }

private:
    std::string m_message;
};

template<typename Derived>
struct addable
{
    friend Derived operator+(Derived lhs, Derived const & rhs) { return lhs += rhs; }
};

template<typename Derived>
struct subtractable
{
    friend Derived operator-(Derived lhs, Derived const & rhs) { return lhs -= rhs; }
};

template<typename Derived>
struct multipliable
{
    friend Derived operator*(Derived lhs, Derived const & rhs) { return lhs *= rhs; }
};

template<typename Derived>
struct dividable
{
    friend Derived operator/(Derived lhs, Derived const & rhs) { return lhs /= rhs; }
};

template<typename Derived>
struct incrementable
{
    friend Derived & operator++(Derived & x)       { return x += Derived(1); }
    friend Derived   operator++(Derived & x, int)  { Derived t(x); ++x; return t; }
};

template<typename Derived>
struct decrementable
{
    friend Derived & operator--(Derived & x)       { return x -= Derived(1); }
    friend Derived   operator--(Derived & x, int)  { Derived t(x); --x; return t; }
};

template<typename T>
class Rational
    : public addable      <Rational<T>>
    , public subtractable <Rational<T>>
    , public multipliable <Rational<T>>
    , public dividable    <Rational<T>>
    , public incrementable<Rational<T>>
    , public decrementable<Rational<T>>
{
public:
    Rational(T num = 0, T den = 1) : m_num(num), m_den(den)
    {
        if (m_den == T{0})
            throw Exception("Rational: denominator must not be zero");

        reduce();
    }

    explicit operator double() const { return 1.0 * m_num / m_den; }

    auto & operator+=(Rational const & other)
    {
        auto lcm = std::lcm(m_den, other.m_den);
        m_num = m_num * (lcm / m_den) + other.m_num * (lcm / other.m_den);
        m_den = lcm;
        reduce();
        return *this;
    }

    auto & operator-=(Rational const & other)
    {
        return *this += Rational(other.m_num * -1, other.m_den);
    }

    auto & operator*=(Rational const & other)
    {
        m_num *= other.m_num;
        m_den *= other.m_den;
        reduce();
        return *this;
    }

    auto & operator/=(Rational const & other)
    {
        return *this *= Rational(other.m_den, other.m_num);
    }

    friend std::strong_ordering operator<=>(Rational const & lhs, Rational const & rhs)
    {
        auto left  = static_cast<long long>(lhs.m_num) * rhs.m_den;
        auto right = static_cast<long long>(rhs.m_num) * lhs.m_den;
        if (left < right) return std::strong_ordering::less;
        if (left > right) return std::strong_ordering::greater;
        return std::strong_ordering::equal;
    }

    friend bool operator==(Rational const & lhs, Rational const & rhs)
    {
        return lhs.m_num == rhs.m_num && lhs.m_den == rhs.m_den;
    }

    friend auto & operator>>(std::istream & stream, Rational & r)
    {
        return (stream >> r.m_num).ignore() >> r.m_den;
    }

    friend auto & operator<<(std::ostream & stream, Rational const & r)
    {
        return stream << r.m_num << '/' << r.m_den;
    }

private:
    void reduce()
    {
        if (m_den < 0) { m_num = -m_num; m_den = -m_den; }
        auto gcd = std::gcd(m_num, m_den);
        m_num /= gcd;
        m_den /= gcd;
    }

    T m_num = 0, m_den = 1;
};


void demonstrate(std::string const & title, auto action)
{
    std::cerr << "\n--- " << title << " ---\n";
    try
    {
        action();
    }
    catch (std::exception const & e)
    {
        std::cerr << "Caught std::exception: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
    }
}

int main()
{
    // Пользовательское исключение Exception 
    // конструктор Rational бросает Exception если в знаменателе ноль

    demonstrate("Exception (zero denominator in Rational)", []
    {
        Rational<int> bad(1, 0);
    });

    // bad_alloc
    // оператор new не может выделить запрошенный объём памяти, когда запрашиваемый размер
    // (size_t::max / 2 байт) значительно превышает объём памяти системы

    demonstrate("std::bad_alloc (impossible allocation)", []
    {
        [[maybe_unused]] auto * p =
            ::operator new(std::numeric_limits<std::size_t>::max() / 2);
    });

    // bad_variant_access 
    // std::get<T> бросает bad_variant_access, когда текущая
    // альтернатива варианта не совпадает с типом T

    demonstrate("std::bad_variant_access (wrong alternative)", []
    {
        std::variant<int, double> v = 42; // вариант хранит int

        [[maybe_unused]] auto x = std::get<double>(v);  // мы запрашиваем double
    });

    // bad_optional_access
    // метод .value() бросает bad_optional_access, когда опционал не содержит значения

    demonstrate("std::bad_optional_access (empty optional)", []
    {
        std::optional<int> opt;

        [[maybe_unused]] auto x = opt.value();  // обращение к пустому опционалу
    });

    // length_error 
    // std::vector::resize бросает length_error, когда запрошенный размер превышает максимально допустимый 
    // размер контейнера max_size()
    // Значение size_t::max всегда больше max_size() любого вектора

    demonstrate("std::length_error (vector resize beyond max_size)", []
    {
        std::vector<int> v;

        v.resize(std::numeric_limits<std::size_t>::max());
    });

    // std::out_of_range
    // std::vector::at бросает out_of_range при обращении
    // к элементу по индексу, выходящему за пределы [0, size()-1].
    // Вектор содержит 3 элемента - индекс 10 недопустим

    demonstrate("std::out_of_range (vector::at with invalid index)", []
    {
        std::vector<int> v = { 1, 2, 3 };

        [[maybe_unused]] auto x = v.at(10);
    });

    Rational<int> x(1), y(2);

    assert(x + y == Rational<int>(3));
    assert(x - y == Rational<int>(-1));
    assert(x * y == Rational<int>(2));
    assert(x / y == Rational<int>(1, 2));
    assert((++x) == Rational<int>(2));
    assert((x--) == Rational<int>(2));

    std::cout << "\nAll correct tests passed successfully!" << std::endl;
}
