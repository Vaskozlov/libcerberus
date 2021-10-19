#ifndef CERBERUS_TYPES_H
#define CERBERUS_TYPES_H

#if defined(__cplusplus)
#    include <cinttypes>
#    include <cstddef>
#else
#    include <inttypes.h>
#    include <stddef.h>
#endif /* __cplusplus */

#if defined(__unix__) || defined(__APPLE__)
#    include <alloca.h>
#else
#    include <malloc.h>
#    ifndef alloca
#        define alloca(x) _alloca(x)
#    endif /* alloca */
#endif     /* __unix__ */

#if defined(__cplusplus)
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
#else
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#endif /* __cplusplus */

#if defined(__cplusplus)
namespace cerb {
    using byte = u8;
}// namespace cerb
#else
typedef u8 byte;
#endif /* __cplusplus */

#ifndef bitsizeof
#    define bitsizeof(x) (sizeof(x) * 8)
#endif /* bitsizeof */

#ifndef CERBLIB_BYTE_SIZE
#    define CERBLIB_BYTE_SIZE 8
#endif /* CERBLIB_BYTE_SIZE */

#define CERBLIB_STRX(x) #x
#define CERBLIB_STR(x) CERBLIB_STRX(x)

#if defined(__cplusplus)
#    define CERBLIB_BEGIN_DECLS extern "C" {
#    define CERBLIB_END_DECLS }
#elif
#    define CERBLIB_BEGIN_DECLS
#    define CERBLIB_END_DECLS
#endif /* __BEGIN_DECLS */

#if (__cplusplus < 202002L)
#    error cerberus requires at least C++20
#endif /* C++20 */

#if defined(__unix__) || defined(__APPLE__)
#    define CERBLIB_INLINE __attribute__((always_inline)) inline
#else
#    define CERBLIB_INLINE __forceinline
#endif

#ifndef CERBLIB_DECL
#    define CERBLIB_DECL [[nodiscard]] constexpr
#endif /* CERBLIB_DECL */

#ifndef LIKELY
#    define LIKELY [[likely]]
#endif /* LIKELY */

#ifndef UNLIKELY
#    define UNLIKELY [[unlikely]]
#endif /* UNLIKELY */

#ifndef CERBLIB_TRIVIAL
#    if defined(__clang__)
#        define CERBLIB_TRIVIAL [[clang::trivial_abi]]
#    elif defined(__GNUC__)
#        define CERBLIB_TRIVIAL
#    else
#        define CERBLIB_TRIVIAL
#    endif /* COMPILER */
#endif     /* CERBLIB_TRIVIAL */

#ifndef CERBLIB_DEPRECATED
#    if defined(__DEPRECATED)
#        define CERBLIB_DEPRECATED [[deprecated]]
#        define CERBLIB_DEPRECATED_SUGGEST(ALT)                                     \
            [[deprecated("use '" ALT "' instead")]]
#    else
#        define CERBLIB_DEPRECATED
#        define CERBLIB_DEPRECATED_SUGGEST(ALT)
#    endif /* __DEPRECATED */
#endif     /* CERBLIB_DEPRECATED */

#ifndef CERBLIB20_DEPRECATED
#    if defined(__DEPRECATED) && (__cplusplus >= 202002L)
#        define CERBLIB20_DEPRECATED [[deprecated]]
#        define CERBLIB20_DEPRECATED_SUGGEST(ALT)                                   \
            [[deprecated("use '" ALT "' instead")]]
#    else
#        define CERBLIB20_DEPRECATED
#        define CERBLIB20_DEPRECATED_SUGGEST(ALT)
#    endif /* __DEPRECATED */
#endif     /* CERBLIB20_DEPRECATED */

#ifndef CERBLIB20_CONSTEXPR
#    if __cplusplus >= 202002L
#        define CERBLIB20_CONSTEXPR constexpr
#    else
#        define CERBLIB20_CONSTEXPR
#    endif
#endif /* CERBLIB20_CONSTEXPR */

#ifndef CERBLIB_THREE_WAY_COMPARISON
#    if __clang_major__ >= 10 || __GNUC__ >= 10
#        define CERBLIB_THREE_WAY_COMPARISON 1
#    endif
#endif /* CERBLIB_THREE_WAY_COMPARISON */

#ifndef CERBLIB_UNROLL
#    if defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER)
#        define CERBLIB_UNROLL _Pragma(CERBLIB_STR(unroll))
#    else
#        define CERBLIB_UNROLL
#    endif
#endif /* CERBLIB_UNROLL */

#ifndef CERBLIB_UNROLL_N
#    if defined(__clang__)
#        define CERBLIB_UNROLL_N(N) _Pragma(CERBLIB_STR(unroll N))
#    else
#        define CERBLIB_UNROLL_N(N)
#    endif
#endif /* CERBLIB_UNROLL */

#ifndef CERBLIB_JOIN_STR
#    define CERBLIB_JOIN_STR(x, y) CERBLIB_STR(x##y)
#endif /* CERBLIB_JOIN_STR */

