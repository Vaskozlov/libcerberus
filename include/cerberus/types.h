#ifndef cerberusTypes_h
#define cerberusTypes_h

#include <stddef.h>

#if defined(__cplusplus)
#  include <cstddef>
#  include <cinttypes>
#else
#  include <inttypes.h>
#endif /* __cplusplus */

#if defined(__unix__)
#  include <alloca.h>
#else
#  include <malloc.h>

#  ifndef alloca
#    define alloca(x) _alloca(x)
#  endif /* alloca */

#endif /* __unix__ */

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

#if !defined(CERBLIB_STRX) && !defined(CERBLIB_STR)
#  define CERBLIB_STRX(x) #x
#  define CERBLIB_STR(x) CERBLIB_STRX(x)
#endif /* CERBLIB_STR and CERBLIB_STRX */

#if defined(__cplusplus) && !defined(__BEGIN_DECLS)
#  define __BEGIN_DECLS extern "C" {
#  define __END_DECLS }
#elif !defined(__BEGIN_DECLS)
#  define __BEGIN_DECLS
#  define __END_DECLS
#endif /* __BEGIN_DECLS */

#if (__cplusplus < 201703L)
#  error cerberus requires at least C++17
#endif /* C++17 */

#ifndef CERBLIB_NOT_X86_64_CONSTEXPR
#  if defined(__x86_64)
#    define CERBLIB_NOT_X86_64_CONSTEXPR CERBLIB_INLINE
#  else
#    define CERBLIB_NOT_X86_64_CONSTEXPR constexpr
#  endif
#endif

#if !defined(CERBLIB_INLINE)
#  if defined(__unix__)
#    define CERBLIB_INLINE __attribute__((always_inline)) inline
#  else
#    define CERBLIB_INLINE __forceinline
#  endif
#endif /* CERBLIB_INLINE */

#if (__cplusplus <= 201703L)
// C++17 or earlier here
#  define LIKELY
#  define UNLIKELY
#elif defined(__cplusplus)
// C++20 here
#  define LIKELY    [[likely]]
#  define UNLIKELY  [[unlikely]]
#endif /* C++17-20 */

#ifndef CERBLIB_TRIVIAL
#  if defined(__clang__)
#    define CERBLIB_TRIVIAL [[clang::trivial_abi]]
#  elif defined(__GNUC__)
#    define CERBLIB_TRIVIAL __attribute__((trivial_abi))
#  else
#    define CERBLIB_TRIVIAL
#  endif
#endif

#ifndef CERBLIB_DEPRECATED
#  if defined(__DEPRECATED)
#    define CERBLIB_DEPRECATED [[deprecated]]
#    define CERBLIB_DEPRECATED_SUGGEST(ALT) [[deprecated("use '" ALT "' instead")]]
#  else
#    define CERBLIB_DEPRECATED
#    define CERBLIB_DEPRECATED_SUGGEST(ALT)
#  endif /* __DEPRECATED */
#endif /* CERBLIB_DEPRECATED */

#ifndef CERBLIB17_DEPRECATED
#  if defined(__DEPRECATED) && (__cplusplus >= 201703L)
#    define CERBLIB17_DEPRECATED [[deprecated]]
#    define CERBLIB17_DEPRECATED_SUGGEST(ALT) [[deprecated("use '" ALT "' instead")]]
#  else
#    define CERBLIB17_DEPRECATED
#    define CERBLIB17_DEPRECATED_SUGGEST(ALT)
#  endif /* __DEPRECATED */
#endif /* CERBLIB17_DEPRECATED */

#ifndef CERBLIB20_DEPRECATED
#  if defined(__DEPRECATED) && (__cplusplus >= 202002L)
#    define CERBLIB20_DEPRECATED [[deprecated]]
#    define CERBLIB20_DEPRECATED_SUGGEST(ALT) [[deprecated("use '" ALT "' instead")]]
#  else
#    define CERBLIB20_DEPRECATED
#    define CERBLIB20_DEPRECATED_SUGGEST(ALT)
#  endif /* __DEPRECATED */
#endif /* CERBLIB20_DEPRECATED */

#ifndef CERBLIB17_CONSTEXPR
#  if __cplusplus >= 201703L
#    define CERBLIB17_CONSTEXPR constexpr
#  else
#    define CERBLIB17_CONSTEXPR
#  endif
#endif /* CERBLIB17_CONSTEXPR */

#ifndef CERBLIB20_CONSTEXPR
#  if __cplusplus >= 202002L
#    define CERBLIB20_CONSTEXPR constexpr
#  else
#    define CERBLIB20_CONSTEXPR
#  endif
#endif /* CERBLIB20_CONSTEXPR */

#ifndef CERBLIB_COMPILE_TIME
#  if __cplusplus <= 201703L
#    define CERBLIB_COMPILE_TIME constexpr
#  else
#    define CERBLIB_COMPILE_TIME consteval
#  endif /* C++17 */
#endif /* CERBLIB_COMPILE_TIME */

#if __cplusplus <= 201703L
#  define CERBLIB_THREE_WAY_COMPARISON 0
#elif __clang_major__ >= 10 || __GNUC__ >= 10
#  define CERBLIB_THREE_WAY_COMPARISON 1
#endif /* CERBLIB_THREE_WAY_COMPARISON */

