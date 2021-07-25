#ifndef CERBERUS_OPERATORS_HPP
#define CERBERUS_OPERATORS_HPP

#include <cerberus/types.h>

#define CERBLIB_ARITHMETIC_BASE(CLASS, ATTRIBUTES, left_arg_name, right_arg_name, operator_name, CODE)                 \
    template<cerb::operators::arithmetic operator_name>                                                                \
    ATTRIBUTES friend                                                                                                  \
    auto CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE(                                                                        \
                const CLASS &left_arg_name,                                                                            \
                const CLASS &right_arg_name                                                                            \
    ) -> CLASS                                                                                                         \
        CODE                                                                                                           \
    always_inline friend                                                                                               \
    auto operator+(const CLASS &left_arg_name, const CLASS &right_arg_name) -> CLASS {                                 \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::addition>(                           \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline friend                                                                                               \
    auto operator-(const CLASS &left_arg_name, const CLASS &right_arg_name) -> CLASS {                                 \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::subtraction>(                        \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline friend                                                                                               \
    auto operator*(const CLASS &left_arg_name, const CLASS &right_arg_name) -> CLASS {                                 \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::multiplication>(                     \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline friend auto operator/(const CLASS &left_arg_name, const CLASS &right_arg_name) -> CLASS {            \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::division>(                           \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline friend auto operator%(const CLASS &left_arg_name, const CLASS &right_arg_name) -> CLASS {            \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::modulo>(                             \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }

#define CERBLIB_ARITHMETIC_BASE_EQUAL(CLASS, ATTRIBUTES, right_arg_name, operator_name, CODE)                          \
    template<cerb::operators::arithmetic operator_name>                                                                \
    ATTRIBUTES auto CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL(const CLASS &right_arg_name) -> CLASS&                 \
        CODE                                                                                                           \
    always_inline auto operator+=(const CLASS &right_arg_name) -> CLASS& {                                             \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::addition>(                     \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline auto operator-=(const CLASS &right_arg_name) -> CLASS& {                                             \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::subtraction>(                  \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline auto operator*=(const CLASS &right_arg_name) -> CLASS& {                                             \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::multiplication>(               \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline auto operator/=(const CLASS &right_arg_name) -> CLASS& {                                             \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::division>(                     \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline auto operator%=(const CLASS &right_arg_name) -> CLASS& {                                             \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::modulo>(                       \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }

#define CERBLIB_ARITHMETIC_INCREMENT(CLASS, ATTRIBUTE, CODE)    \
    ATTRIBUTE auto operator++() -> CLASS& {                     \
        CODE                                                    \
        return *this;                                           \
    }                                                           \
    ATTRIBUTE auto operator++(int) -> CLASS {                   \
        CLASS cerblib_old = *this;                              \
        CODE                                                    \
        return cerblib_old;                                     \
    }

#define CERBLIB_ARITHMETIC_DECREMENT(CLASS, ATTRIBUTE, CODE)    \
    ATTRIBUTE auto operator--() -> CLASS& {                     \
        CODE                                                    \
        return *this;                                           \
    }                                                           \
    ATTRIBUTE auto operator--(int) -> CLASS {                   \
        CLASS cerblib_old = *this;                              \
        CODE                                                    \
        return cerblib_old;                                     \
    }

#define CERBLIB_CREATE_COMPARISON_RULES(CLASS, ATTRIBUTE, left_arg_name, right_arg_name, operator_name, CODE)          \
    template<cerb::operators::comparison operator_name>                                                                \
    ATTRIBUTE friend                                                                                                   \
    auto CERBLIB_COMPARISON_RULE(const CLASS &left_arg_name, const CLASS &right_arg_name)                              \
        CODE                                                                                                           \
                                                                                                                       \
    always_inline friend auto operator>(const CLASS &left_arg_name, const CLASS &_rhs) -> bool {                       \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater>(                                          \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline friend auto operator>=(const CLASS &left_arg_name, const CLASS &_rhs) -> bool {                      \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater_equal>(                                    \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline friend auto operator<(const CLASS &left_arg_name, const CLASS &_rhs) -> bool {                       \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less>(                                             \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline friend auto operator<=(const CLASS &left_arg_name, const CLASS &_rhs) -> bool {                      \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less_equal>(                                       \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline friend auto operator==(const CLASS &left_arg_name, const CLASS &_rhs) -> bool {                      \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::equal>(                                            \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    always_inline friend auto operator!=(const CLASS &left_arg_name, const CLASS &_rhs) -> bool {                      \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::not_equal>(                                        \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }

namespace cerb::operators {

    enum struct arithmetic {
        addition,
        subtraction,
        multiplication,
        division,
        modulo
    };

    enum struct comparison {
        greater,
        greater_equal,
        less,
        less_equal,
        equal,
        not_equal
    };

    template<comparison OP, typename T1, typename T2>
    CERBLIB20_CONSTEXPR auto compare(const T1 &_lhs, const T2 _rhs) -> bool {
        if constexpr (OP == comparison::greater) {
            return _lhs > _rhs;
        } else if constexpr (OP == comparison::greater_equal) {
            return _lhs >= _rhs;
        } else if constexpr (OP == comparison::less) {
            return _lhs < _rhs;
        } else if constexpr (OP == comparison::less_equal) {
            return _lhs <= _rhs;
        } else if constexpr (OP == comparison::equal) {
            return _lhs == _rhs;
        } else if constexpr (OP == comparison::not_equal) {
            return _lhs != _rhs;
        }
    }

    template<arithmetic OP, typename T1, typename T2>
    CERBLIB20_CONSTEXPR auto count(const T1 &_lhs, const T2 &_rhs) {
        if constexpr (OP == arithmetic::addition) {
            return _lhs + _rhs;
        } else if constexpr (OP == arithmetic::subtraction) {
            return _lhs - _rhs;
        } else if constexpr (OP == arithmetic::multiplication) {
            return _lhs * _rhs;
        } else if constexpr (OP == arithmetic::division) {
            return _lhs / _rhs;
        } else if constexpr (OP == arithmetic::modulo) {
            return _lhs % _rhs;
        }
    }

    template<arithmetic OP, typename T1, typename T2>
    CERBLIB20_CONSTEXPR auto count_equal(T1 &_lhs, const T2 &_rhs) -> void {
        if constexpr (OP == arithmetic::addition) {
            _lhs += _rhs;
        } else if constexpr (OP == arithmetic::subtraction) {
            _lhs -= _rhs;
        } else if constexpr (OP == arithmetic::multiplication) {
            _lhs *= _rhs;
        } else if constexpr (OP == arithmetic::division) {
            _lhs /= _rhs;
        } else if constexpr (OP == arithmetic::modulo) {
            _lhs %= _rhs;
        }
    }
} // namespace cerb::operators

#endif /*  CERBERUS_OPERATORS_HPP */
