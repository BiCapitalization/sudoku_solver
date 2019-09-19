#include "toroidal_list.hpp"

#include "utility.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>

namespace {
    struct sudoku_coordinates {
        int num = 1;
        int x = 0;
        int y = 0;
    };

    // Rows in the sudoku represent triples of the form (number, x, y) which
    // are laid out sequentially, i.e. an index of 0 corresponds to (1, 0, 0),
    // and index of 1 corresponds to (2, 0, 0), an index of 9 corresponds to
    // (1, 1, 0), etc.
    [[nodiscard]] auto row_number_to_coordinates(int row_num) noexcept 
        -> sudoku_coordinates {
        
        assert(row_num >= 0 && row_num < solve::toroidal_list::rows 
                && "Trying to access invalid row.");

        auto const num = row_num % 9;
        auto const x = (row_num / 9) % 9;
        auto const y = row_num / (9 * 9);

        return sudoku_coordinates{num, x, y};
    }

    // Given a constraint quadrant and a row number, which column should the
    // corresponding node go into?
    [[nodiscard]] auto calculate_column_index(int quadrant, int row_num) noexcept
        -> int {

        assert(quadrant >= 0 && quadrant < 4 && "Invalid quadrant.");

        auto const [num, x, y] = row_number_to_coordinates(row_num);

        switch (quadrant) {
            case 0:
                // In the first quadrant, each column represents a constraint
                // of the form (x, y) signifying that the cell at that position
                // has a number in it. 
                return x + 9 * y;
            case 1:
                // In the second quadrant, each column represents a constraint
                // of the form (number, row) signifying that a row contains a
                // certain number.
                return 9 * 9 + num + 9 * y;
            case 2:
                // In the third quadrant, each column represents a constraint
                // of the form (number, column) signifying that a column
                // contains a certain number.
                return 2 * 9 * 9 + num + 9 * x;
            case 3:
                // In the fourth quadrant, each column represents a constraint
                // of the form (number, block) signifying that a block contains
                // a certain number.
                return 3 * 9 * 9 + num + 9 * (x / 3 + (y / 3) * 3);
            default:
                unreachable();
                return -1;
        }
    }
} /* namespace */

namespace solve {

    toroidal_list::node::node(column_head* head,
            std::variant<toroidal_list::node*, toroidal_list::column_head*> up) noexcept
        : m_header{head}, m_left{this}, m_right{this}, m_up(up), m_down(head) {}

    auto toroidal_list::node::left() const noexcept -> toroidal_list::node* {
        return m_left;
    }

    auto toroidal_list::node::right() const noexcept -> toroidal_list::node* {
        return m_right;
    }

    auto toroidal_list::node::header() const noexcept -> toroidal_list::column_head* {
        return m_header;
    }

    auto toroidal_list::node::up() const noexcept
        -> std::variant<toroidal_list::node*, toroidal_list::column_head*> {

        return m_up;
    }

    auto toroidal_list::node::down() const noexcept
        -> std::variant<toroidal_list::node*, toroidal_list::column_head*> {

        return m_down;
    }

    void toroidal_list::node::unlink_horizontally() noexcept {
        // Order is important: First right, then left...
        m_left->m_right = m_right;
        m_right->m_left = m_left;
    }

    void toroidal_list::node::relink_horizontally() noexcept {
        // ...to undo: First left, then right.
        m_right->m_left = this;
        m_left->m_right = this;
    }

    void toroidal_list::node::unlink_vertically() noexcept {
        // Order is important: First up, then down...
        std::visit([this] (auto* up) {
                up->m_down = m_down;
            }, m_up);

        std::visit([this] (auto* down) {
                down->m_up = m_up;
            }, m_down);

        m_header->decrease_count(); 
    }

    void toroidal_list::node::relink_vertically() noexcept {
        //...to undo: First down, then up.
        std::visit([this] (auto* down) {
                down->m_up = this;
            }, m_down);

        std::visit([this] (auto* up) {
                up->m_down = this;
            }, m_up);

        m_header->increase_count();
    }

    toroidal_list::column_head::column_head(toroidal_list::column_head* left,
            toroidal_list::column_head* right)
        : m_left{left}, m_right{right}, m_up{this}, m_down{this} {}

    void toroidal_list::column_head::increase_count() noexcept {
        m_count += 1;
    }

    void toroidal_list::column_head::decrease_count() noexcept {
        assert(m_count > 0 && "Trying to decrement count beyond zero");
        m_count -= 1;
    }

    void toroidal_list::column_head::unlink() noexcept {
        m_right->m_left = m_left;
        m_left->m_right = m_right;
    }

    void toroidal_list::column_head::relink() noexcept {
        m_left->m_right = this;
        m_right->m_left = this;
    }

    void toroidal_list::column_head::cover() noexcept {
        unlink();

        traverse(down_tag{}, [] (auto& down) {
            down.traverse(right_tag{}, [] (auto& right) {
                right.unlink_vertically();
            });
        });
    }

