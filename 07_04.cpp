// g++ -std=c++23 -Wall -Wextra -o 07_04 07_04.cpp -lgtest -lgmock -lpthread && ./07_04


#include <algorithm>
#include <cstddef>
#include <numeric>
#include <ranges>
#include <string>
#include <utility>
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

template <typename T>
void order(std::vector<T> & vector, std::size_t left, std::size_t right)
{
    for (auto i = left + 1; i < right; ++i)
    {
        for (auto j = i; j > left; --j)
        {
            if (vector[j - 1] > vector[j])
                std::swap(vector[j], vector[j - 1]);
        }
    }
}


template <typename T>
T medianOfThree(std::vector<T> & vector, std::size_t left, std::size_t right)
{
    std::size_t mid  = left + (right - left - 1) / 2;
    std::size_t last = right - 1;

    if (vector[left] > vector[mid])  std::swap(vector[left], vector[mid]);
    if (vector[left] > vector[last]) std::swap(vector[left], vector[last]);
    if (vector[mid]  > vector[last]) std::swap(vector[mid],  vector[last]);

    std::swap(vector[mid], vector[last]);

    return vector[last];
}


template <typename T>
std::size_t hoare(std::vector<T> & vector, std::size_t left, std::size_t right)
{
    T pivot = medianOfThree(vector, left, right);
    std::size_t last = right - 1;
    std::size_t i = left;
    std::size_t j = last - 1;

    while (true)
    {
        while (vector[i] < pivot) ++i;
        while (j > left && vector[j] > pivot) --j;

        if (i >= j)
        {
            std::swap(vector[i], vector[last]);
            return i;
        }

        std::swap(vector[i], vector[j]);
    }
}

template <typename T>
void quick_sort(std::vector<T> & vector, std::size_t left, std::size_t right)
{
    if (right - left > 16)
    {
        std::size_t pivot_index = hoare(vector, left, right);
        quick_sort(vector, left, pivot_index);
        quick_sort(vector, pivot_index + 1, right);
    }
    else
    {
        order(vector, left, right);
    }
}

template <typename T>
void sort(std::vector<T> & vector)
{
    quick_sort(vector, 0, std::size(vector));
}

// Простые тесты

TEST(Sort, EmptyVector)
{
    std::vector<int> v;
    sort(v);
    ASSERT_TRUE(std::ranges::is_sorted(v));
}

TEST(Sort, SingleElement)
{
    std::vector<int> v = { 42 };
    sort(v);
    ASSERT_EQ(v[0], 42);
    ASSERT_TRUE(std::ranges::is_sorted(v));
}


TEST(Sort, AlreadySorted)
{
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    sort(v);
    ASSERT_TRUE(std::ranges::is_sorted(v));
    ASSERT_THAT(v, testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(Sort, ReverseSorted)
{
    std::vector<int> v = { 5, 4, 3, 2, 1 };
    sort(v);
    ASSERT_TRUE(std::ranges::is_sorted(v));
    ASSERT_THAT(v, testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(Sort, WithDuplicates)
{
    std::vector<int> v = { 3, 1, 4, 1, 5, 9, 2, 6, 5, 3 };
    sort(v);
    ASSERT_TRUE(std::ranges::is_sorted(v));
    ASSERT_THAT(v, testing::ElementsAre(1, 1, 2, 3, 3, 4, 5, 5, 6, 9));
}


TEST(Sort, AllEqual)
{
    std::vector<int> v(10, 7);
    sort(v);
    ASSERT_TRUE(std::ranges::is_sorted(v));
    ASSERT_THAT(v, testing::Each(testing::Eq(7)));
}

TEST(Sort, Doubles)
{
    std::vector<double> v = { 5.5, 2.2, 8.8, 1.1, 9.9, 3.3 };
    sort(v);
    ASSERT_TRUE(std::ranges::is_sorted(v));
    ASSERT_THAT(v, testing::ElementsAre(1.1, 2.2, 3.3, 5.5, 8.8, 9.9));
}

TEST(Sort, Chars)
{
    std::vector<char> v = { 'z', 'a', 'm', 'b', 'y', 'c' };
    sort(v);
    ASSERT_TRUE(std::ranges::is_sorted(v));
    ASSERT_THAT(v, testing::ElementsAre('a', 'b', 'c', 'm', 'y', 'z'));
}

TEST(Sort, Strings)
{
    std::vector<std::string> v = { "zebra", "apple", "mango", "banana" };
    sort(v);
    ASSERT_TRUE(std::ranges::is_sorted(v));
    ASSERT_THAT(v, testing::ElementsAre("apple", "banana", "mango", "zebra"));
}

// Тесты с фикстурой

class Fixture : public testing::Test
{
public:
    std::vector<int> vector;
};

TEST_F(Fixture, LargeReversed)
{
    auto size = 1'000uz;

    vector.resize(size);

    for (auto i = 0uz; i < size; ++i)
        vector[i] = static_cast<int>(size - i);

    sort(vector);

    ASSERT_TRUE(std::ranges::is_sorted(vector));
    ASSERT_EQ(vector.front(), 1);
    ASSERT_EQ(vector.back(),  static_cast<int>(size));
}

TEST_F(Fixture, Iota)
{
    vector.resize(100);
    std::iota(vector.begin(), vector.end(), 0);

    sort(vector);

    ASSERT_TRUE(std::ranges::is_sorted(vector));
    ASSERT_EQ(vector.front(), 0);
    ASSERT_EQ(vector.back(),  99);
}

// Параметризованные
// Тестируем сортировку для разных размеров вектора

class Adapter : public Fixture, public testing::WithParamInterface<int> {};

TEST_P(Adapter, ReversedOfSize)
{
    auto n = static_cast<std::size_t>(GetParam());

    vector.resize(n);

    for (auto i = 0uz; i < n; ++i)
        vector[i] = static_cast<int>(n - i);

    sort(vector);

    ASSERT_TRUE(std::ranges::is_sorted(vector));
    ASSERT_EQ(std::size(vector), n);
}

INSTANTIATE_TEST_CASE_P(Fixture, Adapter, testing::Values(0, 1, 2, 15, 16, 17, 100, 500, 1000));


int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

