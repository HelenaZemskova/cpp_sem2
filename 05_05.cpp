#include <cassert>
#include <cmath>
#include <compare>
#include <iostream>
#include <istream>
#include <numeric>
#include <ostream>
#include <sstream>
#include <vector>

// operator+ из operator+=
template<typename Derived>
struct addable
{
    friend Derived operator+(Derived lhs, Derived const & rhs)
    {
        return lhs += rhs;
    }
};

// operator- из operator-=
template<typename Derived>
struct subtractable
{
    friend Derived operator-(Derived lhs, Derived const & rhs)
    {
        return lhs -= rhs;
    }
};

// operator* из operator*=
template<typename Derived>
struct multipliable
{
    friend Derived operator*(Derived lhs, Derived const & rhs)
    {
        return lhs *= rhs;
    }
};

// operator/ из operator/=
template<typename Derived>
struct dividable
{
    friend Derived operator/(Derived lhs, Derived const & rhs)
    {
        return lhs /= rhs;
    }
};

// prefix operator++ и postfix operator++ из operator+=
// Prefix через operator+=(Derived(1));
// Postfix через prefix
template<typename Derived>
struct incrementable
{
    friend Derived & operator++(Derived & x)
    {
        return x += Derived(1);
    }

    friend Derived operator++(Derived & x, int)
    {
        Derived tmp(x);
        ++x;
        return tmp;
    }
};

// prefix operator-- и postfix operator-- из operator-=
template<typename Derived>
struct decrementable
{
    friend Derived & operator--(Derived & x)
    {
        return x -= Derived(1);
    }

    friend Derived operator--(Derived & x, int)
    {
        Derived tmp(x);
        --x;
        return tmp;
    }
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
        reduce();
    }

    explicit operator double() const
    {
        return 1.0 * m_num / m_den;
    }

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

    friend auto & operator>>(std::istream & stream, Rational & rational)
    {
        return (stream >> rational.m_num).ignore() >> rational.m_den;
    }

    friend auto & operator<<(std::ostream & stream, Rational const & rational)
    {
        return stream << rational.m_num << '/' << rational.m_den;
    }

private:
    void reduce()
    {
        if (m_den < 0)
        {
            m_num = -m_num;
            m_den = -m_den;
        }

        auto gcd = std::gcd(m_num, m_den);
        m_num /= gcd;
        m_den /= gcd;
    }

    T m_num = 0, m_den = 1;
};

// Тестики
auto equal(double x, double y, double epsilon = 1e-6)
{
    return std::abs(x - y) < epsilon;
}

int main()
{
    Rational<int> x = 1, y(2, 1);

    std::vector<int> vector_2(5);
    std::vector<int> vector_3 = { 1, 2, 3, 4, 5 };

    assert(equal(static_cast<double>(x), 1));

    assert((x += y) == Rational<int>(+3, 1));
    assert((x -= y) == Rational<int>(+1, 1));
    assert((x *= y) == Rational<int>(+2, 1));
    assert((x /= y) == Rational<int>(+1, 1));

    assert((x++) == Rational<int>(+1, 1));
    assert((x--) == Rational<int>(+2, 1));
    assert((++y) == Rational<int>(+3, 1));
    assert((--y) == Rational<int>(+2, 1));

    [[maybe_unused]] auto z = 0;

    assert((x + y) == Rational<int>(+3, 1));
    assert((x - y) == Rational<int>(-1, 1));
    assert((x * y) == Rational<int>(+2, 1));
    assert((x / y) == Rational<int>(+1, 2));

    assert((x += 1) == Rational<int>(+2, 1));
    assert((x + 1) == Rational<int>(+3, 1));
    assert((1 + y) == Rational<int>(+3, 1));
    assert((1 + 1) == Rational<int>(+2, 1));

    assert((x <=> y) == std::strong_ordering::equal);
    assert((x <=> Rational<int>(1, 1)) == std::strong_ordering::greater);
    assert((Rational<int>(1, 2) <=> x) == std::strong_ordering::less);

    assert((x < y)  == false);
    assert((x > y)  == false);
    assert((x <= y) == true);
    assert((x >= y) == true);
    assert((x == y) == true);
    assert((x != y) == false);

    assert(Rational<int>(1, 2) < Rational<int>(2, 3));
    assert(Rational<int>(3, 4) > Rational<int>(1, 2));
    assert(Rational<int>(2, 4) == Rational<int>(1, 2));
    assert(Rational<int>(1, 3) != Rational<int>(1, 2));

    std::stringstream stream_1("1/2");
    std::stringstream stream_2;

    stream_1 >> x;
    stream_2 << x;

    assert(stream_2.str() == stream_1.str());

    Rational<long long> a(1000000000LL, 2000000000LL);
    Rational<long long> b(3000000000LL, 4000000000LL);

    assert(a == Rational<long long>(1, 2));
    assert(b == Rational<long long>(3, 4));

    auto c = a + b;
    assert(c == Rational<long long>(5, 4));

    Rational<short> d(10, 20);
    Rational<short> e(15, 30);

    assert(d == e);
    assert(d == Rational<short>(1, 2));

    std::cout << "All tests passed successfully" << std::endl;
}