#ifndef CERBERUS_STRING_HPP
#define CERBERUS_STRING_HPP

#include <cerberus/types.h>
#include <cerberus/bit.hpp>
#include <cerberus/type_traits.hpp>
#include <cerberus/private/string.hpp>

namespace cerb {
    size_t strlen(const char * __restrict str);

    CERBLIB_DISABLE_WARNING(constant-evaluated,constant-evaluated,0)

    template<typename T> constexpr
    auto memset(void *__restrict ptr, T value, size_t times) -> void {
        if (
            !std::is_constant_evaluated() &&
            cerb::x86_64 &&
            std::is_integral_v<T>
        ) {
            return cerb::PRIVATE::memset<T>(ptr, value, times);
        } else {
            T *copy_of_ptr;
            auto *address = copy_of_ptr = static_cast<T *>(ptr);

            CERBLIB_UNROLL_N(4)
            while (address < copy_of_ptr + times) {
                *(address++) = value;
            }
        }
    }

    template<typename T, size_t Size, typename V> constexpr
    auto memset(std::array<T, Size> &t_array, V value, size_t times) {
        if (!std::is_constant_evaluated()) {
            memset(t_array.data(), value, times);
        } else {
            CERBLIB_UNROLL_N(4)
            for (size_t i = 0; i < times; i++) {
                t_array[i] = value;
            }
        }
    }

    template<typename T> constexpr
    auto memcpy(T *__restrict dest, const T *__restrict src, size_t times) -> void {
        if (
            !std::is_constant_evaluated() &&
            cerb::x86_64 &&
            std::is_integral_v<T>
        ) {
            cerb::PRIVATE::memcpy(dest, src, times);
        } else {
            T *dest_copy = dest;
            const T *converted_src = src;
            T *converted_dest = dest;

            while (converted_dest < dest_copy + times) {
                *converted_dest = *converted_src;
                converted_src++;
                converted_dest++;
            }
        }
    }

    template<typename T, size_t Size> constexpr
    auto memcpy(std::array<T, Size> &dest, const std::array<T, Size> &src, size_t times) {
        if (!std::is_constant_evaluated()) {
            memcpy(dest.data(), src.data(), times);
        } else {
            CERBLIB_UNROLL_N(4)
            for (size_t i = 0; i < Size; ++i) {
                dest[i] = src[i];
            }
        }
    }

    CERBLIB_ENABLE_WARNING(constant-evaluated,constant-evaluated,0)

    template<typename T>
    [[nodiscard]] consteval
    auto str2uint(const char *str) -> T {
        static_assert(
            std::is_integral_v<T> &&
            std::is_unsigned_v<T>
        );

        T result = 0;
        for (size_t i = 0; i < sizeof(T); i++) {
            result |= static_cast<T>(cerb::bit_cast<unsigned char>(str[i])) << (i * 8);
        }

        return result;
    }
}

#endif /* CERBERUS_STRING_HPP */
