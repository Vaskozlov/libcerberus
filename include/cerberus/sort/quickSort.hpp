#ifndef CERBERUS_QUICKSORT_HPP
#define CERBERUS_QUICKSORT_HPP

#include <cerberus/types.h>
#include <cerberus/algorithms.hpp>

namespace cerb {
    namespace PRIVATE {

        template<typename T>
        CERBLIB_INLINE auto Partition(T *data, size_t lo, size_t hi) -> size_t {
            auto j     = static_cast<intmax_t>(hi + 1);
            auto i     = static_cast<intmax_t>(lo) - 1ll;
            auto pivot = data[lo + (hi - lo) / 2_z];

            while (true) {
                for (; data[++i] < pivot;)
                    ;
                for (; data[--j] > pivot;)
                    ;

                if (i >= j) {
                    return j;
                }

                swap(data[i], data[j]);
            }
        }

        template<typename T>
        auto QuickSort(T *data, size_t lo, size_t hi) -> void {
            if (lo < hi) [[likely]] {
                auto p = Partition(data, lo, hi);
                QuickSort(data, lo, p);
                QuickSort(data, p + 1, hi);
            }
        }
    }// namespace PRIVATE

    template<typename T>
    CERBLIB_INLINE auto QuickSort(T *data, size_t count) {
        if (count <= 1) [[unlikely]]
            return;
        PRIVATE::QuickSort(data, 0_z, count - 1);
    }
}// namespace cerb

#endif /* CERBERUS_QUICKSORT_HPP */
