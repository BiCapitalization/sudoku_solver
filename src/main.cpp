#include "input.hpp"
#include "solver.hpp"

#include <cstdio>
#include <string>

static auto to_string(solve::sudoku const& s) -> std::string {
    auto result = std::string();
    result.reserve(81);

    for (auto val : s.data) {
        result.push_back(val + '0');
    }

    return result;
}

auto main(int argc, char const** argv) -> int {
    if (argc != 2) {
        fmt::print(stderr, "Incorrect number of arguments: {:d} arguments given, "
                "but one expected. Invoke the program with a single data file "
                "path as argument.", argc - 1);
        return 1;
    } 

    auto result = solve::read_from_file(std::filesystem::path(argv[1])); 

    if (!result.has_value()) {
        fmt::print(stderr, "An error occured:\n{}", std::move(result).error());
        return 1;
    }

    auto data = std::move(result).value();
    for (auto const& s : data) {
        auto res = solve::solve_sudoku(s);
        fmt::print("{}\n", to_string(res));
    }
}
