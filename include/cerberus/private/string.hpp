#ifndef CERBERUS_PRIVATE_STRING_HPP
#define CERBERUS_PRIVATE_STRING_HPP

#ifndef CERBERUS_STRING_HPP
#  error This file must be included from cerberus/string.hpp
#endif /* CERBERUS_STRING_HPP */

namespace cerb::PRIVATE {
    template<CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR>
    CERBLIB_INLINE void memset8(void *__restrict ptr, u8 value2set, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep stosb\n"
                : "+D" (ptr), "+c" (times)
                : "a" (value2set)
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    template<CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR>
    CERBLIB_INLINE void memset16(void *__restrict ptr, u16 value, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep stosw\n"
                : "+D" (ptr), "+c" (times)
                : "a" (value)
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    template<CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR>
    CERBLIB_INLINE void memset32(void *__restrict ptr, u32 value, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep stosl\n"
                : "+D" (ptr), "+c" (times)
                : "a" (value)
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    template<CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR>
    CERBLIB_INLINE void memset64(void *__restrict ptr, u64 value, size_t times) {
        #if defined(__x86_64__)
            asm volatile (
                "rep stosq\n"
                : "+D" (ptr), "+c" (times)
                : "a" (value)
                : "memory"
            );
        #endif /* __x86_64__ */
    }

    template<CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR>
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

    template<CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR>
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

    template<CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR>
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

    template<CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR>
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

    template<typename T,
            CERBLIB_NOT_FOR_X86_64_RUNTIME>
    constexpr void memcpy(void *__restrict dest, const void *__restrict src, size_t times) {
        static_assert(std::is_integral_v<T>);

        T *dest_copy;
        const T *converted_src = static_cast<const T*>(src);
        T *converted_dest = dest_copy = static_cast<T*>(dest);

        while (converted_dest < dest_copy + times) {
            *converted_dest = *converted_src;
            converted_src++;
            converted_dest++;
        }
    }
}

#endif /* CERBERUS_PRIVATE_STRING_HPP */
