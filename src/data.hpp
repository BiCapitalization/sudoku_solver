#ifndef DATA_HPP
#define DATA_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace solve {

    template <typename T>
    struct iterator_range {
        T begin;
        T end;
    };

    template <typename T>
    iterator_range(T, T) -> iterator_range<T>;

    struct sudoku {
        constexpr static inline auto field_size = 81u;
        constexpr static inline auto empty_field = std::int8_t{0};

        std::array<std::int8_t, field_size> data = [] {
            std::array<std::int8_t, field_size> arr;
            std::fill(arr.begin(), arr.end(), empty_field);
            return arr;
        }();

        struct row_tag {};
        struct column_tag {};
        struct block_tag {};

        template <bool IsConst>
        class column_iterator_impl {
            public:
            using difference_type = std::ptrdiff_t;
            using value_type = std::int8_t;
            using pointer = std::conditional_t<IsConst, std::int8_t const*, std::int8_t*>;
            using reference = std::conditional_t<IsConst, std::int8_t const&,
                  std::int8_t&>;
            using iterator_category = std::forward_iterator_tag;

            private:
            // having an additional end pointer is necessary because we only get a single
            // position past the end of our array, not a full row 
            pointer m_ptr = nullptr;  
            pointer m_end = nullptr;

            friend struct sudoku;

            explicit column_iterator_impl(pointer ptr, pointer end) : m_ptr{ptr}, m_end{end} {} 

            [[nodiscard]] friend auto operator<(column_iterator_impl a,
                    column_iterator_impl b) noexcept -> bool {

                return a.m_ptr < b.m_ptr;
            }

            [[nodiscard]] friend auto operator>(column_iterator_impl a,
                    column_iterator_impl b) noexcept -> bool {

                return a.m_ptr > b.m_ptr;
            }

            [[nodiscard]] friend auto operator<=(column_iterator_impl a,
                    column_iterator_impl b) noexcept -> bool {

                return a.m_ptr <= b.m_ptr;
            }

            [[nodiscard]] friend auto operator>=(column_iterator_impl a,
                    column_iterator_impl b) noexcept -> bool {

                return a.m_ptr >= b.m_ptr;
            }

            [[nodiscard]] friend auto operator==(column_iterator_impl a,
                    column_iterator_impl b) noexcept -> bool {

                return a.m_ptr == b.m_ptr;
            }

            [[nodiscard]] friend auto operator!=(column_iterator_impl a,
                    column_iterator_impl b) noexcept -> bool {

                return a.m_ptr != b.m_ptr;
            }

            public:
            column_iterator_impl() = default;

            auto operator++() noexcept -> column_iterator_impl& {
                if (m_end - m_ptr < 9) {
                    m_ptr = m_end;
                } else {
                    m_ptr += 9;
                }
                return *this;
            }

            auto operator++(int) noexcept -> column_iterator_impl {
                auto copy = *this;
                ++*this;
                return copy;
            }

            template <bool Proxy = IsConst, typename = std::enable_if_t<!Proxy>>
            [[nodiscard]] auto operator*() noexcept -> reference {
                assert(m_ptr != m_end && "Tried to dereference invalid iterator.");
                return *m_ptr;
            }

            [[nodiscard]] auto operator*() const noexcept -> std::int8_t {
                assert(m_ptr != m_end && "Tried to dereference invalid iterator.");
                return *m_ptr;
            }
        };

        template <bool IsConst>
        class block_iterator_impl {
            public:
            using difference_type = std::ptrdiff_t;
            using value_type = std::int8_t;
            using pointer = std::conditional_t<IsConst, std::int8_t const*, std::int8_t*>;
            using reference = std::conditional_t<IsConst, std::int8_t const&,
                  std::int8_t&>;
            using iterator_category = std::forward_iterator_tag;

            private:
            // since this method of iteration is the most complex, we actually need two pointers
            // and a counter
            pointer m_ptr = nullptr;  
            pointer m_start = nullptr;
            unsigned m_increments = 0;

            friend struct sudoku;

            explicit block_iterator_impl(pointer ptr)
                : m_ptr{ptr}, m_start{ptr} {} 

            [[nodiscard]] friend auto operator<(block_iterator_impl<IsConst> a,
                    block_iterator_impl<IsConst> b) noexcept -> bool {

                return a.m_ptr < b.m_ptr;
            }

            [[nodiscard]] friend auto operator>(block_iterator_impl<IsConst> a,
                    block_iterator_impl<IsConst> b) noexcept -> bool {

                return a.m_ptr > b.m_ptr;
            }

            [[nodiscard]] friend auto operator<=(block_iterator_impl<IsConst> a,
                    block_iterator_impl<IsConst> b) noexcept -> bool {

                return a.m_ptr <= b.m_ptr;
            }

            [[nodiscard]] friend auto operator>=(block_iterator_impl<IsConst> a,
                    block_iterator_impl<IsConst> b) noexcept -> bool {

                return a.m_ptr >= b.m_ptr;
            }

            [[nodiscard]] friend auto operator==(block_iterator_impl<IsConst> a,
                    block_iterator_impl<IsConst> b) noexcept -> bool {

                return a.m_ptr == b.m_ptr;
            }

            [[nodiscard]] friend auto operator!=(block_iterator_impl<IsConst> a,
                    block_iterator_impl<IsConst> b) noexcept -> bool {

                return a.m_ptr != b.m_ptr;
            }

            public:
            block_iterator_impl() = default;

            auto operator++() noexcept -> block_iterator_impl& {
                auto horizontal_offset = (m_increments + 1) % 3;
                auto vertical_offset = (m_increments + 1) / 3;

                // We need to make sure not to overstep by more than one element, since
                // that's ub. Thus, we keep a counter of how much we've iterated and just
                // increase by one at the last step.
                if (m_increments == 8) {
                    m_ptr += 1;
                } else {
                    m_ptr = m_start + horizontal_offset + vertical_offset * 9;
                    m_increments += 1;
                }

                return *this;
            }

            auto operator++(int) noexcept -> block_iterator_impl {
                auto copy = *this;
                ++*this;
                return copy;
            }         

            template <bool Proxy = IsConst, typename = std::enable_if_t<!Proxy>>
            [[nodiscard]] auto operator*() noexcept -> reference {
                return *m_ptr;
            }

            [[nodiscard]] auto operator*() const noexcept -> std::int8_t {
                return *m_ptr;
            }
        };

        using row_iterator = decltype(data.begin());
        using const_row_iterator = decltype(data.cbegin());
        using column_iterator = column_iterator_impl<false>;
        using const_column_iterator = column_iterator_impl<true>;
        using block_iterator = block_iterator_impl<false>;
        using const_block_iterator = block_iterator_impl<true>;

        [[nodiscard]] auto iterate_from(row_tag, unsigned index) noexcept
            -> iterator_range<row_iterator>;
        [[nodiscard]] auto iterate_from(column_tag, unsigned index) noexcept
            -> iterator_range<column_iterator>;
        [[nodiscard]] auto iterate_from(block_tag, unsigned index) noexcept
            -> iterator_range<block_iterator>;

        [[nodiscard]] auto iterate_from(row_tag, unsigned index) const noexcept
            -> iterator_range<const_row_iterator>;
        [[nodiscard]] auto iterate_from(column_tag, unsigned index) const noexcept
            -> iterator_range<const_column_iterator>;
        [[nodiscard]] auto iterate_from(block_tag, unsigned index) const noexcept
            -> iterator_range<const_block_iterator>;
    }; 
}
#endif // DATA_HPP
