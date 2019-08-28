# sudoku_solver

Modern and moderately fast sudoku solver

## Building and Configuring the Project
This project uses CMake. Just change to a build directory of your choice, invoke `cmake <src_dir>` and call `make`/`ninja`/your build system of choice. This will set up all dependencies and then build the project.

Tests are not built by default. If you want to build the tests, pass `-DBuildTests=On` on the CMake command line.

Building this project produces a binary `sudoku_solve` in `<build_dir>/bin` and a static library `libssolve.a` in `<build_dir>/lib` as well as a binary `test` in `<build_dir>/tests` if building tests is enabled.

## Usage
Invoke the binary with a single file as argument, which should contain one or more lines of sudokus. Each sudoku has to be encoded as a single line of text consisting of exactly 81 characters. Each character takes a value in the range `['1', '9']` or is `.` to represent an empty field, respectively.

For each such line, the program outputs a 81 characters long string of digits representing the solved sudoku.

## Notes
The code quality of this project is currently abysmal due to being hacked together without much of a plan in a comparatively short amount of time. Please don't judge me too harshly :). Refactors are coming.

The program cannot currently identify any unsolvable sudokus, and is otherwise not too stable with regard to malformed input. If you feed it bad input, expect crashes to happen.

## Acknowledgments and Dependencies
This project uses [fmt](https://github.com/fmtlib/fmt) and [tl::expected](https://github.com/TartanLlama/expected), as well as [Catch2](https://github.com/catchorg/Catch2) for tests.
