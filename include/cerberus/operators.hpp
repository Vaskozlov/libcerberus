#ifndef CERBERUS_OPERATORS_HPP
#define CERBERUS_OPERATORS_HPP

#include <cerberus/types.h>
#include <cerberus/private/classOperators.hpp>

namespace cerb {
    template<typename T1, typename T2 = T1>
    struct less {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs < rhs;
        }

        static constexpr inline bool is_less = true;
    };

    template<typename T1, typename T2 = T1>
    struct less_equal {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs <= rhs;
        }
    };

    template<typename T1, typename T2 = T1>
    struct greater {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs > rhs;
        }
        static constexpr inline bool is_less = false;
    };

    template<typename T1, typename T2 = T1>
    struct greater_equal {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs >= rhs;
        }
    };

    template<typename T1, typename T2 = T1>
    struct equal_to {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs == rhs;
        }
    };

    template<typename T1, typename T2 = T1>
    struct not_equal_to {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs == rhs;
        }
    };

    template<>
    struct less<void> {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs < rhs;
        }
        static constexpr inline bool is_less = true;
    };

    template<>
    struct less_equal<void> {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs <= rhs;
        }
    };

    template<>
    struct greater<void> {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs > rhs;
        }
        static constexpr inline bool is_less = false;
    };

    template<>
    struct greater_equal<void> {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs >= rhs;
        }
    };

    template<>
    struct equal_to<void> {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs == rhs;
        }
    };

    template<>
    struct not_equal_to<void> {
        template<typename T1, typename T2 = T1>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs != rhs;
        }
    };

    template<typename T1, typename T2 = T1>
    struct addition {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs + rhs;
        }
    };

    template<typename T1, typename T2 = T1>
    struct subtraction {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs - rhs;
        }
    };

    template<typename T1, typename T2 = T1>
    struct multiplication {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs * rhs;
        }
    };

    template<typename T1, typename T2 = T1>
    struct division {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs / rhs;
        }
    };

    template<typename T1, typename T2 = T1>
    struct modulo {
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs % rhs;
        }
    };

    template<>
    struct addition<void> {
        template<typename T1, typename T2>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs + rhs;
        }
    };

    template<>
    struct subtraction<void> {
        template<typename T1, typename T2>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs - rhs;
        }
    };

    template<>
    struct multiplication<void> {
        template<typename T1, typename T2>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs * rhs;
        }
    };

    template<>
    struct division<void> {
        template<typename T1, typename T2>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs / rhs;
        }
    };

    template<>
    struct modulo<void> {
        template<typename T1, typename T2>
        constexpr auto operator()(const T1 &lhs, const T2 &rhs) const noexcept {
            return lhs % rhs;
        }
    };
}
#endif /*  CERBERUS_OPERATORS_HPP */
