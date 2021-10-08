#ifndef CERBERUS_STRING_HPP
#define CERBERUS_STRING_HPP

#include <cerberus/bit.hpp>
#include <cerberus/type_traits.hpp>
#include <cerberus/types.h>

#if defined(__x86_64__)
#    include <cerberus/private/string.hpp>
#endif

namespace cerb {
    namespace PRIVATE {
        template<typename T>
        constexpr auto trivially_copyable = std::is_trivial_v<T> &&
                                            sizeof(T) <= sizeof(uintmax_t) && x86_64;
    }

    auto strlen(const char *__restrict str) -> size_t;

    CERBLIB_CLANG_DISABLE_WARNING("-Wconstant-evaluated")

    template<typename T>
    constexpr auto memset(void *__restrict ptr, T value, size_t times) -> void
    {
#ifdef __x86_64__
        if (!std::is_constant_evaluated() && PRIVATE::trivially_copyable<T>) {
            return PRIVATE::memset<T>(ptr, value, times);
        }
#endif

        auto *address  = static_cast<T *>(ptr);
        auto *ptr_copy = static_cast<T *>(ptr);

        CERBLIB_UNROLL_N(4)
        while (address < ptr_copy + times) {
            *(address++) = value;
        }
    }

    template<typename V, typename T, size_t Size>
    constexpr auto memset(std::array<T, Size> &t_array, V value, size_t times)
    {
        if (!std::is_constant_evaluated() && PRIVATE::trivially_copyable<T>) {
            memset(t_array.data(), value, times);

        } else {
            CERBLIB_UNROLL_N(4)
            for (size_t i = 0; i < times; i++) {
                t_array[i] = value;
            }
        }
    }

    template<typename T>
    constexpr auto memcpy(T *__restrict dest, const T *__restrict src, size_t times)
        -> void
    {
#ifdef __x86_64__
        if (!std::is_constant_evaluated() && PRIVATE::trivially_copyable<T>) {
            return PRIVATE::memcpy(dest, src, times);
        }
#endif /* __x86_64__ */

        T *dest_copy           = dest;
        const T *converted_src = src;
        T *converted_dest      = dest;

        CERBLIB_UNROLL_N(2)
        while (converted_dest < dest_copy + times) {
            *converted_dest = *converted_src;
            converted_src++;
            converted_dest++;
        }
    }

    template<typename T, size_t Size>
    constexpr auto memcpy(
        std::array<T, Size> &dest,
        const std::array<T, Size> &src,
        size_t times)
    {
        if (!std::is_constant_evaluated()) {
            memcpy(dest.data(), src.data(), times);
        } else {
            CERBLIB_UNROLL_N(4)
            for (size_t i = 0; i < Size; ++i) {
                dest[i] = src[i];
            }
        }
    }

    CERBLIB_CLANG_ENABLE_WARNING

    template<typename T>
    [[nodiscard]] consteval auto str2Uint(const char *str) -> T
    {
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);

        T result = 0;

        CERBLIB_UNROLL_N(4)
        for (size_t i = 0; i < sizeof(T); i++) {
            result |= static_cast<T>(
                static_cast<T>(bit_cast<unsigned char>(str[i]))
                << (i * bitsizeof(char)));
        }

        return result;
    }
}// namespace cerb

#endif /* CERBERUS_STRING_HPP */
