#ifndef CERBERUS_BIT_HPP
#define CERBERUS_BIT_HPP

#if __cplusplus > 201703L
#  include <bit>
#endif /* C++20 */

#include <cerberus/types.h>

namespace cerb {

    template<typename T>
    constexpr auto isPowerOf2(T value) -> bool {
        static_assert(std::is_integral_v<T>);
        return (value != 0) && (value & (value - 1)) == 0;
    }

    /**
     * @brief byte mask for different types.
     *
     * @tparam T type of bit
     */
    template<typename T>
    union CERBLIB_TRIVIAL ByteMask{
        T value;

        std::array<u8,  sizeof(T) / sizeof(u8 )> mask_u8 ;
        std::array<u16, sizeof(T) / sizeof(u16)> mask_u16;
        std::array<u32, sizeof(T) / sizeof(u32)> mask_u32;
        std::array<u64, sizeof(T) / sizeof(u64)> mask_u64;

    public:
        constexpr auto &getAsIntegral() noexcept {
            static_assert(
                sizeof(T) < sizeof(u64) && cerb::isPowerOf2(sizeof(T))
            );

            if constexpr (sizeof(T) == sizeof(u8)) {
                return mask_u8[0];
            } else if constexpr (sizeof(T) == sizeof(u16)) {
                return mask_u16[0];
            } else if constexpr (sizeof(T) == sizeof(u32)) {
                return mask_u32[0];
            } else if constexpr (sizeof(T) == sizeof(u64)) {
                return mask_u64[0];
            }
        }

    public:
        explicit constexpr ByteMask(T _value)
                : value(_value)
        {}
    };


    /**
     * @brief similar to C++20 std::bit_cast, but woks since C++17
     * @tparam TO target type (must be the same size as FROM and trivially copyable)
     * @tparam FROM current type (must be the same size as TO and trivially copyable)
     * @param x value to translate
     * @return bits of x as TO type
     */
    template<typename TO, typename FROM> [[nodiscard]]
    constexpr auto bit_cast(const FROM &x) noexcept -> TO {
        static_assert(
                sizeof(TO) == sizeof(FROM) &&
                std::is_trivially_copyable_v<FROM> &&
                std::is_trivially_copyable_v<TO>
        );
    #if __cplusplus >= 202002L
        return std::bit_cast<TO>(x);
    #else
        union { FROM current; TO target; } u = {x};
        return u.target;
    #endif /* C++20 */
    }

    template<typename T>
    class CERBLIB_TRIVIAL BitPattern {
        static_assert(std::is_integral_v<T>);
        T expected{0};
        T mask{~static_cast<T>(0)};

    public:
        constexpr friend auto operator==(const BitPattern<T>& pattern, const T& value) -> bool {
            return (value & pattern.mask) == pattern.expected;
        }

        constexpr friend auto operator==(const T& value, const BitPattern<T>& pattern) -> bool {
            return operator==(pattern, value);
        }

        constexpr friend auto operator!=(const BitPattern<T>& pattern, const T& value) -> bool {
            return !operator==(pattern, value);
        }

        constexpr friend auto operator!=(const T& value, const BitPattern<T>& pattern) -> bool {
            return !operator==(pattern, value);
        }

    public:
        template<int SIZE>
        CERBLIB_COMPILE_TIME explicit BitPattern(const char (&input)[SIZE]) {
            T cur_bit = (1<<(SIZE - 2));

            for (const auto elem: input) {
                if (elem == '\0') {
                    return;
                } else if (elem == '1') {
                    expected |= cur_bit;
                } else if (elem == 'x' || elem == 'X') {
                    mask &= ~cur_bit;
                }
                cur_bit >>= 1;
            }
        }
    };
}

#endif /* CERBERUS_BIT_HPP */