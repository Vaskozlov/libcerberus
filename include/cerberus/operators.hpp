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
        not_equal = 0b0,
        equal = 0b1,
        greater = 0b10,
        greater_equal = 0b11,
        less = 0b100,
        less_equal = 0b101
    };

    namespace operators {

        constexpr auto comparison_contains(comparison value, comparison contains) -> bool {
            return (static_cast<u16>(value) & static_cast<u16>(contains)) != 0;
        }

        template<comparison OP, typename T1, typename T2>
        constexpr auto compare(const T1 &_lhs, const T2 _rhs) -> bool {
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
        constexpr auto count(const T1 &_lhs, const T2 &_rhs) {
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
        constexpr auto count_equal(T1 &_lhs, const T2 &_rhs) -> void {
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
}

#endif /*  CERBERUS_OPERATORS_HPP */
