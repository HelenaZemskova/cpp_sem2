// g++ -std=c++23 -O3 -o 07_05 07_05.cpp \
//     -I/usr/local/include \
//     -L/usr/local/lib \
//     -lbenchmark -lpthread && ./07_05

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <ranges>
#include <utility>
#include <vector>
#include <benchmark/benchmark.h>

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
    std::size_t mid = left + (right - left - 1) / 2;
    std::size_t last = right - 1;

    if (vector[left] > vector[mid])  std::swap(vector[left], vector[mid]);
    if (vector[left] > vector[last]) std::swap(vector[left], vector[last]);
    if (vector[mid] > vector[last]) std::swap(vector[mid], vector[last]);

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
void quick_sort(std::vector<T> & vector,
                std::size_t left, std::size_t right,
                std::size_t threshold)
{
    if (right - left > threshold)
    {
        std::size_t pivot_index = hoare(vector, left, right);
        quick_sort(vector, left, pivot_index, threshold);
        quick_sort(vector, pivot_index + 1, right, threshold);
    }
    else
    {
        order(vector, left, right);
    }
}

template <typename T>
void sort(std::vector<T> & vector, std::size_t threshold = 16)
{
    quick_sort(vector, 0, std::size(vector), threshold);
}


static constexpr auto data_size = 10'000uz;

void benchmark_sort(benchmark::State & state)
{
    auto const threshold = static_cast<std::size_t>(state.range(0));

    std::vector<double> data(data_size);

    for (auto _ : state)
    {
        state.PauseTiming();

        for (auto i = 0uz; i < data_size; ++i)
            data[i] = static_cast<double>(data_size - i);

        state.ResumeTiming();

        sort(data, threshold);

        benchmark::DoNotOptimize(data);
    }
}


BENCHMARK(benchmark_sort)->RangeMultiplier(2)->Range(1, 256);


int main()
{
    benchmark::RunSpecifiedBenchmarks();
}

