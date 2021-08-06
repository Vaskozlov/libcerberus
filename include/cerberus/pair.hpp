#ifndef CERBERUS_PAIR_HPP
#define CERBERUS_PAIR_HPP

#include <cerberus/types.h>

namespace cerb {
    template<typename T1, typename T2>
    struct CERBLIB_TRIVIAL pair {
        T1 first {};
        T2 second {};

    public:
        constexpr friend
        auto operator==(pair &lhs, pair &rhs) -> bool {
            return lhs.first == rhs.first && lhs.second == rhs.second;
        }

        constexpr friend
        auto operator!=(pair &lhs, pair &rhs) -> bool {
            return lhs.first != rhs.first || lhs.second != rhs.second;
        }

    public:
        constexpr auto operator=(pair<T1, T2> &&) noexcept -> pair<T1, T2>& = default;
        constexpr auto operator=(const pair<T1, T2> &) noexcept -> pair<T1, T2>& = default;

    public:
        constexpr pair() noexcept = default;
        constexpr ~pair() noexcept = default;

        constexpr pair(pair&) noexcept = default;
        constexpr pair(pair&&) noexcept = default;

        constexpr pair(const T1 &t_first, const T2 &t_second) noexcept
            : first(t_first), second(t_second)
        {}

        constexpr pair(const T1 &t_first) noexcept
            : first(t_first), second()
        {}

        constexpr pair(T1 &&t_first, T2 &&t_second) noexcept
            : first(t_first), second(t_second)
        {}
    };
} // namespace cerb

#endif /* CERBERUS_PAIR_HPP */
