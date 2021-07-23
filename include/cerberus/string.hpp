#ifndef CERBERUS_STRING_HPP
#define CERBERUS_STRING_HPP

#include <cerberus/types.h>

namespace cerb {

    size_t strlen(const char * __restrict str);

#if defined(__x86_64__)

    always_inline void memset8(void *__restrict _ptr, u8 _value, size_t _times) {
       asm volatile (
            "rep stosb\n"
            : "+D" (_ptr), "+c" (_times)
            : "a" (_value)
            : "memory"
        );
    }

    always_inline void memset16(void *__restrict _ptr, u16 _value, size_t _times) {
        asm volatile (
            "rep stosw\n"
            : "+D" (_ptr), "+c" (_times)
            : "a" (_value)
            : "memory"
        );
    }

    always_inline void memset32(void *__restrict _ptr, u32 _value, size_t _times) {
       asm volatile (
            "rep stosl\n"
            : "+D" (_ptr), "+c" (_times)
            : "a" (_value)
            : "memory"
        );
    }

    always_inline void memset64(void *__restrict _ptr, u64 _value, size_t _times) {
        __asm__ __volatile__ (
            "rep stosq\n"
            : "+D" (_ptr), "+c" (_times)
            : "a" (_value)
            : "memory"
        );
    }

    always_inline void memcpy8(void *__restrict _dest, const void *__restrict _src, size_t _times) {
        asm volatile (
            "rep movsb"
            : "+S" (_src), "+D" (_dest), "+c" (_times)
            :
            : "memory"
        );
    }

    always_inline void memcpy16(void *__restrict _dest, const void *__restrict _src, size_t _times) {
        asm volatile (
            "rep movsw"
            : "+S" (_src), "+D" (_dest), "+c" (_times)
            :
            : "memory"
        );
    }

    always_inline void memcpy32(void *__restrict _dest, const void *__restrict _src, size_t _times) {
        asm volatile (
            "rep movsl"
            : "+S" (_src), "+D" (_dest), "+c" (_times)
            :
            : "memory"
        );
    }

    always_inline void memcpy64(void *__restrict _dest, const void *__restrict _src, size_t _times) {
        asm volatile (
            "rep movsq"
            : "+S" (_src), "+D" (_dest), "+c" (_times)
            :
            : "memory"
        );
    }

#else

    always_inline void memset8(void *__restrict _ptr, u8 _value, size_t _times) {
        u8 *_address_copy;
        u8 *_address = _address_copy = static_cast<u8*>(_ptr);

        while (_address < _address_copy + _times) {
            *(_address++) = _value;
        }
    }

    always_inline void memset16(void *__restrict _ptr, u16 _value, size_t _times) {
        u16 *_address_copy;
        u16 *_address = _address_copy = static_cast<u16*>(_ptr);

        while (_address < _address_copy + _times) {
            *(_address++) = _value;
        }
    }

    always_inline void memset32(void *__restrict _ptr, u32 _value, size_t _times) {
        u32 *_address_copy;
        u32 *_address = _address_copy = static_cast<u32*>(_ptr);

        while (_address < _address_copy + _times) {
            *(_address++) = _value;
        }
    }

    always_inline void memset64(void *__restrict _ptr, u64 _value, size_t _times) {
        u64 *_address_copy;
        u64 *_address = _address_copy = static_cast<u64*>(_ptr);

        while (_address < _address_copy + _times) {
            *(_address++) = _value;
        }
    }

    always_inline void memcpy8(void *__restrict _dest, const void *__restrict _src, size_t _times) {
        u8 *dest_copy;
        u8 *dest_ = dest_copy = static_cast<u8*>(_dest);
        const u8 *src_ = static_cast<const u8*>(_src);

        while (dest_ < dest_copy + _times) {
            *dest_ = *(src_);
            src_++;
            dest_++;
        }
    }

