#ifndef CERBERUS_OPERATORS_HPP
#define CERBERUS_OPERATORS_HPP

#include <cerberus/types.h>

namespace cerb {

    enum struct OperatorSignature : u8
    {
        LESS,
        LESS_EQUAL,
        GREATER,
        GREATER_EQUAL,
        EQUAL,
        NOT_EQUAL,
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
        DIVISION,
        MODULO
    };

    template<typename T1, typename T2 = T1>
    struct less
    {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs < rhs;
        }

        static constexpr inline OperatorSignature is_less = OperatorSignature::LESS;
    };

    template<typename T1, typename T2 = T1>
    struct less_equal
    {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs <= rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::LESS_EQUAL;
    };

    template<typename T1, typename T2 = T1>
    struct greater
    {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs > rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::GREATER;
    };

    template<typename T1, typename T2 = T1>
    struct greater_equal
    {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs >= rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::GREATER_EQUAL;
    };

    template<typename T1, typename T2 = T1>
    struct equal_to
    {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs == rhs;
        }

        static constexpr inline OperatorSignature is_less = OperatorSignature::EQUAL;
    };

    template<typename T1, typename T2 = T1>
    struct not_equal_to
    {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs == rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::NOT_EQUAL;
    };

    template<>
    struct less<void>
    {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs < rhs;
        }

        static constexpr inline OperatorSignature is_less = OperatorSignature::LESS;
    };

    template<>
    struct less_equal<void>
    {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs <= rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::LESS_EQUAL;
    };

    template<>
    struct greater<void>
    {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs > rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::GREATER;
    };

    template<>
    struct greater_equal<void>
    {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs >= rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::GREATER_EQUAL;
    };

    template<>
    struct equal_to<void>
    {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs == rhs;
        }

        static constexpr inline OperatorSignature is_less = OperatorSignature::EQUAL;
    };

    template<>
    struct not_equal_to<void>
    {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs != rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::NOT_EQUAL;
    };

    template<typename T1, typename T2 = T1>
    struct addition
    {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs + rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::ADDITION;
    };

    template<typename T1, typename T2 = T1>
    struct subtraction
    {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs - rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::SUBTRACTION;
    };

    template<typename T1, typename T2 = T1>
    struct multiplication
    {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs * rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::MULTIPLICATION;
    };

    template<typename T1, typename T2 = T1>
    struct division
    {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs / rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::DIVISION;
    };

    template<typename T1, typename T2 = T1>
    struct modulo
    {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs % rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::MODULO;
    };

    template<>
    struct addition<void>
    {
        template<typename T1, typename T2>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs + rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::ADDITION;
    };

    template<>
    struct subtraction<void>
    {
        template<typename T1, typename T2>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs - rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::SUBTRACTION;
    };

    template<>
    struct multiplication<void>
    {
        template<typename T1, typename T2>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs * rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::MULTIPLICATION;
    };

    template<>
    struct division<void>
    {
        template<typename T1, typename T2>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs / rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::DIVISION;
    };

    template<>
    struct modulo<void>
    {
        template<typename T1, typename T2>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs % rhs;
        }

        static constexpr inline OperatorSignature is_less =
            OperatorSignature::MODULO;
    };
}// namespace cerb
#endif /*  CERBERUS_OPERATORS_HPP */
