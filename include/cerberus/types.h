#ifndef cerberusTypes_h
#define cerberusTypes_h

#if defined(__cplusplus)
#  include <cstddef>
#  include <cinttypes>
#else
#  include <stddef.h>
#  include <inttypes.h>
#endif /* __cplusplus */

#if defined(__cplusplus)
#  if (__cplusplus == 201103L)
#    define CERBLIB_CXX 2011
#  elif (__cplusplus == 201402L)
#    define CERBLIB_CXX 2014
#  elif (__cplusplus == 201703L)
#    define CERBLIB_CXX 2017
#  elif (__cplusplus == 202002L)
#    define CERBLIB_CXX 2020
#  elif (__cplusplus > 202002L)
#    define CERBLIB_CXX (20 * 100 + (__cplusplus / 100 % 100))
#  endif /* C++ standard */
#endif /* __cplusplus */

#if defined(CERBLIB_CXX)
    using i8  = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;
    
    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
#else
    typedef int8_t  i8 ;
    typedef int16_t i16;
    typedef int32_t i32;
    typedef int64_t i64;

    typedef uint8_t  u8 ;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
#endif /* __cplusplus */

#if defined(CERBLIB_CXX)
namespace cerb {
    using byte = u8;
} // namespace cerb
#else
    typedef u8 byte;
#endif /* _cplusplus */

#if !defined(bitsizeof)
#  define bitsizeof(x) (sizeof(x) * 8)
#endif /* bitsizeof */

#if defined(asm)
#  define asm __asm__
#  define volatile __volatile
#endif /* asm */

#if !defined(STRX) && !defined(STR)
#  define STRX(x) #x
#  define STR(x) STRX(x)
#endif /* STR and STRX */

#ifndef offsetof
#  define offsetof(t, d) __builtin_offsetof(t, d)
#endif /* offsetof */

#if defined(__cplusplus) && !defined(__BEGIN_DECLS)
#  define __BEGIN_DECLS extern "C" {
#  define __END_DECLS }
#elif !defined(__BEGIN_DECLS)
#  define __BEGIN_DECLS
#  define __END_DECLS
#endif /* __BEGIN_DECLS */

#if (CERBLIB_CXX < 2017)
#  error cerberus requires at least C++17
#endif /* C++17 */

#if !defined(always_inline)
#define always_inline   __attribute__((always_inline)) inline
#endif /* always_inline */

#if defined(CERBLIB_CXX) && (CERBLIB_CXX <= 2017)
// C++17 here
#  define LIKELY
#  define UNLIKELY
#elif defined(CERBLIB_CXX)
// C++20 here
#  define LIKELY    [[likely]]
#  define UNLIKELY  [[unlikely]]
#endif /* C++17-20 */

#if defined(__clang__) && !defined(TRIVIAL)
#  define TRIVIAL __attribute__((trivial_abi))
#elif !defined(TRIVIAL)
#  define TRIVIAL
#endif

#ifndef CERBLIB_PURE
#  define CERBLIB_PURE __attribute__ ((__pure__))
#endif /* CERBLIB_PURE */

#ifndef CERBLIB_CONST
#  define CERBLIB_CONST __attribute__ ((__const__))
#endif /* CERBLIB_CONST */

#ifndef CERBLIB_NORETURN
#  define CERBLIB_NORETURN __attribute__ ((__noreturn__))
#endif /* CERBLIB_NORETURN */

#if defined(__DEPRECATED) && !defined(CERBLIB_DEPRECATED)
#  define CERBLIB_DEPRECATED __attribute__ ((__deprecated__))
#  define CERBLIB_DEPRECATED_SUGGEST(ALT) \
  __attribute__ ((__deprecated__ ("use '" ALT "' instead")))
#else
#  define CERBLIB_DEPRECATED
#  define CERBLIB_DEPRECATED_SUGGEST(ALT)
#endif /* CERBLIB_DEPRECATED */

#if defined(__DEPRECATED) && (CERBLIB_CXX >= 2017)
#  define CERBLIB17_DEPRECATED __attribute__ ((__deprecated__))
#  define CERBLIB17_DEPRECATED_SUGGEST(ALT) \
  __attribute__ ((__deprecated__ ("use '" ALT "' instead")))
