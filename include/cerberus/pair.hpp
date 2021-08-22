#ifndef CERBERUS_PAIR_HPP
#define CERBERUS_PAIR_HPP

#include <cerberus/types.h>
#include <cerberus/literals.hpp>

namespace cerb {
    using namespace cerb::literals;

    enum PairCompare : u16
    {
        DEFAULT,
        BY_FIRST_VALUE,
        BY_SECOND_VALUE
    };

    template<typename T1, typename T2, PairCompare Compare = DEFAULT>
    struct CERBLIB_TRIVIAL Pair
    {
        T1 first;
        T2 second;


    public:
        constexpr auto operator==(const Pair<T1, T2, DEFAULT> &other) const -> bool
        {
            return first == other.first && second == other.second;
        }

        constexpr auto operator==(const Pair<T1, T2, BY_FIRST_VALUE> &other) const
            -> bool
        {
            return first == other.first;
        }

        constexpr auto operator==(const Pair<T1, T2, BY_SECOND_VALUE> &other) const
            -> bool
        {
            return second == other.second;
        }

        template<typename U>
        constexpr friend auto operator==(const Pair &lhs, const U &rhs) -> bool
        {
            static_assert(Compare == BY_FIRST_VALUE || Compare == BY_SECOND_VALUE);
            if constexpr (Compare == BY_FIRST_VALUE) {
                return lhs.first == rhs;
            } else if constexpr (Compare == BY_SECOND_VALUE) {
                return lhs.second == rhs;
            }
        }

        constexpr auto operator<=>(const Pair<T1, T2, BY_FIRST_VALUE> &other) const
        {
            return first <=> other.first;
        }

        constexpr auto operator<=>(const Pair<T1, T2, BY_SECOND_VALUE> &other) const
        {
            return second <=> other.second;
        }

        constexpr auto operator<=>(const Pair<T1, T2, DEFAULT> &other) const
        {
            if (first == other.first) {
                return second <=> other.second;
            }

            return first <=> other.first;
        }

        template<typename U>
        constexpr auto operator<=>(const U &other) const
        {
            static_assert(Compare == BY_FIRST_VALUE || Compare == BY_SECOND_VALUE);

            if constexpr (Compare == BY_FIRST_VALUE) {
                return first <=> other;
            } else if constexpr (Compare == BY_SECOND_VALUE) {
                return second <=> other;
            }
        }

    public:
        constexpr auto operator=(Pair &&) noexcept -> Pair & = default;
        constexpr auto operator=(const Pair &) noexcept -> Pair & = default;

    public:
        constexpr Pair() noexcept             = default;
        constexpr ~Pair() noexcept            = default;
        constexpr Pair(Pair &&) noexcept      = default;
        constexpr Pair(const Pair &) noexcept = default;

        constexpr explicit Pair(T1 &&t_first) noexcept : first(t_first), second()
        {}

        constexpr explicit Pair(const T1 &t_first) noexcept
          : first(t_first), second()
        {}

        constexpr Pair(T1 &&t_first, T2 &&t_second) noexcept
          : first(t_first), second(t_second)
        {}

        constexpr Pair(const T1 &t_first, const T2 &t_second) noexcept
          : first(t_first), second(t_second)
        {}
    };

    template<typename T1, typename T2, PairCompare Compare = DEFAULT>
    constexpr auto make_pair(const T1 &first, const T2 &second)
        -> Pair<T1, T2, Compare>
    {
        return Pair<T1, T2, Compare>(first, second);
    }
}// namespace cerb

#endif /* CERBERUS_PAIR_HPP */
