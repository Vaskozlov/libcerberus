#ifndef CERBERUS_TYPE_TRAITS_HPP
#define CERBERUS_TYPE_TRAITS_HPP

#include <type_traits>
#include <cerberus/types.h>

namespace cerb {
    template<typename T>
    class is_iterative
    {
        using yes = std::true_type;
        using no  = std::false_type;

        template<typename U>
        static constexpr auto test(typename U::iterator * /*unused*/) -> bool
        {
            return true;
        }

        template<typename... Us>
        static constexpr auto test(Us... /*unused*/) -> bool
        {
            return false;
        }

    public:
        static constexpr bool value = test<T>(nullptr);
    };

    template<typename T>
    inline constexpr auto is_iterative_v = is_iterative<T>::value;

    template<typename F, typename... Ts>
    [[nodiscard]] constexpr CERBLIB_INLINE auto forEach(F &&func, Ts &&...values)
    {
        auto _ = { ([&](auto &i) -> int {
            func(i);
            return 0;
        })(values)... };
    }
}// namespace cerb

#endif /* CERBERUS_TYPE_TRAITS_HPP */
