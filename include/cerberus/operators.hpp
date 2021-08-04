#ifndef CERBERUS_OPERATORS_HPP
#define CERBERUS_OPERATORS_HPP

#include <cerberus/types.h>
#include <cerberus/private/classOperators.hpp>

namespace cerb {
    enum struct arithmetic {
        addition,
        subtraction,
        multiplication,
        division,
        modulo
    };

    enum struct comparison : u16 {
        not_equal       = 0b000,
        equal           = 0b001,
        greater         = 0b010,
        greater_equal   = 0b011,
        less            = 0b100,
        less_equal      = 0b101
    };

    namespace operators {

        [[nodiscard]] constexpr
        auto comparison_contains(comparison value, comparison contains) -> bool {
            return (static_cast<u16>(value) & static_cast<u16>(contains)) != 0;
        }

        template<comparison OP, typename T1, typename T2>
        constexpr auto compare(const T1 &lhs, const T2 rhs) -> bool {
            if constexpr (OP == comparison::greater) {
                return lhs > rhs;
            } else if constexpr (OP == comparison::greater_equal) {
                return lhs >= rhs;
            } else if constexpr (OP == comparison::less) {
                return lhs < rhs;
            } else if constexpr (OP == comparison::less_equal) {
                return lhs <= rhs;
            } else if constexpr (OP == comparison::equal) {
                return lhs == rhs;
            } else if constexpr (OP == comparison::not_equal) {
                return lhs != rhs;
            }
        }

        template<arithmetic OP, typename T1, typename T2>
        constexpr auto count(const T1 &lhs, const T2 &rhs) {
            if constexpr (OP == arithmetic::addition) {
                return lhs + rhs;
            } else if constexpr (OP == arithmetic::subtraction) {
                return lhs - rhs;
            } else if constexpr (OP == arithmetic::multiplication) {
                return lhs * rhs;
            } else if constexpr (OP == arithmetic::division) {
                return lhs / rhs;
            } else if constexpr (OP == arithmetic::modulo) {
                return lhs % rhs;
            }
        }

        template<arithmetic OP, typename T1, typename T2>
        constexpr auto count_equal(T1 &lhs, const T2 &rhs) -> void {
            if constexpr (OP == arithmetic::addition) {
                lhs += rhs;
            } else if constexpr (OP == arithmetic::subtraction) {
                lhs -= rhs;
            } else if constexpr (OP == arithmetic::multiplication) {
                lhs *= rhs;
            } else if constexpr (OP == arithmetic::division) {
                lhs /= rhs;
            } else if constexpr (OP == arithmetic::modulo) {
                lhs %= rhs;
            }
        }
    } // namespace cerb::operators
}

#endif /*  CERBERUS_OPERATORS_HPP */
