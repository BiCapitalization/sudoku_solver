#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstdlib>
#include <variant>
#include <utility>

#if defined(__GNUC__) || defined(__clang__)
#define unreachable() __builtin_unreachable()
#else
#define unreachable() std::abort()
#endif


namespace solve::util {
    template <typename... Fns>
    struct overload_set : public Fns... {
       using Fns::operator()...;
    }; 

    template <typename... Fns>
    overload_set(Fns...) -> overload_set<Fns...>;

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
} /* namespace solve */
#endif // UTILITY_HPP