    always_inline void memcpy16(void *__restrict _dest, const void *__restrict _src, size_t _times) {
        u16 *dest_copy;
        u16 *dest_ = dest_copy = static_cast<u16*>(_dest);
        const u16 *src_ = static_cast<const u16*>(_src);

        while (dest_ < dest_copy + _times) {
            *dest_ = *(src_);
            src_++;
            dest_++;
        }
    }

    always_inline void memcpy32(void *__restrict _dest, const void *__restrict _src, size_t _times) {
        u32 *dest_copy;
        u32 *dest_ = dest_copy = static_cast<u32*>(_dest);
        const u32 *src_ = static_cast<const u32*>(_src);

        while (dest_ < dest_copy + _times) {
            *dest_ = *(src_);
            src_++;
            dest_++;
        }
    }

    always_inline void memcpy64(void *__restrict _dest, const void *__restrict _src, size_t _times) {
        u64 *dest_copy;
        u64 *dest_ = dest_copy = static_cast<u64*>(_dest);
        const u64 *src_ = static_cast<const u64*>(_src);

        while (dest_ < dest_copy + _times) {
            *dest_ = *(src_);
            src_++;
            dest_++;
        }
    }

#endif /* ARCH */

    [[nodiscard]]
    CERBLIB_COMPILE_TIME auto str2u16(const char *str) -> u16 {
        u16 result = 0;

        constexpr_for<0, sizeof(u16), 1>(
            [&](auto i){
                result += static_cast<u16>(str[i.value]) << (i.value * 8U);
            }
        );

        return result;
    }

    [[nodiscard]]
    CERBLIB_COMPILE_TIME auto str2u32(const char *str) -> u32 {
        u32 result = 0;

        constexpr_for<0, sizeof(u32), 1>(
            [&](auto i){
                result += static_cast<u32>(str[i.value]) << (i.value * 8U);
            }
        );

        return result;
    }

    [[nodiscard]]
    CERBLIB_COMPILE_TIME auto str2u64(const char *str) -> u64 {
        u64 result = 0;

        constexpr_for<0, sizeof(u64), 1>(
            [&](auto i){
                result += static_cast<u64>(str[i.value]) << (i.value * 8U);
            }
        );
        
        return result;
    }

    template<typename T>
    always_inline void memcpy(T *__restrict _dest, const T *__restrict _src, size_t _times) {
        if (_times == 0) UNLIKELY {
            return;
        }

        if constexpr (sizeof(T) == sizeof(u8)) {
            cerb::memcpy8(_dest, _src, _times);
        } else if constexpr (sizeof(T) == sizeof(u16)) {
            cerb::memcpy16(_dest, _src, _times);
        } else if constexpr (sizeof(T) == sizeof(u32)) {
            cerb::memcpy32(_dest, _src, _times);
        } else if constexpr (sizeof(T) == sizeof(u64)) {
            cerb::memcpy64(_dest, _src, _times);
        } else {
            cerb::memcpy8(_dest, _src, _times * sizeof(T));
        }
    }

    template<typename T, typename T2>
    always_inline void memset(T *__restrict _dest, T2 _value, size_t _times) {
        if (_times == 0) UNLIKELY {
            return;
        }

        static_assert(
            sizeof(T) == sizeof(u8) ||
            sizeof(T) == sizeof(u16) ||
            sizeof(T) == sizeof(u32) ||
            sizeof(T) == sizeof(u64)
        );

        if constexpr (sizeof(T) == sizeof(u8)) {
            cerb::memset8(_dest, static_cast<u8>(_value), _times);
        } else if constexpr (sizeof(T) == sizeof(u16)) {
            cerb::memset16(_dest, static_cast<u16>(_value), _times);
        } else if constexpr (sizeof(T) == sizeof(u32)) {
            cerb::memset32(_dest, static_cast<u32>(_value), _times);
        } else if constexpr (sizeof(T) == sizeof(u64)) {
            cerb::memset64(_dest, static_cast<u64>(_value), _times);
        }
    }
}

#endif /* CERBERUS_STRING_HPP */
