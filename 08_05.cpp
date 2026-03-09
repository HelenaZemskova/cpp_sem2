#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <print>
#include <stdexcept>
#include <string>
#include <vector>

template < typename D = std::chrono::duration < double > > class Timer
{
public :

    explicit Timer(std::string const & scope) : m_scope(scope) {}


    void start()
    {
        if (m_running)
            throw std::logic_error("Timer::start : timer is already running");

        m_running = true;
        m_begin   = clock_t::now();
    }


    void stop()
    {
        if (!m_running)
            throw std::logic_error("Timer::stop : timer is not running");

        m_samples.push_back(std::chrono::duration_cast < D > (clock_t::now() - m_begin));

        m_running = false;
    }


    auto average() const -> D
    {
        if (m_samples.empty())
            throw std::logic_error("Timer::average : no measurements recorded");

        D total{};

        for (auto const & sample : m_samples) total += sample;

        return total / static_cast < double > (m_samples.size());
    }


    auto size() const -> std::size_t { return m_samples.size(); }

   ~Timer()
    {
        if (!m_samples.empty())
        {
            std::print("{} : average = {:.6f}s over {} measurement(s)\n",
                       m_scope, average().count(), m_samples.size());
        }
    }


private :

    using clock_t = std::chrono::steady_clock;


    std::string m_scope;

    bool m_running = false;

    clock_t::time_point m_begin;

    std::vector < D > m_samples;
};


auto calculate(std::size_t size)
{
    auto x = 0.0;

    for (auto i = 0uz; i < size; ++i)
    {
        x += std::pow(std::sin(i), 2) + std::pow(std::cos(i), 2);
    }

    return x;
}

auto equal(double x, double y, double epsilon = 1e-6)
{
    return std::abs(x - y) < epsilon;
}

int main()
{
    constexpr auto iterations = 5uz;
    constexpr auto workload   = 1'000'000uz;

    Timer timer("main : timer");

    for (auto i = 0uz; i < iterations; ++i)
    {
        timer.start();

        assert(equal(calculate(workload), static_cast < double > (workload)));

        timer.stop();

        std::print("main : iteration {} : {:.6f}s\n", i, timer.average().count());
    }

    std::print("main : measurements = {}\n", timer.size());
    std::print("main : average = {:.6f}s\n", timer.average().count());
}
