#ifndef CERBERUS_OPERATORS_HPP
#define CERBERUS_OPERATORS_HPP

#include <cerberus/types.h>

#define CERBLIB_ARITHMETIC_BASE(CLASS, ATTRIBUTES, CODE) \
    template<cerb::operators::arithmetic OP>                                                                            \
    ATTRIBUTES friend auto CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE(const CLASS &_lhs, const CLASS &_rhs) -> CLASS         \
        CODE                                                                                                            \
    always_inline friend auto operator+(const CLASS &_lhs, const CLASS &_rhs) -> CLASS {                                \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::addition>(_lhs, _rhs);                \
    }                                                                                                                   \
    always_inline friend auto operator-(const CLASS &_lhs, const CLASS &_rhs) -> CLASS {                                \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::subtraction>(_lhs, _rhs);             \
    }                                                                                                                   \
    always_inline friend auto operator*(const CLASS &_lhs, const CLASS &_rhs) -> CLASS {                                \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::multiplication>(_lhs, _rhs);          \
    }                                                                                                                   \
    always_inline friend auto operator/(const CLASS &_lhs, const CLASS &_rhs) -> CLASS {                                \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::division>(_lhs, _rhs);                \
    }                                                                                                                   \
    always_inline friend auto operator%(const CLASS &_lhs, const CLASS &_rhs) -> CLASS {                                \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::modulo>(_lhs, _rhs);                  \
    }

#define CERBLIB_ARITHMETIC_BASE_EQUAL(CLASS, ATTRIBUTES, CODE)                                                          \
    template<cerb::operators::arithmetic OP>                                                                            \
    ATTRIBUTES auto CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL(const CLASS &other) -> CLASS&                           \
        CODE                                                                                                            \
    always_inline auto operator+=(const CLASS &other) -> CLASS& {                                                       \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::addition>(other);               \
    }                                                                                                                   \
    always_inline auto operator-=(const CLASS &other) -> CLASS& {                                                       \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::subtraction>(other);            \
    }                                                                                                                   \
    always_inline auto operator*=(const CLASS &other) -> CLASS& {                                                       \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::multiplication>( other);        \
    }                                                                                                                   \
    always_inline auto operator/=(const CLASS &other) -> CLASS& {                                                       \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::division>(other);               \
    }                                                                                                                   \
    always_inline auto operator%=(const CLASS &other) -> CLASS& {                                                       \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::modulo>(other);                 \
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

#define CERBLIB_CREATE_COMPARISON_RULES(CLASS, ATTRIBUTE, CODE)                                     \
    template<cerb::operators::comparison OP>                                                        \
    ATTRIBUTE friend auto CERBLIB_COMPARISON_RULE(const CLASS &_lhs, const CLASS &_rhs)             \
        CODE                                                                                        \
                                                                                                    \
    always_inline friend auto operator>(const CLASS &_lhs, const CLASS &_rhs) -> bool {             \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater>(_lhs, _rhs);           \
    }                                                                                               \
    always_inline friend auto operator>=(const CLASS &_lhs, const CLASS &_rhs) -> bool {            \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater_equal>(_lhs, _rhs);     \
    }                                                                                               \
    always_inline friend auto operator<(const CLASS &_lhs, const CLASS &_rhs) -> bool {             \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less>(_lhs, _rhs);              \
    }                                                                                               \
    always_inline friend auto operator<=(const CLASS &_lhs, const CLASS &_rhs) -> bool {            \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less_equal>(_lhs, _rhs);        \
    }                                                                                               \
    always_inline friend auto operator==(const CLASS &_lhs, const CLASS &_rhs) -> bool {            \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::equal>(_lhs, _rhs);             \
    }                                                                                               \
    always_inline friend auto operator!=(const CLASS &_lhs, const CLASS &_rhs) -> bool {            \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::not_equal>(_lhs, _rhs);         \
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
