#ifndef CERBERUS_ALGORITHMS_HPP
#define CERBERUS_ALGORITHMS_HPP

#include <cerberus/types.h>

namespace cerb {
    template<typename Iterator, typename T>
    constexpr auto find(Iterator first, Iterator last, const T &value) -> Iterator {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (*first == value) {
                return first;
            }
        }
        return first;
    }

    template<typename Iterator, class UnaryPredicate>
    constexpr auto find_if(Iterator first, Iterator last, UnaryPredicate p) -> Iterator {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (p(*first)) {
                return first;
            }
        }
        return first;
    }

    template<typename Iterator, class UnaryPredicate>
    constexpr auto find_if_not(Iterator first, Iterator last, UnaryPredicate p) -> Iterator {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (!p(*first)) {
                return first;
            }
        }
        return first;
    }

    template<typename Iterator, typename T>
    constexpr auto count(Iterator first, Iterator last, const T &value) -> size_t {
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
    constexpr auto count_if(Iterator first, Iterator last, UnaryPredicate p) -> size_t {
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
    constexpr inline auto all_of(Iterator first, Iterator last, UnaryPredicate p) -> bool {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (!p(*first)) {
                return false;
            }
        }
        return true;
    }

    template<typename Iterator, class UnaryPredicate>
    constexpr inline auto any_of(Iterator first, Iterator last, UnaryPredicate p) -> bool {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (p(*first)) {
                return true;
            }
        }
        return false;
    }

    template<typename Iterator, class UnaryPredicate>
    constexpr inline auto none_of(Iterator first, Iterator last, UnaryPredicate p) -> bool {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (p(*first)) {
                return false;
            }
        }
        return true;
    }

    template<typename Iterator1, typename Iterator2>
    constexpr inline auto mismatch(Iterator1 first1, Iterator1 last1, Iterator2 first2) -> Pair<Iterator1, Iterator2> {
        CERBLIB_UNROLL_N(2)
        for (; first1 != last1; ++first1, ++first2) {
            if (*first1 != *first2) {
                return { first1, first2 };
            }
        }
        return { first1, first2 };
    }

    template<typename Iterator1, typename Iterator2>
    constexpr inline auto mismatch(Iterator1 first1, Iterator1 last1, Iterator2 first2, Iterator2 last2) -> Pair<Iterator1, Iterator2> {
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
