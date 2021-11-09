#ifndef CERBERUS_TYPE_TRAITS_HPP
#define CERBERUS_TYPE_TRAITS_HPP

#include <type_traits>
#include <cerberus/types.h>

namespace cerb {
    template<typename... Ts>
    CERBLIB_DECL CERBLIB_INLINE auto forEach(auto &&func, Ts &&...values)
    {
        [[maybe_unused]] auto iter = { ([&](auto &iterator) -> int {
            func(iterator);
            return 0;
        })(values)... };
    }
}// namespace cerb

#endif /* CERBERUS_TYPE_TRAITS_HPP */