#else
#  define CERBLIB17_DEPRECATED
#  define CERBLIB17_DEPRECATED_SUGGEST(ALT)
#endif /* CERBLIB17_DEPRECATED */

#if defined(__DEPRECATED) && (CERBLIB_CXX >= 2020)
#  define CERBLIB20_DEPRECATED __attribute__ ((__deprecated__))
#  define CERBLIB20_DEPRECATED_SUGGEST(ALT) \
  __attribute__ ((__deprecated__ ("use '" ALT "' instead")))
#else
#  define CERBLIB20_DEPRECATED
#  define CERBLIB20_DEPRECATED_SUGGEST(ALT)
#endif /* CERBLIB20_DEPRECATED */

#if !defined(CERBLIB17_CONSTEXPR) && (CERBLIB_CXX >= 2017)
#  define CERBLIB17_CONSTEXPR constexpr
#else
#  define CERBLIB17_CONSTEXPR always_inline
#endif /* CERBLIB17_CONSTEXPR */

#if !defined(CERBLIB20_CONSTEXPR) && (CERBLIB_CXX > 2017)
#  define CERBLIB20_CONSTEXPR constexpr
#else
#  define CERBLIB20_CONSTEXPR always_inline
#endif /* CERBLIB20_CONSTEXPR */

#ifndef CERBLIB_COMPILE_TIME
#  if CERBLIB_CXX <= 2017
#    define CERBLIB_COMPILE_TIME constexpr
#  else
#    define CERBLIB_COMPILE_TIME consteval
#  endif /* C++17 */
#endif /* CERBLIB_COMPILE_TIME */

#if CERBLIB_CXX <= 2020
    constexpr auto operator ""_z(unsigned long long x) -> size_t {
        return static_cast<size_t>(x);
    }

    constexpr auto operator ""_Z(unsigned long long x) -> size_t {
        return static_cast<size_t>(x);
    }
#else
/*
 * Do later, when C++23 will be released
 */
#endif /* < C++20 */

#if defined(CERBLIB_CXX)
#  if CERBLIB_CXX <= 2017
#    define CERBLIB_THREE_WAY_COMPARISON 0
#  elif defined(__clang__) && __clang_major__ >= 10 || defined(__GNUC__) && __GNUC__ >= 10
#    define CERBLIB_THREE_WAY_COMPARISON 1
#  else
#    define CERBLIB_THREE_WAY_COMPARISON 0
#  endif /* CERBLIB_THREE_WAY_COMPARISON */

#  define GEN_COMPARISON_RULES(x, m) \
    always_inline friend bool operator==(const x &_lhs, const x &_rhs) { \
        return _lhs.m == _rhs.m; \
    } \
    always_inline friend bool operator!=(const x &_lhs, const x &_rhs) { \
        return _lhs.m != _rhs.m; \
    } \
    always_inline friend bool operator>(const x &_lhs, const x &_rhs) { \
        return _lhs.m > _rhs.m; \
    } \
    always_inline friend bool operator<(const x &_lhs, const x &_rhs) { \
        return _lhs.m < _rhs.m; \
    } \
    always_inline friend bool operator>=(const x &_lhs, const x &_rhs) { \
        return _lhs.m >= _rhs.m; \
    } \
    always_inline friend bool operator<=(const x &_lhs, const x &_rhs) { \
        return _lhs.m <= _rhs.m; \
    }
#endif /* _cplusplus */

#if defined(CERBLIB_CXX)
#  include <array>
#  include <atomic>
#  include <utility>
#  include <type_traits>

#  if CERBLIB_CXX >= 2020
#    include <bit>
#  endif /* C++20 */

namespace cerb {

    namespace PRIVATE {
        inline std::atomic<size_t> uid = 0;
    }

    /**
     * @brief thread safe return unsigned value
     *
     * @return unique unsigned value
     */
    always_inline auto uuid() -> size_t {
        return PRIVATE::uid++;
    }

