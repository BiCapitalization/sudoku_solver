#ifndef TOROIDAL_LIST_HPP
#define TOROIDAL_LIST_HPP

#include <functional>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include <cassert>

namespace solve {
    class column_head;

    class node {
        public:
        column_head* m_header;
        node* m_left;
        node* m_right;
        std::variant<node*, column_head*> m_up;
        std::variant<node*, column_head*> m_down;

        int m_row_index;

        node() = default;

        explicit node(column_head* head, std::variant<node*, column_head*> up,
                int row_index);
        private:
        friend class toroidal_list;
    };

    class column_head {
        public:
        column_head* m_left;
        column_head* m_right;
        std::variant<node*, column_head*> m_up;
        std::variant<node*, column_head*> m_down;
        int m_count = 0;


        column_head();
        explicit column_head(column_head* left, column_head* right);

        private:
        friend class toroidal_list;

        public:
        void cover();
        void uncover();
    };

    class toroidal_list {
        public:
        constexpr static inline auto max_columns = 9 * 9 * 4;
        constexpr static inline auto max_rows = 9 * 9 * 9; 

        private:
        std::vector<column_head> m_header_storage;
        std::vector<node> m_node_storage;
        std::vector<node*> m_row_references;
        column_head* m_root = nullptr; 

        public:
        toroidal_list();

        toroidal_list(toroidal_list const&) = delete;
        toroidal_list(toroidal_list&&) = default;

        auto operator=(toroidal_list const&) -> toroidal_list& = delete;
        auto operator=(toroidal_list&&) -> toroidal_list& = default;

        ~toroidal_list() = default;

        auto add_columns(int num) -> column_head*;
        auto add_column() -> column_head*;

        void cover_row(int index);

        auto solve() -> std::vector<int>;

        template <typename Fun>
        void build_matrix(int row_count, Fun&& fun) {
            if (m_header_storage.empty() || row_count <= 0) {
                return;
            }

            // - 1 for the root node
            auto const columns = m_header_storage.size() - 1;

            for (auto row_index = 0; row_index < row_count; ++row_index) {

                node* first_row_node = nullptr;
                node* last_row_node = nullptr;

                for (auto column_index = 0;
                        static_cast<std::size_t>(column_index) < columns;
                        ++column_index) {

                    auto res = std::invoke(std::forward<Fun>(fun),
                            column_index, row_index);

                    if (res == 0) {
                        continue;
                    }

                    auto* current_column = m_header_storage.data() + column_index + 1;
                    auto previous_node = current_column->m_up;

                    m_node_storage.emplace_back(current_column, previous_node, row_index);
                    auto* current_node = &m_node_storage.back();

                    current_column->m_up = current_node;
                    current_column->m_count += 1;

                    std::visit([=] (auto* ptr) {
                            ptr->m_down = current_node;
                        }, previous_node);

                    if (first_row_node == nullptr) {
                        first_row_node = current_node;
                    } else {
                        current_node->m_right = first_row_node;
                        first_row_node->m_left = current_node;
                    }

                    if (last_row_node != nullptr) {
                        last_row_node->m_right = current_node;
                        current_node->m_left = last_row_node;
                    }

                    last_row_node = current_node;
                }
                
                // It is unimportant at which node in a row we point as long as we do
                // point at one.
                m_row_references.push_back(last_row_node);
            }
        }
    };
} /* namespace solve */
#endif // TOROIDAL_LIST_HPP
