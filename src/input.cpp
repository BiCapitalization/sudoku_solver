#include "input.hpp"

#include <fmt/core.h>

#include <cctype>
#include <fstream>
#include <utility>

using std::literals::string_view_literals::operator""sv;

static constexpr auto is_valid_char(char c) -> bool {
    if (('1' <= c && c <= '9') || c == '.') {
        return true;
    }

    return false;
}

static constexpr auto to_board_value(char c) -> std::int8_t {
    if (c == '.') {
        return std::int8_t{0};
    }

    return static_cast<std::int8_t>(c - '0');
}

static auto parse_single_line(std::string_view line)
    -> tl::expected<solve::sudoku, solve::io_error> {

    if (line.size() != solve::sudoku::field_size) {
        return tl::unexpected(solve::io_error(solve::io_error::err_code::FORMAT_ERROR,
                    "Given input line is not 81 characters long."));
    }

    solve::sudoku s;
    for (std::size_t i = 0; i < solve::sudoku::field_size; ++i) {
        auto c = line[i];
        if (!is_valid_char(c)) {
            return tl::unexpected(solve::io_error(solve::io_error(
                        solve::io_error::err_code::FORMAT_ERROR,
                        fmt::format("Invalid character '{:c}' in input.", c))));
        }

        s.data[i] = to_board_value(c);
    } 

    return s;
}

namespace solve {
    auto io_error::err_code_to_string(solve::io_error::err_code code) noexcept
        -> std::string_view {

        switch (code) {
            case err_code::NO_SUCH_FILE:
                return "No such file"sv;
            case err_code::FORMAT_ERROR:
                return "Format error"sv;
            case err_code::UNKNOWN_ERROR:
                return "Unknown error"sv;
            default:
                return ""sv;
        }
    } 

    io_error::io_error(err_code code)
        : m_code(code), m_details() {}
    io_error::io_error(err_code code, std::string details) 
        : m_code(code), m_details(std::move(details)) {}

    auto io_error::details() const noexcept -> std::optional<std::string> const& {
        return m_details;
    }

    auto io_error::code() const noexcept -> io_error::err_code {
        return m_code;
    }

    auto read_from_file(std::filesystem::path const& path)
        -> tl::expected<std::vector<sudoku>, io_error> {

        auto file = std::ifstream(path); 
        if (!file.is_open()) {
            return tl::unexpected(io_error(io_error::err_code::NO_SUCH_FILE));
        }

        std::vector<sudoku> results;

        std::string line; 
        while (std::getline(file, line)) {
            auto res = parse_single_line(line);

            // Oh, how I wish for Rust's `?`...
            if (!res.has_value()) {
                return tl::unexpected(res.error()); 
            }

            results.push_back(std::move(res).value());
        }

        return results;
    }
}
