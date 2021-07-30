#ifndef CERBERUS_STRING_HPP
#define CERBERUS_STRING_HPP

#include <cerberus/types.h>
#include <cerberus/bit.hpp>
#include <cerberus/private/string.hpp>

namespace cerb {

    size_t strlen(const char * __restrict str);

    template<typename T,
            CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR>
    CERBLIB_INLINE
    void memset(void *__restrict ptr, T value, size_t times) {
        static_assert(
            std::is_integral_v<T> &&
            sizeof(T) <= sizeof(u64)
        );

        if (times == 0) UNLIKELY {
            return;
        }
        if constexpr(sizeof(T) == sizeof(u8)) {
            cerb::PRIVATE::memset8<Constexpr>(ptr, cerb::bit_cast<u8>(value), times);
        } else if constexpr (sizeof(T) == sizeof(u16)) {
            cerb::PRIVATE::memset16<Constexpr>(ptr, cerb::bit_cast<u16>(value), times);
        } else if constexpr (sizeof(T) == sizeof(u32)) {
            cerb::PRIVATE::memset32<Constexpr>(ptr, cerb::bit_cast<u32>(value), times);
        } else if constexpr (sizeof(T) == sizeof(u64)) {
            cerb::PRIVATE::memset16<Constexpr>(ptr, cerb::bit_cast<u64>(value), times);
        }
    }

    CERBLIB_DISABLE_WARNING(constant-evaluated,constant-evaluated,0)

    template<typename T,
    CERBLIB_NOT_FOR_X86_64_RUNTIME>
    constexpr void memset(void *__restrict ptr, T value, size_t times) {
        static_assert(std::is_trivially_copyable_v<T> && std::is_trivially_constructible_v<T>);

        #if (__cplusplus >= 202002L)
        if constexpr (
                !std::is_constant_evaluated() &&
                cerb::x86_64 &&
                std::is_integral_v<T> &&
                sizeof(T) <= sizeof(u64)
            ) {
                return cerb::memset<T, false>(ptr, value, times);
            }
        #endif

        decltype(value) *copy_of_ptr;
        auto *address = copy_of_ptr = static_cast<decltype(value) *>(ptr);

        CERBLIB_UNROLL_N(4)
        while (address < copy_of_ptr + times) {
            *(address++) = value;
        }
    }

    template<typename T,
            CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR>
    CERBLIB_INLINE constexpr
    void memcpy(T *__restrict dest, const T *__restrict src, size_t times) {
        if (times == 0) UNLIKELY {
            return;
        }
        if constexpr (sizeof(T) == sizeof(u8)) {
            cerb::PRIVATE::memcpy8<Constexpr>(dest, src, times);
        } else if constexpr (sizeof(T) == sizeof(u16)) {
            cerb::PRIVATE::memcpy16<Constexpr>(dest, src, times);
        } else if constexpr (sizeof(T) == sizeof(u32)) {
            cerb::PRIVATE::memcpy32<Constexpr>(dest, src, times);
        } else if constexpr (sizeof(T) == sizeof(u64)) {
            cerb::PRIVATE::memcpy64<Constexpr>(dest, src, times);
        } else {
            cerb::PRIVATE::memcpy8<Constexpr>(dest, src, times * sizeof(T));
        }
    }

    template<typename T,
            CERBLIB_NOT_FOR_X86_64_RUNTIME>
    CERBLIB_INLINE CERBLIB20_CONSTEXPR
    void memcpy(T *__restrict dest, const T *__restrict src, size_t times) {
        #if (__cplusplus >= 202002L)
            if constexpr (!std::is_constant_evaluated() == false && cerb::x86_64) {
                return cerb::memcpy<T, false>(dest, src, times);
            } else {
                return cerb::PRIVATE::memcpy<T, true>(dest, src, times);
            }
        #else
            cerb::PRIVATE::memcpy<T, Constexpr>(dest, src, times);
        #endif
    }

    CERBLIB_ENABLE_WARNING(constant-evaluated,constant-evaluated,0)

    template<typename T>
    [[nodiscard]] CERBLIB_COMPILE_TIME
    auto str2uint(const char *str) -> T {
        static_assert(
                std::is_integral_v<T> &&
                std::is_unsigned_v<T>
        );
        T result = 0;

        constexpr_for<0, static_cast<int>(sizeof(T)), 1>(
            [&](auto i){
                result += static_cast<T>(static_cast<int>(str[i.value]) << (i.value * 8));
            }
        );

        return result;
    }
}

#endif /* CERBERUS_STRING_HPP */
