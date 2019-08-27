#include "data.hpp"

#include <cassert>

namespace solve {
    auto sudoku::iterate_from(sudoku::row_tag, unsigned index) noexcept
        -> iterator_range<sudoku::row_iterator> {
        
        assert(index < 9 && "Row index out of range");
        return iterator_range{data.begin() + index * 9,
            data.begin() + (index + 1) * 9};
    }

    auto sudoku::iterate_from(sudoku::column_tag, unsigned index) noexcept
        -> iterator_range<sudoku::column_iterator> {

        assert(index < 9 && "Column index out of range");

        auto end_ptr = data.data() + data.size();
        return iterator_range{column_iterator(&data[index], end_ptr),
            column_iterator(end_ptr, end_ptr)};
    }
    
    auto sudoku::iterate_from(sudoku::block_tag, unsigned index) noexcept
        -> iterator_range<sudoku::block_iterator> {

        assert(index < 9 && "Block index out of range");
        auto horizontal_offset = (index % 3) * 3;
        auto vertical_offset = (index / 3) * 3;
        auto block_start = data.data() + vertical_offset * 9 + horizontal_offset;
        auto block_end = block_start + 2 * 9 + 3;

        return iterator_range{block_iterator(block_start), block_iterator(block_end)};
    }

    auto sudoku::iterate_from(sudoku::row_tag, unsigned index) const noexcept
        -> iterator_range<sudoku::const_row_iterator> {
        
        assert(index < 9 && "Row index out of range");
        return iterator_range{data.begin() + index * 9,
            data.begin() + (index + 1) * 9};
    }

    auto sudoku::iterate_from(sudoku::column_tag, unsigned index) const noexcept
        -> iterator_range<sudoku::const_column_iterator> {

        assert(index < 9 && "Column index out of range");

        auto end_ptr = data.data() + data.size();
        return iterator_range{const_column_iterator(&data[index], end_ptr),
            const_column_iterator(end_ptr, end_ptr)};
    }
    
    auto sudoku::iterate_from(sudoku::block_tag, unsigned index) const noexcept
        -> iterator_range<sudoku::const_block_iterator> {

        assert(index < 9 && "Block index out of range");
        auto horizontal_offset = (index % 3) * 3;
        auto vertical_offset = (index / 3) * 3;
        auto block_start = data.data() + vertical_offset * 9 + horizontal_offset;
        auto block_end = block_start + 2 * 9 + 3;

        return iterator_range{const_block_iterator(block_start),
            const_block_iterator(block_end)};
    }
} /* namespace solve */
