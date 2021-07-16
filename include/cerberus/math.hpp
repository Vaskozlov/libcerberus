#ifndef cerberusMath_hpp
#define cerberusMath_hpp

#include <cerberus/types.h>

#ifndef isinf
#  define isinf(x) __builtin_isinf (x)
#endif

#ifndef isnan
#  define isnan(x) __builtin_isnan (x)
#endif

#ifndef INFINITY
#  define INFINITY (__builtin_inff ())
#endif

#ifndef NAN
#  define NAN (__builtin_nanf (""))
#endif

namespace cerb {

    struct EmptyType {
        u8 empty;

        EmptyType() = delete;
        ~EmptyType() = delete;
        auto operator=(EmptyType &&)      -> EmptyType& = delete;
        auto operator=(const EmptyType &) -> EmptyType& = delete;
    };

    enum AlignMode : u32 {
        CEIL,
        TRUNC,
        ALIGN
    };
    
    template<typename T>
    constexpr T pi = static_cast<T>(3.14159265358979323846);

    /**
     * @brief returns maximum value of _lhs and _rhs
     * 
     * @tparam T 
     * @param _lhs first value
     * @param _rhs second value
     * @return T max(_lhs, _rhs) 
     */
    template<typename T>
    constexpr auto max(const T _lhs, const T _rhs) -> T {
        return cmov(_lhs > _rhs, _lhs, _rhs);
    }

    /**
     * @brief returns minimum value of _lhs and _rhs
     * 
     * @tparam T
     * @param _lhs first value
     * @param _rhs second value
     * @return T min(_lhs, _rhs) 
     */
    template<typename T>
    constexpr auto min(const T _lhs, const T _rhs) -> T {
        return cmov(_lhs < _rhs, _lhs, _rhs);
    }

    /**
     * @brief 2^power for floating point and integeral numbers
     * 
     * @tparam T 
     * @param power 2 will be raised in power of "power" (power must be unsigned integeral)
     * @return T(2)^power
     */
    template<typename T>
    constexpr auto pow2(u32 power) -> T {
        if constexpr (std::is_floating_point<T>::value) {
            static_assert((sizeof(T) == sizeof(u32) && std::is_same<T, float>::value) || (sizeof(T) == sizeof(u64) && std::is_same<T, double>::value));

            if constexpr (sizeof(T) == sizeof(u32)) {
                cerb::ByteMask<float> a{1.0F};
                a.mask32[0] += static_cast<u32>(0x800000U) * power;
                return a.value;
            } else {
                cerb::ByteMask<double> a{1.0};
                a.mask64[0] += static_cast<u64>(0x10000000000000UL) * power;
                return a.value;
            }
        } else if constexpr (std::is_integral<T>::value) {
            return static_cast<T>(1) << power;
        } else {
            static_assert(std::is_integral<T>::value);
        }
    }

    /**
     * @brief returns absolute value of argument
     * 
     * @tparam ResultType if it is set function converts result to this type
     * @tparam T 
     * @param value 
     * @return T or ResultType
     */
    template<typename ResultType = EmptyType, typename T>
    constexpr auto abs(T value) {
        if constexpr(std::is_unsigned<T>::value) {
            return value;
        }
        
        if constexpr (std::is_floating_point<T>::value) {
            if constexpr (sizeof(T) == sizeof(u32)) {
                ByteMask<T> mask{value};
                mask.mask32[0] &= INT32_MAX;
                return mask.value;
            } else if constexpr (sizeof(T) == sizeof(u64)) {
                ByteMask<T> mask{value};
                mask.mask64[0] &= INT64_MAX;
                return mask.value;
            }
        }

        if constexpr (std::is_same<ResultType, EmptyType>::value){
            return cmov(value < 0, -value, value);
        } else {
            return static_cast<ResultType>(cmov(value < 0, -value, value));
        }
    }

    template<u32 powerOf2, auto mode = AlignMode::ALIGN, typename T>
    constexpr auto align(T value) -> T {
        static_assert(std::is_integral<T>::value);
        
        if constexpr (mode == AlignMode::TRUNC) {
            return value & ~(pow2<T>(powerOf2) - 1);
        } else if constexpr (mode == AlignMode::CEIL) {
            return value + (pow2<T>(powerOf2) - (value & (pow2<T>(powerOf2) - 1)));
        } else {
            return cmov((value & (pow2<T>(powerOf2) - 1)) == 0, value, align<powerOf2, AlignMode::CEIL>(value));
        }
    }

    /**
     * @brief log2 from value
     * 
     * @tparam T - only integer
     * @param value 
     * @return log2(value)
     */
    template<typename T>
    constexpr auto log2(const T value) -> T {
        static_assert(std::is_integral<T>::value);
        if constexpr (sizeof(T) <= sizeof(u32)) {
            return (bitsizeof(u32) - 1) - __builtin_clz(value);
        } else {
            return (bitsizeof(u64) - 1) - __builtin_clzl(value);
        }
    }

    /**
     * @brief find free bit in 64 bit integer. UNSAFE if value does not have free bit
     * 
     * @param value 
     * @return u32 location of free bit
     */
    always_inline auto findFreeBit(u64 value) -> u32 {
        #if defined(__x86_64__)
             u64 result = 0;

            __asm__ __volatile__(
                "xorq %0, %0\n\t"
                "1:\n\t"
                "bt %0, %1\n\t"
                "jae 2f\n\t"
                "incq %0\n\t"
                "jmp 1b\n\t"
                "2:\n\t"
                : "=a" (result)
                : "D" (value)
            );

            return result;
        #else
            u64 j = 0;

            for (; value > 0; j++) {
                if ((value & static_cast<u64>(1)) == 0) {
                    return j;
                }
                value >>= 1;
            }
            return j + 1;
        #endif /* ARCH */
        
    }
    
    /**
     * @brief find first bit which contains 1. UNSAFE if value does not have them
     * 
     * @param value 
     * @return location of set bit
     */
    always_inline auto findSetBit(u64 value) -> u32 {
        #if defined(__x86_64__)
            u64 result = 0;

            __asm__ __volatile__(
                "xorq %0, %0\n\t"
                "1:\n\t"
                "bt %0, %1\n\t"
                "jc 2f\n\t"
                "incq %0\n\t"
                "jmp 1b\n\t"
                "2:\n\t"
                : "=a" (result)
                : "D" (value)
            );

            return result;
        #else
            u64 j = 0;

            for (; value > 0; j++) {
                if ((value & static_cast<u64>(1)) == 1) {
                    return j;
                }
                value >>= 1;
            }
        
            return UINTMAX_MAX;
        #endif /* ARCH */
    }
} // namespace cerb

#endif /* cerberusMath_hpp */
