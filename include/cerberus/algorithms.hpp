#ifndef CERBERUS_ALGORITHMS_HPP
#define CERBERUS_ALGORITHMS_HPP

#include <cerberus/types.h>
#include <cerberus/string.hpp>

namespace cerb {
    template<typename T>
    constexpr auto destroy(T &_value) -> void
    {
        (&_value)->~T();
    }

    template<typename T>
    auto move(T &&value) noexcept -> typename std::remove_reference<T>::type &&
    {
        return static_cast<typename std::remove_reference<T>::type &&>(value);
    }

    template<typename T>
    constexpr auto raw_move(T *dest, T *begin, T *end) -> void
    {
        if (std::is_constant_evaluated()) {
            if constexpr (std::is_trivial_v<T>) {
                return memcpy(dest, begin, static_cast<long>(end - begin));
            }
        }

        CERBLIB_UNROLL_N(2)
        for (; begin != end; ++begin, ++dest) {
            std::construct_at(dest, cerb::move(*begin));
        }
    }

    template<typename T>
    constexpr auto raw_copy(T *dest, const T *begin, const T *end) -> void
    {
        if (!std::is_constant_evaluated()) {
            if constexpr (std::is_trivial_v<T>) {
                return memcpy(dest, begin, static_cast<ptrdiff_t>(end - begin));
            }
        }

        CERBLIB_UNROLL_N(2)
        for (; begin != end; ++begin, ++dest) {
            std::construct_at(dest, static_cast<const T&>(*begin));
        }
    }

    template<typename T>
    auto swap(T &&lhs, T &&rhs) noexcept(std::is_nothrow_move_constructible_v<T>
                                             &&std::is_nothrow_move_assignable_v<T>)
        -> typename std::enable_if<
            std::is_move_constructible_v<T> && std::is_move_assignable_v<T>,
            bool>::type
    {
        auto tmp = move(lhs);
        lhs      = move(rhs);
        rhs      = move(tmp);
    }

    template<typename Iterator, typename T>
    constexpr auto find(Iterator first, Iterator last, const T &value) -> Iterator
    {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (*first == value) {
                return first;
            }
        }
        return first;
    }

    template<typename Iterator, typename UnaryPredicate>
    constexpr auto find_if(Iterator first, Iterator last, UnaryPredicate p)
        -> Iterator
    {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (p(*first)) {
                return first;
            }
        }
        return first;
    }

    template<typename Iterator, class UnaryPredicate>
    constexpr auto find_if_not(Iterator first, Iterator last, UnaryPredicate p)
        -> Iterator
    {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (!p(*first)) {
                return first;
            }
        }
        return first;
    }

    template<typename Iterator, typename T>
    constexpr auto count(Iterator first, Iterator last, const T &value) -> size_t
    {
        size_t result = 0;

        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (*first == value) {
                ++result;
            }
        }
        return result;
    }

    template<typename Iterator, class UnaryPredicate>
    constexpr auto count_if(Iterator first, Iterator last, UnaryPredicate p)
        -> size_t
    {
        size_t result = 0;

        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (p(*first)) {
                ++result;
            }
        }
        return result;
    }

    template<typename Iterator, class UnaryPredicate>
    constexpr inline auto all_of(Iterator first, Iterator last, UnaryPredicate p)
        -> bool
    {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (!p(*first)) {
                return false;
            }
        }
        return true;
    }

    template<typename Iterator, class UnaryPredicate>
    constexpr inline auto any_of(Iterator first, Iterator last, UnaryPredicate p)
        -> bool
    {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (p(*first)) {
                return true;
            }
        }
        return false;
    }

    template<typename Iterator, class UnaryPredicate>
    constexpr inline auto none_of(Iterator first, Iterator last, UnaryPredicate p)
        -> bool
    {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (p(*first)) {
                return false;
            }
        }
        return true;
    }

    template<typename Iterator1, typename Iterator2>
    constexpr inline auto
        mismatch(Iterator1 first1, Iterator1 last1, Iterator2 first2)
            -> Pair<Iterator1, Iterator2>
    {
        CERBLIB_UNROLL_N(2)
        for (; first1 != last1; ++first1, ++first2) {
            if (*first1 != *first2) {
                return { first1, first2 };
            }
        }
        return { first1, first2 };
    }

    template<typename Iterator1, typename Iterator2>
    constexpr inline auto mismatch(
        Iterator1 first1,
        Iterator1 last1,
        Iterator2 first2,
        Iterator2 last2) -> Pair<Iterator1, Iterator2>
    {
        CERBLIB_UNROLL_N(2)
        for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if (*first1 != *first2) {
                return { first1, first2 };
            }
        }
        return { first1, first2 };
    }
}// namespace cerb

#endif /* CERBERUS_ALGORITHMS_HPP */