    /**
     * @brief byte mask for different types.
     * 
     * @tparam T type of bitmask
     */
    template<typename T>
    union TRIVIAL ByteMask{
        T value;
        std::array<u8,  sizeof(T) / sizeof(u8) > mask8 ;
        std::array<u16, sizeof(T) / sizeof(u16)> mask16;
        std::array<u32, sizeof(T) / sizeof(u32)> mask32;
        std::array<u64, sizeof(T) / sizeof(u64)> mask64;

        std::array<i8,  sizeof(T) / sizeof(u8) > maski8 ;
        std::array<i16, sizeof(T) / sizeof(u16)> maski16;
        std::array<i32, sizeof(T) / sizeof(u32)> maski32;
        std::array<i64, sizeof(T) / sizeof(u64)> maski64;

    public:
        CERBLIB17_CONSTEXPR auto &getBits() const noexcept {
            static_assert(
                sizeof(T) == sizeof(u8) ||
                sizeof(T) == sizeof(u16) ||
                sizeof(T) == sizeof(u32) ||
                sizeof(T) == sizeof(u64)
            );

            if constexpr (std::is_unsigned<T>::value) {
                if constexpr (sizeof(T) == sizeof(u8)) {
                    return mask8[0];
                } else if constexpr (sizeof(T) == sizeof(u16)) {
                    return mask16[0];
                } else if constexpr (sizeof(T) == sizeof(u32)) {
                    return mask32[0];
                } else if constexpr (sizeof(T) == sizeof(u64)) {
                    return mask64[0];
                }
            } else {
                if constexpr (sizeof(T) == sizeof(u8)) {
                    return maski8[0];
                } else if constexpr (sizeof(T) == sizeof(u16)) {
                    return maski16[0];
                } else if constexpr (sizeof(T) == sizeof(u32)) {
                    return maski32[0];
                } else if constexpr (sizeof(T) == sizeof(u64)) {
                    return maski64[0];
                }
            }
        }

    public:
        CERBLIB17_CONSTEXPR explicit ByteMask(T _value) : value(_value) {}
    };

    /**
     * @brief similar to C++20 std::bit_cast, but woks since C++17
     * @tparam TO target type (must be the same size as FROM and trivially copyable)
     * @tparam FROM current type (must be the same size as TO and trivially copyable)
     * @param x value to translate
     * @return bits of x as TO type
     */
    template<typename TO, typename FROM> [[nodiscard]]
    CERBLIB17_CONSTEXPR auto bit_cast(const FROM &x) noexcept -> TO {
        static_assert(
            sizeof(TO) == sizeof(FROM) &&
            std::is_trivially_copyable<FROM>::value &&
            std::is_trivially_copyable<TO>::value
        );
    #if CERBLIB_CXX >= 2020
        return std::bit_cast<TO>(x);
    #else
        union { FROM current; TO target; } u = {x};
        return u.target;
    #endif /* C++20 */
    }

    /**
     * @brief constexpr for loop in C++
     * 
     * @tparam BEGIN    start index
     * @tparam END      end index
     * @tparam INC      incrementer for BEGIN
     * @tparam FUNCTION reference function which will be called
     * @param function  function which will be called
     */
    template<auto BEGIN, auto END, auto INC, typename FUNCTION>
    CERBLIB_COMPILE_TIME void constexpr_for(FUNCTION &&function) {
        if constexpr (BEGIN < END) {
            function(std::integral_constant<decltype(BEGIN), BEGIN>());
            constexpr_for<BEGIN + INC, END, INC>(function);
        }
    };

    /**
     * @brief short form of conditional ? _lhs : _rhs
     * 
     * @tparam T 
     * @param condition if true function returns _lhs otherwise _rhs is returned
     * @param _on_true 
     * @param _on_false 
     * @return condition ? _on_true : _on_false
     */
    template<typename T>
    constexpr auto cmov(bool condition, const T &_on_true, const T &_on_false) -> T {
        return condition ? _on_true : _on_false;
    }

    #if defined(__WINDOWS__) || defined(__WIN32__)
        constexpr auto endl = "\n\r";
    #else
        constexpr auto endl = '\n';
    #endif
}

#endif /* __cplusplus */

#endif /* cerberusTypes_h */
