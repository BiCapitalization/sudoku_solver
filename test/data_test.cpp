#include "data.hpp"

#include <catch2/catch.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <numeric>

using namespace solve;

TEST_CASE("Iterator tests") {
    auto test_grid = sudoku{};

    for (unsigned i = 0; i < 9; ++i) {
        auto [begin, end] = test_grid.iterate_from(sudoku::row_tag{}, i);
        std::iota(begin, end, 1);
    }

    for (unsigned i = 0; i < 9; ++i) {
        auto row_range = test_grid.iterate_from(sudoku::row_tag{}, i);
        auto column_range = test_grid.iterate_from(sudoku::column_tag{}, i);
        auto block_range = test_grid.iterate_from(sudoku::block_tag{}, i);

        REQUIRE_THAT(row_range, Catch::Predicate<decltype(row_range)>(
            [] (auto range) {
                auto [begin, end] = range; 
                auto reference = std::array<std::int8_t, 9>{};
                std::iota(reference.begin(), reference.end(), 1);

                return std::equal(begin, end, reference.begin(), reference.end());
            }));

        REQUIRE_THAT(column_range, Catch::Predicate<decltype(column_range)>(
            [i] (auto range) {
                auto [begin, end] = range; 
                auto reference = std::array<std::int8_t, 9>{};
                std::fill(reference.begin(), reference.end(), i + 1);

                return std::equal(begin, end, reference.begin(), reference.end());
            }));

        REQUIRE_THAT(block_range, Catch::Predicate<decltype(block_range)>(
            [i] (auto range) {
                auto [begin, end] = range; 
                auto reference = std::array<std::int8_t, 9>{1, 2, 3, 1, 2, 3, 1, 2, 3};
                std::transform(reference.begin(), reference.end(), reference.begin(),
                    [i] (auto val) {
                        return val + 3 * (i % 3);
                    });

                return std::equal(begin, end, reference.begin(), reference.end());
            }));
    }
}
