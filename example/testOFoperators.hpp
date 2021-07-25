#include <cerberus/math.hpp>
#include <cerberus/operators.hpp>

struct TRIVIAL operators_test {
    int i;
    float f;

    CERBLIB_ARITHMETIC_BASE(
            operators_test,
            always_inline,
            _lhs, _rhs, OP,
            {
                if constexpr (OP == cerb::operators::arithmetic::modulo) {
                    return operators_test(cerb::operators::count<OP>(_lhs.i, _rhs.i), NAN);
                } else {
                    return operators_test(cerb::operators::count<OP>(_lhs.i, _rhs.i), cerb::operators::count<OP>(_lhs.f, _rhs.f));
                }
            }
    )

    CERBLIB_ARITHMETIC_BASE_EQUAL(
            operators_test,
            always_inline,
            other, OP,
            {
                this->i = cerb::operators::count<OP>(this->i, other.i);

                if constexpr (OP != cerb::operators::arithmetic::modulo) {
                    this->f = cerb::operators::count<OP>(this->f, other.f);
                } else {
                    this->f = NAN;
                }

                return *this;
            }
    )

    CERBLIB_CREATE_COMPARISON_RULES(
            operators_test,
            always_inline,
            _lhs, _rhs, OP,
            {
                return cerb::operators::compare<OP>(_lhs.i, _rhs.i) && cerb::operators::compare<OP>(_lhs.f, _rhs.f);
            }
    )

    CERBLIB_ARITHMETIC_INCREMENT(
            operators_test,
            always_inline,
            {
                this->i++;
                this->f++;
            }
    )

    CERBLIB_ARITHMETIC_DECREMENT(
            operators_test,
            always_inline,
            {
                this->i--;
                this->f--;
            }
    )

    auto operator=(const operators_test&) -> operators_test& = default;
    auto operator=(operators_test&&) noexcept -> operators_test& = default;

    operators_test() = default;
    ~operators_test() = default;

    operators_test(operators_test&) = default;
    operators_test(operators_test&&) noexcept = default;

    operators_test(int in, float flo) : i(in), f(flo) {}
};

struct TRIVIAL operators_test_trivial {
    int i;
    float f;

    always_inline friend auto operator+(const operators_test_trivial &_lhs, const operators_test_trivial &_rhs) -> operators_test_trivial {
        return {_lhs.i + _rhs.i, _lhs.f + _rhs.f};
    }

    always_inline friend auto operator-(const operators_test_trivial &_lhs, const operators_test_trivial &_rhs) -> operators_test_trivial {
        return {_lhs.i - _rhs.i, _lhs.f - _rhs.f};
    }

    always_inline friend auto operator*(const operators_test_trivial &_lhs, const operators_test_trivial &_rhs) -> operators_test_trivial {
        return {_lhs.i * _rhs.i, _lhs.f * _rhs.f};
    }

    always_inline friend auto operator/(const operators_test_trivial &_lhs, const operators_test_trivial &_rhs) -> operators_test_trivial {
        return {_lhs.i / _rhs.i, _lhs.f / _rhs.f};
    }

    always_inline friend auto operator%(const operators_test_trivial &_lhs, const operators_test_trivial &_rhs) -> operators_test_trivial {
        return {_lhs.i % _rhs.i, NAN};
    }

    always_inline auto operator+=(const operators_test_trivial &other) -> operators_test_trivial& {
        this->i += other.i;
        this->f += other.f;
        return *this;
    }

    always_inline auto operator-=(const operators_test_trivial &other) -> operators_test_trivial& {
        this->i -= other.i;
        this->f -= other.f;
        return *this;
    }

    always_inline auto operator*=(const operators_test_trivial &other) -> operators_test_trivial& {
        this->i *= other.i;
        this->f *= other.f;
        return *this;
    }

    always_inline auto operator/=(const operators_test_trivial &other) -> operators_test_trivial& {
        this->i /= other.i;
        this->f /= other.f;
        return *this;
    }

    always_inline auto operator%=(const operators_test_trivial &other) -> operators_test_trivial& {
        this->i %= other.i;
        this->f = NAN;
        return *this;
    }

    always_inline friend auto operator==(const operators_test_trivial &_lhs, const operators_test_trivial &_rhs) -> bool {
        return _lhs.i == _rhs.i && _lhs.f == _rhs.f;
    }

    always_inline friend auto operator!=(const operators_test_trivial &_lhs, const operators_test_trivial &_rhs) -> bool {
        return _lhs.i != _rhs.i && _lhs.f != _rhs.f;
    }

    always_inline friend auto operator>(const operators_test_trivial &_lhs, const operators_test_trivial &_rhs) -> bool {
        return _lhs.i > _rhs.i && _lhs.f > _rhs.f;
    }

    always_inline friend auto operator<(const operators_test_trivial &_lhs, const operators_test_trivial &_rhs) -> bool {
        return _lhs.i < _rhs.i && _lhs.f < _rhs.f;
    }

    always_inline friend auto operator>=(const operators_test_trivial &_lhs, const operators_test_trivial &_rhs) -> bool {
        return _lhs.i >= _rhs.i && _lhs.f >= _rhs.f;
    }

    always_inline friend auto operator<=(const operators_test_trivial &_lhs, const operators_test_trivial &_rhs) -> bool {
        return _lhs.i <= _rhs.i && _lhs.f <= _rhs.f;
    }

    always_inline auto operator++() -> operators_test_trivial&{
        this->i++;
        this->f++;
        return *this;
    }

    always_inline auto operator++(int) -> operators_test_trivial& {
        auto old = this;
        operator++();
        return *this;
    }

    always_inline auto operator--() -> operators_test_trivial&{
        this->i++;
        this->f++;
        return *this;
    }

    always_inline auto operator--(int) -> operators_test_trivial& {
        auto old = this;
        operator--();
        return *this;
    }

    auto operator=(const operators_test_trivial&) -> operators_test_trivial& = default;
    auto operator=(operators_test_trivial&&) noexcept -> operators_test_trivial& = default;

    operators_test_trivial() = default;
    ~operators_test_trivial() = default;

    operators_test_trivial(operators_test_trivial&) = default;
    operators_test_trivial(operators_test_trivial&&) noexcept = default;

    operators_test_trivial(int in, float flo) : i(in), f(flo) {}
};
