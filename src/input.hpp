#ifndef INPUT_HPP
#define INPUT_HPP

#include "data.hpp"

#include <fmt/core.h>
#include <tl/expected.hpp>

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace solve {
    class io_error {
        public:
        enum class err_code {
            NO_SUCH_FILE,
            FORMAT_ERROR,
            UNKNOWN_ERROR
        };

        static auto err_code_to_string(err_code code) noexcept -> std::string_view;

        private:
        err_code m_code;
        std::optional<std::string> m_details;

        public:
        explicit io_error(err_code code);
        explicit io_error(err_code code, std::string details);
        [[nodiscard]] auto details() const noexcept -> std::optional<std::string> const&; 
        [[nodiscard]] auto code() const noexcept -> err_code;
    };

    [[nodiscard]] auto read_from_file(std::filesystem::path const& path) 
        -> tl::expected<std::vector<sudoku>, io_error>;
} /* namespace solve */

// Custom formatter for use with fmt
namespace fmt {
    template <>
    struct formatter<solve::io_error> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext& context) {
            return context.begin();
        }

        template <typename FormatContext>
        constexpr auto format(solve::io_error const& err, FormatContext& context) {
            if (err.details().has_value()) {
                return format_to(context.out(), "IO Error: {}: {}",
                        solve::io_error::err_code_to_string(err.code()),
                        err.details().value());
            }

            return format_to(context.out(), "IO Error: {}",
                    solve::io_error::err_code_to_string(err.code()));
        }
    }; 
} /* namespace fmt */
#endif // INPUT_HPP
