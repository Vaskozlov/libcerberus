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
        static constexpr auto is_nothrow_destructible =
            std::is_nothrow_destructible_v<T1> && std::is_nothrow_destructible_v<T2>;

        static constexpr auto is_nothrow_copy_assignable =
            std::is_nothrow_copy_assignable_v<T1> &&
            std::is_nothrow_copy_assignable_v<T2>;

        static constexpr auto is_nothrow_move_assignable =
            std::is_nothrow_move_assignable_v<T1> &&
            std::is_nothrow_move_assignable_v<T2>;

        static constexpr auto is_nothrow_copy_constructible =
            std::is_nothrow_copy_constructible_v<T1> &&
            std::is_nothrow_copy_constructible_v<T2>;

        static constexpr auto is_nothrow_move_constructible =
            std::is_nothrow_move_constructible_v<T1> &&
            std::is_nothrow_move_constructible_v<T2>;

        static constexpr auto is_nothrow_default_constructible =
            std::is_nothrow_default_constructible_v<T1> &&
            std::is_nothrow_default_constructible_v<T2>;

    public:
        constexpr auto operator==(const Pairable auto &other) const -> bool
        {
            if constexpr (Compare == DEFAULT) {
                return first == other.first && second == other.second;
            } else if constexpr (Compare == BY_FIRST_VALUE) {
                return first == other.first;
            } else {
                return second == other.second;
            }
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

        constexpr auto operator<=>(const Pairable auto &other) const
        {
            if constexpr (Compare == DEFAULT) {
                if (first == other.first) {
                    return second <=> other.second;
                }
                return first <=> other.first;
            } else if constexpr (Compare == BY_FIRST_VALUE) {
                return first <=> other.first;
            } else {
                return second <=> other.second;
            }
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

        constexpr auto operator=(const Pairable auto &other) noexcept -> Pair &
        {
            first  = other.first;
            second = other.second;
            return *this;
        }

        constexpr auto operator=(Pairable auto &&other) noexcept -> Pair &
        {
            first  = std::move(other.first);
            second = std::move(other.second);
            return *this;
        }

    public:
        constexpr Pair() noexcept(is_nothrow_default_constructible) = default;
        constexpr ~Pair() noexcept(is_nothrow_destructible)         = default;

        constexpr Pair(Pair &&) noexcept(is_nothrow_move_assignable)      = default;
        constexpr Pair(const Pair &) noexcept(is_nothrow_copy_assignable) = default;

        constexpr explicit Pair(const Pairable auto &other) noexcept(
            is_nothrow_copy_assignable)
          : first(other.first), second(other.second)
        {}

        constexpr explicit Pair(Pairable auto &&other) noexcept(
            is_nothrow_move_assignable)
          : first(std::move(other.first)), second(std::move(other.second))
        {}

        constexpr explicit Pair(T1 &&t_first) noexcept(is_nothrow_move_constructible)
          : first(t_first), second()
        {}

        constexpr explicit Pair(const T1 &t_first) noexcept(
            is_nothrow_copy_constructible)
          : first(t_first), second()
        {}

        constexpr Pair(T1 &&t_first, T2 &&t_second) noexcept(
            is_nothrow_move_constructible)
          : first(t_first), second(t_second)
        {}

        constexpr Pair(const T1 &t_first, const T2 &t_second) noexcept(
            is_nothrow_copy_constructible)
          : first(t_first), second(t_second)
        {}
    };

    template<PairCompare Compare = DEFAULT, typename T1, typename T2>
    CERBLIB_DECL auto make_pair(const T1 &first, const T2 &second)
        -> Pair<T1, T2, Compare>
    {
        return Pair<T1, T2, Compare>(first, second);
    }
}// namespace cerb

#endif /* CERBERUS_PAIR_HPP */
