#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "data.hpp"

#include <random>

namespace solve {
    [[nodiscard]] auto verify_sudoku(sudoku const& s) noexcept -> bool;
    [[nodiscard]] auto solve_sudoku(sudoku const& s) noexcept -> sudoku; 
} /* namespace solve */

#endif // SOLVER_HPP