#ifndef CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR
#  define CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR bool Constexpr = true, \
            typename std::enable_if<!Constexpr && x86_64, bool>::type = true
#endif /* CERBLIB_ONLY_FOR_X86_64_AND_NO_CONSTEXPR */

#ifndef CERBLIB_NOT_FOR_X86_64_RUNTIME
#  define CERBLIB_NOT_FOR_X86_64_RUNTIME bool Constexpr = true, \
            typename std::enable_if<Constexpr || !x86_64, bool>::type = true
#endif /* CERBLIB_NOT_FOR_X86_64_RUNTIME */

#ifndef CERBLIB_UNROLL
#  if defined(__clang__) || defined(__GNUC__)
#    define CERBLIB_UNROLL _Pragma(unroll)
#  else
#    define CERBLIB_UNROLL 
#  endif
#endif /* CERBLIB_UNROLL */

#ifndef CERBLIB_UNROLL_N
#  if defined(__clang__) || defined(__GNUC__)
#    define CERBLIB_UNROLL_N(N) _Pragma(unroll CERBLIB_STR(N))
#  else
#    define CERBLIB_UNROLL_N(N) 
#  endif
#endif /* CERBLIB_UNROLL */

#ifndef CERBLIB_JOIN_STR
#  define CERBLIB_JOIN_STR(x,y) CERBLIB_STR(x ## y)
#endif /* CERBLIB_JOIN_STR */

#ifndef CERBLIB_DISABLE_WARNING
#  ifdef _MSC_VER
#    define CERBLIB_DO_PRAGMA(x) __pragma (#x)
#    define CERBLIB_PRAGMA(compiler,x) CERBLIB_DO_PRAGMA(warning(x))
#  else
#    define CERBLIB_DO_PRAGMA(x) _Pragma (#x)
#    define CERBLIB_PRAGMA(compiler,x) CERBLIB_DO_PRAGMA(compiler diagnostic x)
#  endif
#  if defined(__clang__)
#    define CERBLIB_DISABLE_WARNING(gcc_unused,clang_option,msvc_unused) \
        CERBLIB_PRAGMA(clang,push) CERBLIB_PRAGMA(clang,ignored CERBLIB_JOIN_STR(-W,clang_option))
#    define CERBLIB_ENABLE_WARNING(gcc_unused,clang_option,msvc_unused) \
        CERBLIB_PRAGMA(clang,pop)
#  elif defined(_MSC_VER)
#    define CERBLIB_DISABLE_WARNING(gcc_unused,clang_unused,msvc_errorcode) \
        CERBLIB_PRAGMA(msvc,push) CERBLIB_DO_PRAGMA(warning(disable:##msvc_errorcode))
#    define CERBLIB_ENABLE_WARNING(gcc_unused,clang_unused,msvc_errorcode) \
        CERBLIB_PRAGMA(msvc,pop)
#  elif defined(__GNUC__)
#    if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
#      define CERBLIB_DISABLE_WARNING(gcc_option,clang_unused,msvc_unused) \
            CERBLIB_PRAGMA(GCC,push) CERBLIB_PRAGMA(GCC,ignored CERBLIB_JOIN_STR(-W,gcc_option))
#      define CERBLIB_ENABLE_WARNING(gcc_option,clang_unused,msvc_unused) \
            CERBLIB_PRAGMA(GCC,pop)
#    else
#      define CERBLIB_DISABLE_WARNING(gcc_option,clang_unused,msvc_unused) \
            CERBLIB_PRAGMA(GCC,ignored CERBLIB_JOIN_STR(-W,gcc_option))
#      define CERBLIB_ENABLE_WARNING(gcc_option,clang_option,msvc_unused) \
            CERBLIB_PRAGMA(GCC,warning CERBLIB_JOIN_STR(-W,gcc_option))
#    endif
#  endif
#endif /* CERBLIB_DISABLE_WARNING */

#if defined(__cplusplus)
#  include <array>
#  include <limits>
#  include <utility>
#  include <type_traits>
#  include <cerberus/private/literals.hpp>

namespace cerb {
    using namespace ::cerb::literals;

    template<typename T> [[nodiscard]]
    constexpr auto getLimits(const T &) -> std::numeric_limits<T> {
        return std::numeric_limits<T>();
    }

#if defined(__x86_64__)
    constexpr auto x86_64 = true;
#else
    constexpr auto x86_64 = false;
#endif

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
    constexpr void constexpr_for(Function &&function) {
        if constexpr (Begin < End) {
            function(std::integral_constant<decltype(Begin), Begin>());
            constexpr_for<Begin + Inc, End, Inc>(function);
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
    constexpr auto cmov(bool condition, const T &on_true, const T &on_false) -> const T& {
        return condition ? on_true : on_false;
    }

    struct EmptyType {
        u8 empty;

        EmptyType() = delete;
        ~EmptyType() = delete;
        auto operator=(EmptyType &&)      -> EmptyType& = delete;
        auto operator=(const EmptyType &) -> EmptyType& = delete;
    };

    #if defined(__WINDOWS__) || defined(__WIN32__)
        constexpr auto endl = "\n\r";
    #else
        constexpr auto endl = '\n';
    #endif
} // namespace cerb

#endif /* __cplusplus */

#endif /* cerberusTypes_h */
