#include "toroidal_list.hpp"

#include "utility.hpp"

#include <cassert>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include <cstdio>

namespace {
    template <typename Variant0, typename Variant1>
    [[nodiscard]] auto variant_equal(Variant0&& a, Variant1&& b) -> bool {
        return std::visit(
            [] (auto&& l, auto&& r) {
                if constexpr (std::is_same_v<decltype(l), decltype(r)>) {
                    return l == r;
                }

                return false;
            }, std::forward<Variant0>(a), std::forward<Variant1>(b));
    }

    [[nodiscard]] auto choose_next_column(solve::column_head* head)
        -> solve::column_head* {

       int min_count = std::numeric_limits<int>::max();
       solve::column_head* min_col = head;
       for (auto* ptr = head->m_right; ptr != head; ptr = ptr->m_right) {
           std::tie(min_count, min_col) = std::min(std::tie(min_count, min_col),
               std::tie(ptr->m_count, ptr), [] (auto a, auto b) {
                    return std::get<0>(a) < std::get<0>(b);
               });
       }

       return min_col;
    }

    bool solve_impl(solve::column_head* head,
            std::vector<solve::node*>& solutions, int index) {

        if (head == head->m_right) {
            return true;
        }

        solve::column_head* next_column = choose_next_column(head);
        next_column->cover();

        auto count = 0;
        for (auto down = next_column->m_down;
                !std::holds_alternative<solve::column_head*>(down);
                down = std::visit([] (auto ptr) {
                        return ptr->m_down;
                    }, down)) {

            auto* down_node = std::get<solve::node*>(down);

            solutions[index] = down_node;

            for (auto right = down_node->m_right; right != down_node;
                    right = right->m_right)  {
                right->m_header->cover();
            }

            if (solve_impl(head, solutions, index + 1)) {
                return true;
            }

            down_node = solutions[index];
            next_column = down_node->m_header;

            for (auto left = down_node->m_left; left != down_node;
                    left = left->m_left) {
                left->m_header->uncover();
            }
            ++count;
        }

        next_column->uncover();
        return false;
    }
}

namespace solve {

    node::node(column_head* head, std::variant<node*, column_head*> up, int row_index) 
        : m_header{head}, m_left{this}, m_right{this}, m_up(up), m_down(head),
            m_row_index{row_index} {}

    column_head::column_head() 
        : m_left{this}, m_right{this}, m_up{}, m_down{} {}

    column_head::column_head(column_head* left, column_head* right)
        : m_left{left}, m_right{right}, m_up{this}, m_down{this} {}

    void column_head::cover() {
        m_right->m_left = m_left;
        m_left->m_right = m_right;

        for (auto n = m_down;
                !std::holds_alternative<column_head*>(n);
                n = std::visit([] (auto ptr) {
                        return ptr->m_down;
                    }, n)) {

            auto* n_ptr = std::get<node*>(n);
            
            for (auto* current_node = n_ptr->m_right;
                    current_node != n_ptr;
                    current_node = current_node->m_right) {


                std::visit([current_node] (auto* down) {
                        down->m_up = current_node->m_up;
                    }, current_node->m_down);

                std::visit([current_node] (auto* up) {
                        up->m_down = current_node->m_down;
                    }, current_node->m_up);

                current_node->m_header->m_count -= 1;
            }
        }
    }

    void column_head::uncover() {
        for (auto n = m_up;
                !std::holds_alternative<column_head*>(n);
                n = std::visit([] (auto ptr) {
                        return ptr->m_up;
                    }, n)) {

            auto* n_ptr = std::get<node*>(n);
            
            for (auto* current_node = n_ptr->m_left;
                    current_node != n_ptr;
                    current_node = current_node->m_left) {

                std::visit([current_node] (auto* up) {
                        up->m_down = current_node;
                    }, current_node->m_up);

                std::visit([current_node] (auto* down) {
                        down->m_up = current_node;
                    }, current_node->m_down);

                current_node->m_header->m_count += 1;
            }
        }

        m_left->m_right = this;
        m_right->m_left = this;
    }
    
    toroidal_list::toroidal_list() {
        // + 1 for the root header
        m_header_storage.reserve(max_columns + 1);
        m_node_storage.reserve(max_rows * max_columns);
        m_row_references.reserve(max_rows);
    }

    auto toroidal_list::add_columns(int num) -> column_head* {

        assert(num >= 0 && "Cannot add a negative number of columns");

        if (num == 0) {
            return nullptr;
        }

        if (m_root == nullptr) {
            m_header_storage.emplace_back(); 

            m_root = m_header_storage.data();
        }

        auto* last_column = &m_header_storage.back();
        for (auto i = 0; i < num; ++i) {
            m_header_storage.emplace_back(last_column, m_root);
            auto* new_column = &m_header_storage.back();

            last_column->m_right = new_column; 
            new_column->m_left = last_column;

            last_column = new_column;
        }

        return last_column;
    }

    auto toroidal_list::add_column() -> column_head* {
        return add_columns(1);
    }

    void toroidal_list::cover_row(int index) {
        if (index < 0 || static_cast<std::size_t>(index) >= m_row_references.size()) {
            return;
        }

        auto* node = m_row_references[index];
        assert(node->m_row_index == index);

        for (auto* current_node = node->m_right; current_node != node;
                current_node = current_node->m_right) {

            auto* head = current_node->m_header;
            head->cover(); 
        }

        node->m_header->cover();
    }

    auto toroidal_list::solve() -> std::vector<int> {
        auto result = std::vector<node*>(max_rows);
        solve_impl(m_root, result, 0);

        auto indices = std::vector<int>();
        indices.reserve(result.size());

        for (auto* ptr : result) {

            if (ptr == nullptr) {
                break;
            }

            indices.push_back(ptr->m_row_index);
        }

        return indices;
    }
} /* namespace solve */
