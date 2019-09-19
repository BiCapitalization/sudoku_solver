#ifndef TOROIDAL_LIST_HPP
#define TOROIDAL_LIST_HPP

#include <array>
#include <functional>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

namespace solve {

    class toroidal_list {
        public:
        constexpr static inline auto columns = 9 * 9 * 4;
        constexpr static inline auto rows = 9 * 9 * 9; 
        // Each row contains exactly four nodes, one for each constraint.
        constexpr static inline auto total_nodes = 4 * rows;

        private:
        struct up_tag {};
        struct down_tag {};
        struct left_tag {};
        struct right_tag {};

        class column_head;

        class node {
            private:
            column_head* m_header;
            node* m_left;
            node* m_right;
            std::variant<node*, column_head*> m_up;
            std::variant<node*, column_head*> m_down;

            // Ugly, but I can't think of a better solution right now
            friend class toroidal_list;

            public:
            node() = default;

            explicit node(column_head* head, std::variant<node*, column_head*> up)
                noexcept;

            [[nodiscard]] auto left() const noexcept -> node*;
            [[nodiscard]] auto right() const noexcept -> node*;
            [[nodiscard]] auto header() const noexcept -> column_head*;
            [[nodiscard]] auto up() const noexcept -> std::variant<node*, column_head*>;
            [[nodiscard]] auto down() const noexcept -> std::variant<node*, column_head*>;

            void unlink_horizontally() noexcept;
            void relink_horizontally() noexcept;
            void unlink_vertically() noexcept;
            void relink_vertically() noexcept;

            template <typename Fun>
            void traverse(right_tag, Fun&& f) {
                for (auto* right = m_right; right != this; right = right->right()) {
                    std::invoke(std::forward<Fun>(f), *right);
                }
            }

            template <typename Fun>
            void traverse(left_tag, Fun&& f) {
                for (auto* left = m_left; left != this; left = left->left()) {
                    std::invoke(std::forward<Fun>(f), *left);
                }
            }

            template <typename Fun>
            void traverse(down_tag, Fun&& f) {
                for (auto down = m_down; !std::holds_alternative<column_head*>(down);
                        down = std::get<node*>(down)->down()) {

                    std::invoke(std::forward<Fun>(f), *std::get<node*>(down));
                }
            }

            template <typename Fun>
            void traverse(up_tag, Fun&& f) {
                for (auto up = m_up; !std::holds_alternative<column_head*>(up);
                        up = std::get<node*>(up)->m_up) {

                    std::invoke(std::forward<Fun>(f), *std::get<node*>(up));
                }
            }
        };

        class column_head {
            private:
            column_head* m_left;
            column_head* m_right;
            std::variant<node*, column_head*> m_up;
            std::variant<node*, column_head*> m_down;
            int m_count = 0;

            friend class toroidal_list;

            public:
            column_head() noexcept
                : m_left{}, m_right{}, m_up{}, m_down{} {}

            explicit column_head(column_head* left, column_head* right);

            void increase_count() noexcept;
            void decrease_count() noexcept;

            void unlink() noexcept;
            void relink() noexcept;

            void cover() noexcept;
            void uncover() noexcept;

            template <typename Fun>
            void traverse(up_tag, Fun&& f) {
                for (auto up = m_up; !std::holds_alternative<column_head*>(up);
                        up = std::get<node*>(up)->up()) {

                    std::invoke(std::forward<Fun>(f), *std::get<node*>(up));
                }
            }

            template <typename Fun>
            void traverse(down_tag, Fun&& f) {
                for (auto down = m_down; !std::holds_alternative<column_head*>(down);
                        down = std::get<node*>(down)->down()) {

                    std::invoke(std::forward<Fun>(f), *std::get<node*>(down));
                }
            }

            template <typename Fun>
            void traverse(right_tag, Fun&& f) {
                for (auto* right = m_right; right != this; right = right->m_right) { 

                    std::invoke(std::forward<Fun>(f), *right);
                }
            }

            template <typename Fun>
            auto traverse_until(down_tag, Fun&& f) -> bool {
                for (auto down = m_down; !std::holds_alternative<column_head*>(down);
                        down = std::get<node*>(down)->down()) {

                    if (std::invoke(std::forward<Fun>(f), *std::get<node*>(down))) {
                        return true;
                    }
                }

                return false;
            }
        };

        struct storage {
            // + 1 for the list header
            std::array<column_head, columns + 1> headers;
            std::array<node, total_nodes> nodes;
        };

        std::unique_ptr<storage> m_storage = std::make_unique<storage>();

        void make_columns() noexcept;
        void make_rows() noexcept;

        auto select_next_head() noexcept -> column_head&;

        auto solve_impl(std::vector<node*>& solutions, int index) noexcept -> bool;

        public:
        toroidal_list();

        toroidal_list(toroidal_list const&) = delete;
        toroidal_list(toroidal_list&&) = default;

        auto operator=(toroidal_list const&) -> toroidal_list& = delete;
        auto operator=(toroidal_list&&) -> toroidal_list& = default;

        ~toroidal_list() = default;

        void cover_row(int index) noexcept;

        auto solve() noexcept -> std::vector<int>;
    };
} /* namespace solve */
#endif // TOROIDAL_LIST_HPP
