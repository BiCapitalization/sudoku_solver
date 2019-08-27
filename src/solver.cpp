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
#include <variant>

#include <cstdio>

static auto encode_sudoku(solve::sudoku const& s) noexcept -> solve::toroidal_list {

    solve::toroidal_list list; 
    list.add_columns(solve::toroidal_list::max_columns);

    // TODO: Comment and check that this is correct
    auto encoder = [] (auto i, auto j) -> bool {

        auto const num = j % 9;
        auto const x = (j / 9) % 9;
        auto const y = (j / (9 * 9)) % 9;

        auto const constraint_index = i / (9 * 9);
        auto const constraint_x = (i % (9 * 9)) % 9;
        auto const constraint_y = (i % (9 * 9)) / 9;

        switch (constraint_index) {
            case 0:
                return x == constraint_x && y == constraint_y;
            case 1:
                // Here, constraint_x represents the number and constraint_y represents the
                // row that number is in.
                return num == constraint_x && y == constraint_y;
            case 2:
                // Here, constraint_x represents the number (again) and constraint_y
                // represents the column that number is in.
                return num == constraint_x && x == constraint_y; 
            case 3:
                // constraint_x represents the number and constraint_y represents the block
                // that number is in. Furthermore, we obtain the block that we're currently
                // in by doing some indexing adjustments.
                return num == constraint_x && (y / 3) * 3 + x / 3 == constraint_y;
            default:
                assert(false && "detected out of range constraint");
                return false;
        }
    };
    
    list.build_matrix(solve::toroidal_list::max_rows, encoder);

    for (unsigned y = 0; y < 9; ++y) {
        for (unsigned x = 0; x < 9; ++x) {
            auto value = s.data[x + 9 * y];

            if (value != solve::sudoku::empty_field) {
                list.cover_row(value + x * 9 + y * 9 * 9 - 1); 
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
            assert(elem > 0 && "encountered invalid value during verification");
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
