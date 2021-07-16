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
    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;
    
    using u8 = uint8_t;
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

#if defined(__cplusplus)
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

#if __cplusplus < 201703L
#  error cerberus requires at least C++17
#endif /* C++17 */

#if !defined(always_inline)
#define always_inline   __attribute__((always_inline)) inline
#endif /* always_inline */

#if defined(__cplusplus) && __cplusplus <= 201703L
// C++17 here
#  define LIKELY
#  define UNLIKELY
#elif defined(__cplusplus)
// C++20 here
#  define LIKELY    [[likely]]
#  define UNLIKELY  [[unlikely]]
#endif /* C++17-20 */

#if defined(__clang__) && !defined(TRIVIAL)
#  define TRIVIAL __attribute__((trivial_abi))
#elif !defined(TRIVIAL)
#  define TRIVIAL
#endif

#if defined(__cplusplus)
#  if __cplusplus <= 201703L
#    define cerb_three_way_comparison 0
#  elif defined(__clang__) && __clang_major__ >= 10
#    define cerb_three_way_comparison 1
#  elif defined(__GNUC__) && __GNUC__ >= 10
#    define cerb_three_way_comparison 1
#  else
#    define cerb_three_way_comparison 0
#  endif /* cerb_three_way_comparison */

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

#if defined(__cplusplus)
#  include <array>
#  include <type_traits>

namespace cerb {

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

        constexpr explicit ByteMask(T _value) : value(_value) {}
    };

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
    constexpr void constexpr_for(FUNCTION &&function) {
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
    constexpr auto cmov(bool condition, const T _on_true, const T _on_false) -> T {
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
