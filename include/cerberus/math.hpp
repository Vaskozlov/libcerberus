#ifndef cerberusMath_hpp
#define cerberusMath_hpp

#include <cerberus/types.h>
#include <cerberus/bit.hpp>
#include <cerberus/literals.hpp>

#ifndef isinf
#    define isinf(x) __builtin_isinf(x)
#endif

#ifndef isnan
#    define isnan(x) __builtin_isnan(x)
#endif

#ifndef INFINITY
#    define INFINITY (__builtin_inff())
#endif

#ifndef NAN
#    define NAN (__builtin_nanf(""))
#endif

#if defined(_MSC_VER)
namespace cerb::PRIVATE {
    template<unsigned Value>
    [[nodiscard]] constexpr auto findBitForward(u64 value) -> u64 {
        u64 j = 0;

        for (; value > 0; j++) {
            if ((value & 0b1ull) == Value) {
                return j;
            }
            value >>= 1;
        }
        return j + 1;
    }

    template<unsigned Value>
    [[nodiscard]] constexpr auto findBitReverse(u64 value) -> u64 {
        u64 j = 63;

        for (; value > 0; j--) {
            if ((value & (1ULL << 63ULL)) == Value) {
                return j;
            }
            value <<= 1;
        }
        return j + 1;
    }
}// namespace cerb::PRIVATE
#endif /* _MSC_VER */

namespace cerb {

    using namespace ::cerb::literals;

    enum AlignMode : u32
    {
        CEIL,
        TRUNC,
        ALIGN
    };

    template<typename T = double>
    constexpr T PI = static_cast<T>(3.14159265358979323846);

    /**
     * @brief returns maximum value of lhs and rhs
     *
     * @tparam T
     * @param lhs first value
     * @param rhs second value
     * @return T max(lhs, rhs)
     */
    template<typename T>
    constexpr auto max(const T &lhs, const T &rhs) -> const T & {
        return cmov(lhs > rhs, lhs, rhs);
    }

    /**
     * @brief returns minimum value of lhs and rhs
     *
     * @tparam T
     * @param lhs first value
     * @param rhs second value
     * @return T min(lhs, rhs)
     */
    template<typename T>
    constexpr auto min(const T &lhs, const T &rhs) -> const T & {
        return cmov(lhs < rhs, lhs, rhs);
    }

    /**
     * @brief 2^power for floating point and integral numbers
     *
     * @tparam T
     * @param power 2 will be raised in power of "power" (power must be unsigned integral)
     * @return T(2)^power
     */
    template<typename T>
    constexpr auto pow2(u32 power) -> T {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);

        if constexpr (std::is_floating_point_v<T>) {
            static_assert(sizeof(T) == sizeof(u32) || sizeof(T) == sizeof(u64));

            cerb::ByteMask<T> mask{ static_cast<T>(1.0) };

            if constexpr (sizeof(T) == sizeof(u32)) {
                mask.getAsIntegral() += "x80 0000"_2val * power;
            } else {
                mask.getAsIntegral() += "x10 0000 0000 0000"_2val * power;
            }

            return mask.value;
        } else if constexpr (std::is_integral_v<T>) {
            return static_cast<T>(1) << power;
        } else {
            return 0;
        }
    }

    /**
     * @brief returns absolute value of argument
     *
     * @tparam ResultType if it is Set function converts result to this type
     * @tparam T
     * @param value
     * @return T or ResultType
     */
    template<typename ResultType = EmptyType, typename T>
    constexpr auto abs(T value) {
        if constexpr (std::is_unsigned_v<T>) {
            return value;
        } else if constexpr (std::is_floating_point_v<T>) {
            ByteMask<T> mask(value);

            if constexpr (sizeof(T) == sizeof(u32)) {
                mask.getAsIntegral() &= INT32_MAX;
            } else {
                mask.getAsIntegral() &= INT64_MAX;
            }

            if constexpr (std::is_same_v<ResultType, EmptyType>) {
                return mask.value;
            } else {
                return static_cast<ResultType>(mask.value);
            }
        } else if constexpr (std::is_same_v<ResultType, EmptyType>) {
            return cmov(value < 0, -value, value);
        } else {
            return static_cast<ResultType>(cmov(value < 0, -value, value));
        }
    }

    template<u32 powerOf2, auto mode = AlignMode::ALIGN, typename T>
    constexpr auto align(T value) -> T {
        static_assert(std::is_integral_v<T>);

        if constexpr (mode == AlignMode::TRUNC) {
            return value & ~(pow2<T>(powerOf2) - 1);
        } else if constexpr (mode == AlignMode::CEIL) {
            return value + (pow2<T>(powerOf2) - value % pow2<T>(powerOf2));
        } else {
            return cmov(
                value % pow2<T>(powerOf2) == 0,
                value, align<powerOf2, AlignMode::CEIL>(value));
        }
    }

    [[nodiscard]] constexpr auto findSetBitForward(u64 value) -> u64 {
        if (value == 0) {
            return UINTMAX_MAX;
        }

#if defined(__clang__) || defined(__GNUC__)
        return __builtin_ctzl(value);
#elif defined(_MSC_VER)
        if (std::is_constant_evaluated()) {
            return PRIVATE::findBitForward<1>(value);
        } else {
            unsigned long result;
            _BitScanForward64(&result, value);
            return result;
        }
#else
#    error Unsupported compiler
#endif
    }

    [[nodiscard]] constexpr auto findFreeBitForward(u64 value) -> u64 {
        return findSetBitForward(~value);
    }

    [[nodiscard]] constexpr auto findSetBitReverse(u64 value) -> u64 {
        if (value == 0) {
            return UINTMAX_MAX;
        }

#if defined(__clang__) || defined(__GNUC__)
        return __builtin_clzl(value);
#elif defined(_MSC_VER)
        if (std::is_constant_evaluated()) {
            return PRIVATE::findBitForward<1>(value);
        } else {
            unsigned long result;
            _BitScanForward64(&result, value);
            return result;
        }
#else
#    error Unsupported compiler
#endif
    }

    [[nodiscard]] constexpr auto findFreeBitReverse(u64 value) -> u64 {
        return findSetBitReverse(~value);
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
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>);

        if constexpr (std::is_integral_v<T>) {
#if defined(__clang__) || defined(__GNUC__)
            if constexpr (sizeof(T) <= sizeof(u32)) {
                return (bitsizeof(u32) - 1) - __builtin_clz(value);
            } else {
                return (bitsizeof(u64) - 1) - __builtin_clzl(value);
            }
#else
            return findSetBitReverse(static_cast<u64>(value));
#endif
        } else {
            cerb::ByteMask<T> mask(value);

            if constexpr (sizeof(T) == sizeof(u32)) {
                return ((mask.getAsIntegral() & "xFF80 0000"_2val) >> 23) - 0x7fu;
            } else {
                return ((mask.getAsIntegral() & "xFFF0 0000 0000 0000"_2val) >> 52) - 0x3ffu;
            }
        }
    }
}// namespace cerb

#endif /* cerberusMath_hpp */
