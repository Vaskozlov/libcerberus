#ifndef CERBERUS_RADIXSORT_HPP
#define CERBERUS_RADIXSORT_HPP

#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include <cerberus/string.hpp>
#include <cerberus/bit.hpp>

namespace cerb {
    template<typename T>
    auto RadixSort(T *data, size_t size) -> void {
        if (size <= 1)
            return;

        static_assert(
            sizeof(T) == sizeof(u8) ||
            sizeof(T) == sizeof(u16) ||
            sizeof(T) == sizeof(u32) ||
            sizeof(T) == sizeof(u64));

        static_assert(std::is_trivially_copyable<T>::value);

        unsigned count[256];
        auto converted_data = reinterpret_cast<cerb::ByteMask<T> *>(data);
        auto output         = reinterpret_cast<cerb::ByteMask<T> *>(operator new(size * sizeof(cerb::ByteMask<T>)));

        for (unsigned shift = 0u, s = 0u; shift < sizeof(T); shift++, s += 8) {
            cerb::memset(count, 0u, sizeof(count) / sizeof(count[0]));

            for (unsigned i = 0; i < size; i++) {
                count[(converted_data[i].getBits() >> s) & 0xffu]++;
            }

            for (unsigned i = 1; i < sizeof(count) / sizeof(count[0]); i++) {
                count[i] += count[i - 1];
            }

            for (auto i = static_cast<long int>(size - 1); i >= 0; i--) {
                auto idx             = (converted_data[i].getBits() >> s) & 0xffu;
                output[--count[idx]] = converted_data[i];
            }

            std::swap(converted_data, output);
        }

        if (reinterpret_cast<cerb::ByteMask<T> *>(data) == output) {
            std::swap(converted_data, output);

            cerb::memcpy(converted_data, output, size);
        }

        operator delete(output);
    }

}// namespace cerb

#endif /* CERBERUS_RADIXSORT_HPP */