#ifndef CERBLIB_DISABLE_WARNING
#    ifdef _MSC_VER
#        define CERBLIB_DO_PRAGMA(x) __pragma(#        x)
#        define CERBLIB_PRAGMA(compiler, x) CERBLIB_DO_PRAGMA(warning(x))
#    else
#        define CERBLIB_DO_PRAGMA(x) _Pragma(#        x)
#        define CERBLIB_PRAGMA(compiler, x) CERBLIB_DO_PRAGMA(compiler diagnostic x)
#    endif
#    if defined(__clang__)
#        define CERBLIB_DISABLE_WARNING(gcc_unused, clang_option, msvc_unused)      \
            CERBLIB_PRAGMA(clang, push)                                             \
            CERBLIB_PRAGMA(clang, ignored clang_option)
#        define CERBLIB_ENABLE_WARNING(gcc_unused, clang_option, msvc_unused)       \
            CERBLIB_PRAGMA(clang, pop)
#    elif defined(_MSC_VER)
#        define CERBLIB_DISABLE_WARNING(gcc_unused, clang_unused, msvc_errorcode)   \
            CERBLIB_PRAGMA(msvc, push)                                              \
            CERBLIB_DO_PRAGMA(warning(disable :##msvc_errorcode))
#        define CERBLIB_ENABLE_WARNING(gcc_unused, clang_unused, msvc_errorcode)    \
            CERBLIB_PRAGMA(msvc, pop)
#    elif defined(__GNUC__)
#        if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
#            define CERBLIB_DISABLE_WARNING(gcc_option, clang_unused, msvc_unused)  \
                CERBLIB_PRAGMA(GCC, push)                                           \
                CERBLIB_PRAGMA(GCC, ignored gcc_option)
#            define CERBLIB_ENABLE_WARNING(gcc_option, clang_unused, msvc_unused)   \
                CERBLIB_PRAGMA(GCC, pop)
#        else
#            define CERBLIB_DISABLE_WARNING(gcc_option, clang_unused, msvc_unused)  \
                CERBLIB_PRAGMA(GCC, ignored gcc_option)
#            define CERBLIB_ENABLE_WARNING(gcc_option, clang_option, msvc_unused)   \
                CERBLIB_PRAGMA(GCC, warning gcc_option)
#        endif
#    endif
#endif /* CERBLIB_DISABLE_WARNING */

#ifndef CERBLIB_CLANG_DISABLE_WARNING
#    if defined(__clang__)
#        define CERBLIB_CLANG_DISABLE_WARNING(clang_option)                         \
            CERBLIB_PRAGMA(clang, push)                                             \
            CERBLIB_PRAGMA(clang, ignored clang_option)
#        define CERBLIB_CLANG_ENABLE_WARNING CERBLIB_PRAGMA(clang, pop)
#    else
#        define CERBLIB_CLANG_DISABLE_WARNING(clang_option)
#        define CERBLIB_CLANG_ENABLE_WARNING
#    endif
#endif /* CERBLIB_CLANG_DISABLE_WARNING */

#if defined(__cplusplus)
#    include <array>
#    include <limits>
#    include <type_traits>
#    include <utility>

namespace cerb {

    struct EmptyType
    {
        u8 empty;

        EmptyType()  = delete;
        ~EmptyType() = delete;

        EmptyType(const EmptyType &) = delete;
        EmptyType(EmptyType &&)      = delete;

        auto operator=(EmptyType &&) -> EmptyType & = delete;
        auto operator=(const EmptyType &) -> EmptyType & = delete;
    };

    template<typename T>
    CERBLIB_DECL auto getLimits(const T & /*unused*/) -> std::numeric_limits<T>
    {
        return std::numeric_limits<T>();
    }

#    if defined(__x86_64__)
    constexpr auto x86_64 = true;
#    else
    constexpr auto x86_64 = false;
#    endif

#    if defined(__WINDOWS__) || defined(__WIN32__)
    constexpr auto endl = "\n\r";
#    else
    constexpr auto endl   = '\n';
#    endif

    /**
     * @brief constexpr for loop in C++
     *
     * @tparam BEGIN    start index
     * @tparam END      end index
     * @tparam INC      incrementer for BEGIN
     * @tparam FUNCTION reference function which will be called
     * @param function  function which will be called
     */
    template<auto Begin, auto End, auto Inc, typename Function>
    constexpr auto constexprFor(Function &&function) -> void
    {
        if constexpr (Begin < End) {
            function(std::integral_constant<decltype(Begin), Begin>());
            constexprFor<Begin + Inc, End, Inc>(function);
        }
    }

    /**
     * @brief short form of conditional ? lhs : rhs
     *
     * @tparam T
     * @param condition if true function returns lhs otherwise rhs is returned
     * @param on_true
     * @param _on_false
     * @return condition ?_on_true :_on_false
     */
    template<typename T>
    constexpr auto cmov(bool condition, const T &on_true, const auto &on_false)
        -> const T &
    {
        return condition ? on_true : static_cast<T>(on_false);
    }
}// namespace cerb

#endif /* __cplusplus */
#endif /* CERBERUS_TYPES_H */
