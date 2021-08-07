#ifndef CERBERUS_OPERATORS_HPP
#define CERBERUS_OPERATORS_HPP

#include <cerberus/types.h>
#include <cerberus/private/classOperators.hpp>

namespace cerb {
    enum struct Arithmetic {
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
        DIVISION,
        MODULO
    };

    enum struct ComparisonRules : u16 {
        NOT_EQUAL       = 0b000,
        EQUAL           = 0b001,
        GREATER         = 0b010,
        GREATER_EQUAL   = 0b011,
        LESS            = 0b100,
        LESS_EQUAL      = 0b101
    };

    namespace operators {

        [[nodiscard]] constexpr
        auto comparisonContains(ComparisonRules value, ComparisonRules contains) -> bool {
            return (static_cast<u16>(value) & static_cast<u16>(contains)) != 0;
        }

        template<ComparisonRules OP, typename T1, typename T2>
        constexpr auto compare(const T1 &lhs, const T2 rhs) -> bool {
            if constexpr (OP == ComparisonRules::GREATER) {
                return lhs > rhs;
            }
            else if constexpr (OP == ComparisonRules::GREATER_EQUAL) {
                return lhs >= rhs;
            }
            else if constexpr (OP == ComparisonRules::LESS) {
                return lhs < rhs;
            }
            else if constexpr (OP == ComparisonRules::LESS_EQUAL) {
                return lhs <= rhs;
            }
            else if constexpr (OP == ComparisonRules::EQUAL) {
                return lhs == rhs;
            }
            else if constexpr (OP == ComparisonRules::NOT_EQUAL) {
                return lhs != rhs;
            }
        }

        template<Arithmetic OP, typename T1, typename T2>
        constexpr auto count(const T1 &lhs, const T2 &rhs) {
            if constexpr (OP == Arithmetic::ADDITION) {
                return lhs + rhs;
            }
            else if constexpr (OP == Arithmetic::SUBTRACTION) {
                return lhs - rhs;
            }
            else if constexpr (OP == Arithmetic::MULTIPLICATION) {
                return lhs * rhs;
            }
            else if constexpr (OP == Arithmetic::DIVISION) {
                return lhs / rhs;
            }
            else if constexpr (OP == Arithmetic::MODULO) {
                return lhs % rhs;
            }
        }

        template<Arithmetic OP, typename T1, typename T2>
        constexpr auto countAndEquate(T1 &lhs, const T2 &rhs) -> void {
            if constexpr (OP == Arithmetic::ADDITION) {
                lhs += rhs;
            }
            else if constexpr (OP == Arithmetic::SUBTRACTION) {
                lhs -= rhs;
            }
            else if constexpr (OP == Arithmetic::MULTIPLICATION) {
                lhs *= rhs;
            }
            else if constexpr (OP == Arithmetic::DIVISION) {
                lhs /= rhs;
            }
            else if constexpr (OP == Arithmetic::MODULO) {
                lhs %= rhs;
            }
        }
    } // namespace cerb::operators
}
#endif /*  CERBERUS_OPERATORS_HPP */
