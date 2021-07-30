#ifndef CERBERUS_ALGORITHMS_HPP
#define CERBERUS_ALGORITHMS_HPP

#include <cerberus/types.h>
#include <cerberus/private/quickSort.hpp>
#include <cerberus/private/radixSort.hpp>

namespace cerb {
    template<typename Iterator, typename T>
    constexpr auto find(Iterator first, Iterator last, const T& value) -> Iterator
    {
        for (; first != last; ++first) {
            if (*first == value) {
                return first;
            }
        }
        return last;
    }

    template<typename Iterator, class UnaryPredicate>
    constexpr Iterator find_if(Iterator first, Iterator last, UnaryPredicate p)
    {
        for (; first != last; ++first) {
            if (p(*first)) {
                return first;
            }
        }
        return last;
    }
}

#endif /* CERBERUS_ALGORITHMS_HPP */
