#include "solver.hpp"
#include "toroidal_list.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <memory>
#include <random>
#include <vector>
#include <utility>

static auto encode_sudoku(solve::sudoku const& s) noexcept -> solve::toroidal_list {

    auto list = solve::toroidal_list();

    for (unsigned y = 0; y < 9; ++y) {
        for (unsigned x = 0; x < 9; ++x) {
            auto value = s.data[x + 9 * y];

            if (value != solve::sudoku::empty_field) {
                list.cover_row(value - 1 + x * 9 + y * 9 * 9); 
            }
        }
    }

    return list;
};

static auto reencode(solve::sudoku const& constraints,
        std::vector<int> const& indices) -> solve::sudoku {

    auto result = constraints;

    for (auto index : indices) {
        auto const num = index % 9 + 1;
        auto const x = (index / 9) % 9;
        auto const y = index / (9 * 9);

        result.data[x + 9 * y] = num;
    }

    return result;
}

namespace solve {
    auto verify_sudoku(sudoku const& s) noexcept -> bool {
        auto const mask = std::uint16_t{0b00000001'11111111};
        auto accumulator = [] (std::uint16_t sum, std::int8_t elem) -> std::uint16_t {
            // just set a high bit to fail if we encounter a bad element
            if (elem <= 0 || elem > 9) {
                return sum | 0b1u << 10;
            }

            return sum | 0b1u << (elem - 1);
        };

        for (unsigned i = 0; i < 9; ++i) {
            auto row_range = s.iterate_from(sudoku::row_tag{}, i);
            auto column_range = s.iterate_from(sudoku::column_tag{}, i);
            auto block_range = s.iterate_from(sudoku::block_tag{}, i);

            auto row_mask = std::accumulate(row_range.begin, row_range.end,
                    std::uint16_t{0}, accumulator);
            auto column_mask = std::accumulate(column_range.begin, column_range.end,
                    std::uint16_t{0}, accumulator);
            auto block_mask = std::accumulate(block_range.begin, block_range.end,
                    std::uint16_t{0}, accumulator);

            if (row_mask != mask || column_mask != mask || block_mask != mask) {
                return false;
            }
        }

        return true;
    }

    auto solve_sudoku(sudoku const& s) noexcept -> sudoku {
        auto list = encode_sudoku(s);
        auto indices = list.solve();
        auto solution = reencode(s, indices);
        return solution;
    }
}
