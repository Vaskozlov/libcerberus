#ifndef CERBERUS_PRIVATE_STRING_HPP
#define CERBERUS_PRIVATE_STRING_HPP

#ifndef CERBERUS_STRING_HPP
#    error This file must be included from cerberus/string.hpp
#endif /* CERBERUS_STRING_HPP */

namespace cerb::PRIVATE {

#if defined(__x86_64__)
    CERBLIB_INLINE
    auto memset8(void *__restrict ptr, u8 value2set, size_t times) -> void
    {
        asm volatile("rep stosb\n"
                     : "+D"(ptr), "+c"(times)
                     : "a"(value2set)
                     : "memory");
    }

    CERBLIB_INLINE
    auto memset16(void *__restrict ptr, u16 value, size_t times) -> void
    {
        asm volatile("rep stosw\n" : "+D"(ptr), "+c"(times) : "a"(value) : "memory");
    }

    CERBLIB_INLINE
    auto memset32(void *__restrict ptr, u32 value, size_t times) -> void
    {
        asm volatile("rep stosl\n" : "+D"(ptr), "+c"(times) : "a"(value) : "memory");
    }

    CERBLIB_INLINE
    auto memset64(void *__restrict ptr, u64 value, size_t times) -> void
    {
        asm volatile("rep stosq\n" : "+D"(ptr), "+c"(times) : "a"(value) : "memory");
    }

    CERBLIB_INLINE
    auto memcpy8(void *__restrict dest, const void *__restrict src, size_t times)
        -> void
    {
        asm volatile("rep movsb" : "+S"(src), "+D"(dest), "+c"(times) : : "memory");
    }

    CERBLIB_INLINE
    auto memcpy16(void *__restrict dest, const void *__restrict src, size_t times)
        -> void
    {
        asm volatile("rep movsw" : "+S"(src), "+D"(dest), "+c"(times) : : "memory");
    }

    CERBLIB_INLINE
    auto memcpy32(void *__restrict dest, const void *__restrict src, size_t times)
        -> void
    {
        asm volatile("rep movsl" : "+S"(src), "+D"(dest), "+c"(times) : : "memory");
    }

    CERBLIB_INLINE
    auto memcpy64(void *__restrict dest, const void *__restrict src, size_t times)
        -> void
    {
        asm volatile("rep movsq" : "+S"(src), "+D"(dest), "+c"(times) : : "memory");
    }
#endif /* __x86_64__ */

    template<typename T>
    CERBLIB_INLINE auto memset(void *__restrict ptr, T value, size_t times) -> void
    {
        if (times == 0) [[unlikely]] {
            return;
        }

        if constexpr (sizeof(T) == sizeof(u8)) {
            PRIVATE::memset8(ptr, bit_cast<u8>(value), times);
        } else if constexpr (sizeof(T) == sizeof(u16)) {
            PRIVATE::memset16(ptr, bit_cast<u16>(value), times);
        } else if constexpr (sizeof(T) == sizeof(u32)) {
            PRIVATE::memset32(ptr, bit_cast<u32>(value), times);
        } else if constexpr (sizeof(T) == sizeof(u64)) {
            PRIVATE::memset64(ptr, bit_cast<u64>(value), times);
        } else {
            static_assert(sizeof(T) <= sizeof(u64));
        }
    }

    template<typename T>
    CERBLIB_INLINE auto
        memcpy(T *__restrict dest, const T *__restrict src, size_t times) -> void
    {
        if (times == 0) [[unlikely]] {
            return;
        }

        if constexpr (sizeof(T) == sizeof(u8)) {
            PRIVATE::memcpy8(
                reinterpret_cast<u8 *>(dest),
                reinterpret_cast<const u8 *>(src),
                times);
        } else if constexpr (sizeof(T) == sizeof(u16)) {
            PRIVATE::memcpy16(
                reinterpret_cast<u16 *>(dest),
                reinterpret_cast<const u16 *>(src),
                times);
        } else if constexpr (sizeof(T) == sizeof(u32)) {
            PRIVATE::memcpy32(
                reinterpret_cast<u32 *>(dest),
                reinterpret_cast<const u32 *>(src),
                times);
        } else if constexpr (sizeof(T) == sizeof(u64)) {
            PRIVATE::memcpy64(
                reinterpret_cast<u64 *>(dest),
                reinterpret_cast<const u64 *>(src),
                times);
        } else {
            PRIVATE::memcpy8(
                reinterpret_cast<u8 *>(dest),
                reinterpret_cast<const u8 *>(src),
                times * sizeof(T));
        }
    }
}// namespace cerb::PRIVATE

#endif /* CERBERUS_PRIVATE_STRING_HPP */
