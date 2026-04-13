// g++ -std=c++23 -Wall -Wextra -o 10_03 10_03.cpp -I/usr/local/include && ./10_03


#include <boost/multi_array.hpp>

#include <cassert>
#include <cstddef>
#include <iostream>

static constexpr auto N = 10uz;

using Grid = boost::multi_array < bool, 2 >;

auto make_grid() -> Grid
{
    Grid grid(boost::extents[N][N]);

    for (auto r = 0uz; r < N; ++r)
    {
        for (auto c = 0uz; c < N; ++c)
        {
            grid[r][c] = false;
        }
    }

    return grid;
}

auto count_neighbors(Grid const & grid, std::size_t r, std::size_t c) -> int
{
    auto count = 0;

    for (auto dr = -1; dr <= 1; ++dr)
    {
        for (auto dc = -1; dc <= 1; ++dc)
        {
            if (dr == 0 && dc == 0) continue;

            auto nr = static_cast < int > (r) + dr;
            auto nc = static_cast < int > (c) + dc;

            if (nr >= 0 && nr < static_cast < int > (N) &&
                nc >= 0 && nc < static_cast < int > (N))
            {
                count += grid[static_cast < std::size_t > (nr)]
                              [static_cast < std::size_t > (nc)] ? 1 : 0;
            }
        }
    }

    return count;
}

auto step(Grid const & current) -> Grid
{
    auto next = make_grid();

    for (auto r = 0uz; r < N; ++r)
    {
        for (auto c = 0uz; c < N; ++c)
        {
            auto n = count_neighbors(current, r, c);

            if (current[r][c])
            {
                next[r][c] = (n == 2 || n == 3);
            }
            else
            {
                next[r][c] = (n == 3);
            }
        }
    }

    return next;
}

void print(Grid const & grid, int generation)
{
    std::cout << "Generation " << generation << ":\n";
    std::cout << '+' << std::string(N, '-') << "+\n";

    for (auto r = 0uz; r < N; ++r)
    {
        std::cout << '|';

        for (auto c = 0uz; c < N; ++c)
        {
            std::cout << (grid[r][c] ? '#' : '.');
        }

        std::cout << "|\n";
    }

    std::cout << '+' << std::string(N, '-') << "+\n\n";
}

int main()
{
    auto grid = make_grid();

    grid[1][2] = true;
    grid[2][3] = true;
    grid[3][1] = true;
    grid[3][2] = true;
    grid[3][3] = true;

    {
        int count = 0;
        for (auto r = 0uz; r < N; ++r)
            for (auto c = 0uz; c < N; ++c)
                if (grid[r][c]) ++count;
        assert(count == 5);
    }

    auto gen0 = grid;

    for (auto i = 0; i < 4; ++i) grid = step(grid);

    {
        bool same = true;
        for (auto r = 0uz; r < N; ++r)
            for (auto c = 0uz; c < N; ++c)
                if (grid[r][c] != gen0[r][c]) { same = false; break; }
        assert(!same);
    }

    {
        int count = 0;
        for (auto r = 0uz; r < N; ++r)
            for (auto c = 0uz; c < N; ++c)
                if (grid[r][c]) ++count;
        assert(count == 5);
    }

    assert(grid[2][3]);
    assert(grid[3][4]);
    assert(grid[4][2]);
    assert(grid[4][3]);
    assert(grid[4][4]);

    grid = gen0;

    static constexpr auto generations = 20;

    for (auto i = 0; i <= generations; ++i)
    {
        print(grid, i);

        grid = step(grid);
    }
    std::cout << "All tests passed successfully.\n";
}
