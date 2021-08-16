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
        return last;
    }

    template<typename Iterator, class UnaryPredicate>
    constexpr Iterator find_if(Iterator first, Iterator last, UnaryPredicate p) {
        CERBLIB_UNROLL_N(2)
        for (; first != last; ++first) {
            if (p(*first)) {
                return first;
            }
        }
        return last;
    }
}// namespace cerb

#endif /* CERBERUS_ALGORITHMS_HPP */
