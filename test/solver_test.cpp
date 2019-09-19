#include "solver.hpp"

#include <catch2/catch.hpp>

using namespace solve;

TEST_CASE("Solver tests") {
    auto empty = sudoku{};
    auto solve_result = solve_sudoku(empty);

    REQUIRE(verify_sudoku(solve_result));
}
