#ifndef CERBERUS_PAIR_HPP
#define CERBERUS_PAIR_HPP

#include <cerberus/types.h>

namespace cerb {
    template<typename T1, typename T2>
    struct CERBLIB_TRIVIAL Pair {
        T1 first {};
        T2 second {};

    public:
        constexpr friend
        auto operator==(Pair &lhs, Pair &rhs) -> bool {
            return lhs.first == rhs.first && lhs.second == rhs.second;
        }

        constexpr friend
        auto operator!=(Pair &lhs, Pair &rhs) -> bool {
            return lhs.first != rhs.first || lhs.second != rhs.second;
        }

    public:
        constexpr auto operator=(Pair<T1, T2> &&) noexcept -> Pair<T1, T2>& = default;
        constexpr auto operator=(const Pair<T1, T2> &) noexcept -> Pair<T1, T2>& = default;

    public:
        constexpr Pair() noexcept = default;
        constexpr ~Pair() noexcept = default;

        constexpr Pair(Pair&) noexcept = default;
        constexpr Pair(Pair&&) noexcept = default;

        constexpr Pair(const T1 &t_first, const T2 &t_second) noexcept
            : first(t_first), second(t_second)
        {}

        constexpr explicit Pair(const T1 &t_first) noexcept
            : first(t_first), second()
        {}

        constexpr Pair(T1 &&t_first, T2 &&t_second) noexcept
            : first(t_first), second(t_second)
        {}
    };

    template<typename T1, typename T2> constexpr
    auto make_pair(const T1 &first, const T2 &second) -> cerb::Pair<T1, T2> {
        return cerb::Pair<T1, T2>(first, second);
    }
} // namespace cerb

#endif /* CERBERUS_PAIR_HPP */
