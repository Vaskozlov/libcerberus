#ifndef CERBERUS_PRIVATE_STRING_HPP
#define CERBERUS_PRIVATE_STRING_HPP

#ifndef CERBERUS_STRING_HPP
#  error This file must be included from cerberus/string.hpp
#endif /* CERBERUS_STRING_HPP */

namespace cerb::PRIVATE {
    CERBLIB_INLINE
    void memset8(void *__restrict ptr, u8 value2set, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep stosb\n"
                : "+D" (ptr), "+c" (times)
                : "a" (value2set)
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    CERBLIB_INLINE
    void memset16(void *__restrict ptr, u16 value, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep stosw\n"
                : "+D" (ptr), "+c" (times)
                : "a" (value)
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    CERBLIB_INLINE
    void memset32(void *__restrict ptr, u32 value, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep stosl\n"
                : "+D" (ptr), "+c" (times)
                : "a" (value)
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    CERBLIB_INLINE
    void memset64(void *__restrict ptr, u64 value, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep stosq\n"
                : "+D" (ptr), "+c" (times)
                : "a" (value)
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    CERBLIB_INLINE
    void memcpy8(void *__restrict dest, const void *__restrict src, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep movsb"
                : "+S" (src), "+D" (dest), "+c" (times)
                :
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    CERBLIB_INLINE
    void memcpy16(void *__restrict dest, const void *__restrict src, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep movsw"
                : "+S" (src), "+D" (dest), "+c" (times)
                :
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    CERBLIB_INLINE
    void memcpy32(void *__restrict dest, const void *__restrict src, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep movsl"
                : "+S" (src), "+D" (dest), "+c" (times)
                :
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    CERBLIB_INLINE
    void memcpy64(void *__restrict dest, const void *__restrict src, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep movsq"
                : "+S" (src), "+D" (dest), "+c" (times)
                :
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    template<typename T> CERBLIB_INLINE
    auto memset(void *__restrict ptr, T value, size_t times) -> void {
        static_assert(
            (std::is_pointer_v<T> ||
            std::is_integral_v<T>) &&
            sizeof(T) <= sizeof(u64)
        );

        if (times == 0) UNLIKELY {
            return;
        }

        if constexpr(sizeof(T) == sizeof(u8)) {
            cerb::PRIVATE::memset8(
                    ptr,
                    cerb::bit_cast<u8>(value),
                    times
            );
        } else if constexpr (sizeof(T) == sizeof(u16)) {
            cerb::PRIVATE::memset16(
                    ptr,
                    cerb::bit_cast<u16>(value),
                    times
            );
        } else if constexpr (sizeof(T) == sizeof(u32)) {
            cerb::PRIVATE::memset32(
                    ptr,
                    cerb::bit_cast<u32>(value),
                    times
            );
        } else if constexpr (sizeof(T) == sizeof(u64)) {
            cerb::PRIVATE::memset64(
                    ptr,
                    cerb::bit_cast<u64>(value),
                    times
            );
        }
    }

    template<typename T> CERBLIB_INLINE
    void memcpy(T *__restrict dest, const T *__restrict src, size_t times) {
        if (times == 0) UNLIKELY {
            return;
        }

        if constexpr (sizeof(T) == sizeof(u8)) {
            cerb::PRIVATE::memcpy8(
                    reinterpret_cast<u8*>(dest),
                    reinterpret_cast<const u8*>(src),
                    times
            );
        } else if constexpr (sizeof(T) == sizeof(u16)) {
            cerb::PRIVATE::memcpy16(
                    reinterpret_cast<u16*>(dest),
                    reinterpret_cast<const u16*>(src),
                    times
            );
        } else if constexpr (sizeof(T) == sizeof(u32)) {
            cerb::PRIVATE::memcpy32(
                    reinterpret_cast<u32*>(dest),
                    reinterpret_cast<const u32*>(src),
                    times
            );
        } else if constexpr (sizeof(T) == sizeof(u64)) {
            cerb::PRIVATE::memcpy64(
                    reinterpret_cast<u64*>(dest),
                    reinterpret_cast<const u64*>(src),
                    times
            );
        } else {
            cerb::PRIVATE::memcpy8(
                    reinterpret_cast<u8*>(dest),
                    reinterpret_cast<const u8*>(src),
                    times * sizeof(T)
            );
        }
    }
}

#endif /* CERBERUS_PRIVATE_STRING_HPP */