    void toroidal_list::column_head::uncover() noexcept {

        traverse(up_tag{}, [] (auto& up) {
            up.traverse(left_tag{}, [] (auto& left) {
                left.relink_vertically();
            });    
        });

        relink();
    }
    
    toroidal_list::toroidal_list() {
        make_columns(); 
        make_rows();
    }

    void toroidal_list::make_columns() noexcept {
        auto& headers = m_storage->headers; 

        for (std::size_t i = 1; i < headers.size(); ++i) {
            headers[i - 1].m_right = &headers[i];
            headers[i].m_left = &headers[i - 1];
        }

        headers[0].m_left = &headers.back();
        headers.back().m_right = &headers[0];
    }

    void toroidal_list::make_rows() noexcept {
        auto& nodes = m_storage->nodes;
        auto& headers = m_storage->headers;

        auto last_vertical_nodes = std::array<node*, columns>{};

        auto vertically_link = [&last_vertical_nodes, &headers]
            (node& n, int column_index) {

            // + 1 for the head node.
            auto& current_header = headers[column_index + 1];

            n.m_header = &current_header;
            current_header.increase_count();

            if (last_vertical_nodes[column_index] == nullptr) {
                n.m_up = &current_header;
                current_header.m_down = &n;
            } else {
                n.m_up = last_vertical_nodes[column_index];
                last_vertical_nodes[column_index]->m_down = &n;
            }

            last_vertical_nodes[column_index] = &n;
        };

        for (std::size_t row_num = 0; row_num < nodes.size() / 4; ++row_num) {

            auto const row_index = row_num * 4;

            auto const zero_index = calculate_column_index(0, row_num);
            vertically_link(nodes[row_index], zero_index);

            for (int i = 1; i < 4; ++i) {
                auto const index = calculate_column_index(i, row_num);

                auto& current_node = nodes[row_index + i];
                auto& last_node = nodes[row_index + i - 1];

                current_node.m_left = &last_node;
                last_node.m_right = &current_node;
                
                vertically_link(current_node, index);
            }

            // Finally, make the horizontal wrap-around links.
            nodes[row_index + 3].m_right = &nodes[row_index];
            nodes[row_index].m_left = &nodes[row_index + 3];
        }

        // Make the vertical wrap-around links.
        for (auto* n : last_vertical_nodes) {
            n->m_header->m_up = n;
            n->m_down = n->m_header;
        }
    }

    void toroidal_list::cover_row(int index) noexcept {
        assert(index >= 0 && index < rows && "Row index out of range.");

        auto& node = m_storage->nodes[index * 4];

        node.traverse(right_tag{}, [] (auto& right) {
            right.m_header->cover();
        });

        node.m_header->cover();
    }

    auto toroidal_list::select_next_head() noexcept -> toroidal_list::column_head& {
        auto min_count = std::numeric_limits<int>::max();
        column_head* min_head = &m_storage->headers[0];

        min_head->traverse(right_tag{}, [&min_count, &min_head] (auto& right) mutable {

            // Sadly, we need this because std::tie takes lvalue references so
            // simply passing &right does not work.
            auto* right_ptr = &right;
            std::tie(min_count, min_head) = std::min(std::tie(min_count, min_head),
                    std::tie(right.m_count, right_ptr), 
                    [] (auto a, auto b) {
                        return std::get<0>(a) < std::get<0>(b);
            });
        });

        return *min_head;
    }

    auto toroidal_list::solve_impl(std::vector<node*>& solutions, int index) noexcept
        -> bool {

        auto& root = m_storage->headers[0];

        if (&root == root.m_right) {
            return true;
        }

        column_head* next_column = &select_next_head();
        next_column->cover();

        auto found_solution = next_column->traverse_until(down_tag{}, [&] (auto& down) {
            auto* down_ptr = &down;

            solutions[index] = down_ptr;

            down.traverse(right_tag{}, [] (auto& right) {
                right.m_header->cover();
            });

            if (solve_impl(solutions, index + 1)) {
                return true;
            }

            down_ptr = solutions[index];
            next_column = down_ptr->m_header;

            down_ptr->traverse(left_tag{}, [] (auto& left) {
                left.m_header->uncover();
            });

            return false;
        }); 

        if (found_solution) {
            return true;
        }

        next_column->uncover();
        return false;
    }

    auto toroidal_list::solve() noexcept -> std::vector<int> {
        auto result = std::vector<node*>(9 * 9);
        solve_impl(result, 0);

        auto range_end = std::find(result.begin(), result.end(), nullptr);

        auto indices = std::vector<int>();
        indices.reserve(result.size());

        std::transform(result.begin(), range_end, std::back_inserter(indices), 
            [&nodes = m_storage->nodes] (auto* node) {
                return (node - nodes.data()) / 4;
            });

        return indices;
    }
} /* namespace solve */
