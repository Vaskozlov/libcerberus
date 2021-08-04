#include <cerberus/math.hpp>
#include <cerberus/operators.hpp>

struct CERBLIB_TRIVIAL operators_test {
    int i;
    float f;

    // friend operator+(or other)(const operators_test&, const operators_test&) -> operators_test
    CERBLIB_CLASS_ARITHMETIC(
            operators_test,
            constexpr CERBLIB_INLINE,
            constexpr,
            lhs, rhs, OP,
            {
                if constexpr (OP == cerb::operators::arithmetic::modulo) {
                    return operators_test(cerb::operators::count<OP>(lhs.i, rhs.i), NAN);
                } else {
                    return operators_test(cerb::operators::count<OP>(lhs.i, rhs.i), cerb::operators::count<OP>(lhs.f, rhs.f));
                }
            }
    )

    // friend operator+(or other)(const operators_test&, const T&) -> operators_test
    CERBLIB_CLASS_ARITHMETIC_FOR_ANY_TYPE(
        operators_test, T,
        constexpr CERBLIB_INLINE,
        constexpr,
        lhs, rhs, OP,
        {
            static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value);
            if constexpr (OP == cerb::operators::arithmetic::modulo) {
                return operators_test(cerb::operators::count<OP>(lhs.i, rhs), NAN);
            } else {
                return operators_test(cerb::operators::count<OP>(lhs.i, rhs), cerb::operators::count<OP>(lhs.f, static_cast<float>(rhs)));
            }
        }
    )

    // operator+=(or other)(const operators_test&) -> operators_test& (this)
    CERBLIB_CLASS_ARITHMETIC_ON_SELF(
            operators_test,
            constexpr CERBLIB_INLINE,
            constexpr,
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

    // operator+=(or other)(const T&) -> operators_test& (this)
    CERBLIB_CLASS_ARITHMETIC_ON_SELF_FOR_ANY_TYPE(
        operators_test, T,
        constexpr CERBLIB_INLINE,
        constexpr,
        other, OP,
        {
            static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value);

            this->i = cerb::operators::count<OP>(this->i, other);

            if constexpr (OP != cerb::operators::arithmetic::modulo) {
                this->f = cerb::operators::count<OP>(this->f, other);
            } else {
                this->f = NAN;
            }

            return *this;
        }
    )

    CERBLIB_CLASS_ARITHMETIC_ON_SELF_FOR_OTHER_TYPE(
            operators_test, int,
            constexpr CERBLIB_INLINE,
            constexpr,
            other, OP,
            {
                this->i = cerb::operators::count<OP>(this->i, other);

                if constexpr (OP != cerb::operators::arithmetic::modulo) {
                    this->f = cerb::operators::count<OP>(this->f, other);
                } else {
                    this->f = NAN;
                }

                return *this;
            }
    )

    CERBLIB_CLASS_ARITHMETIC_INCREMENT(
            operators_test,
            constexpr CERBLIB_INLINE,
            {
                this->i++;
                this->f++;
            }
    )

    CERBLIB_CLASS_ARITHMETIC_DECREMENT(
            operators_test,
            constexpr CERBLIB_INLINE,
            {
                this->i--;
                this->f--;
            }
    )

    CERBLIB_CLASS_COMPARISON(
            operators_test,
            constexpr CERBLIB_INLINE,
            constexpr,
            lhs, rhs, OP,
            {
                return cerb::operators::compare<OP>(lhs.i, rhs.i) && cerb::operators::compare<OP>(lhs.f, rhs.f);
            }
    )

    CERBLIB_CLASS_COMPARISON_FOR_ANY_TYPE(
            operators_test, T,
            constexpr CERBLIB_INLINE,
            constexpr,
            lhs, rhs, OP,
            {
                static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value);
                return cerb::operators::compare<OP>(lhs.i, static_cast<int>(rhs)) && cerb::operators::compare<OP>(lhs.f, static_cast<float>(rhs));
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

struct CERBLIB_TRIVIAL operators_test_trivial {
    int i;
    float f;

    CERBLIB_INLINE friend auto operator+(const operators_test_trivial &lhs, const operators_test_trivial &rhs) -> operators_test_trivial {
        return {lhs.i + rhs.i, lhs.f + rhs.f};
    }

    CERBLIB_INLINE friend auto operator-(const operators_test_trivial &lhs, const operators_test_trivial &rhs) -> operators_test_trivial {
        return {lhs.i - rhs.i, lhs.f - rhs.f};
    }

    CERBLIB_INLINE friend auto operator*(const operators_test_trivial &lhs, const operators_test_trivial &rhs) -> operators_test_trivial {
        return {lhs.i * rhs.i, lhs.f * rhs.f};
    }

    CERBLIB_INLINE friend auto operator/(const operators_test_trivial &lhs, const operators_test_trivial &rhs) -> operators_test_trivial {
        return {lhs.i / rhs.i, lhs.f / rhs.f};
    }

    CERBLIB_INLINE friend auto operator%(const operators_test_trivial &lhs, const operators_test_trivial &rhs) -> operators_test_trivial {
        return {lhs.i % rhs.i, NAN};
    }

    CERBLIB_INLINE auto operator+=(const operators_test_trivial &other) -> operators_test_trivial& {
        this->i += other.i;
        this->f += other.f;
        return *this;
    }

    CERBLIB_INLINE auto operator-=(const operators_test_trivial &other) -> operators_test_trivial& {
        this->i -= other.i;
        this->f -= other.f;
        return *this;
    }

    CERBLIB_INLINE auto operator*=(const operators_test_trivial &other) -> operators_test_trivial& {
        this->i *= other.i;
        this->f *= other.f;
        return *this;
    }

    CERBLIB_INLINE auto operator/=(const operators_test_trivial &other) -> operators_test_trivial& {
        this->i /= other.i;
        this->f /= other.f;
        return *this;
    }

    CERBLIB_INLINE auto operator%=(const operators_test_trivial &other) -> operators_test_trivial& {
        this->i %= other.i;
        this->f = NAN;
        return *this;
    }

    CERBLIB_INLINE friend auto operator==(const operators_test_trivial &lhs, const operators_test_trivial &rhs) -> bool {
        return lhs.i == rhs.i && lhs.f == rhs.f;
    }

    CERBLIB_INLINE friend auto operator!=(const operators_test_trivial &lhs, const operators_test_trivial &rhs) -> bool {
        return lhs.i != rhs.i && lhs.f != rhs.f;
    }

    CERBLIB_INLINE friend auto operator>(const operators_test_trivial &lhs, const operators_test_trivial &rhs) -> bool {
        return lhs.i > rhs.i && lhs.f > rhs.f;
    }

    CERBLIB_INLINE friend auto operator<(const operators_test_trivial &lhs, const operators_test_trivial &rhs) -> bool {
        return lhs.i < rhs.i && lhs.f < rhs.f;
    }

    CERBLIB_INLINE friend auto operator>=(const operators_test_trivial &lhs, const operators_test_trivial &rhs) -> bool {
        return lhs.i >= rhs.i && lhs.f >= rhs.f;
    }

    CERBLIB_INLINE friend auto operator<=(const operators_test_trivial &lhs, const operators_test_trivial &rhs) -> bool {
        return lhs.i <= rhs.i && lhs.f <= rhs.f;
    }

    CERBLIB_INLINE auto operator++() -> operators_test_trivial&{
        this->i++;
        this->f++;
        return *this;
    }

    CERBLIB_INLINE auto operator++(int) -> operators_test_trivial {
        auto old = *this;
        operator++();
        return old;
    }

    CERBLIB_INLINE auto operator--() -> operators_test_trivial&{
        this->i++;
        this->f++;
        return *this;
    }

    CERBLIB_INLINE auto operator--(int) -> operators_test_trivial {
        auto old = *this;
        operator--();
        return old;
    }

    auto operator=(const operators_test_trivial&) -> operators_test_trivial& = default;
    auto operator=(operators_test_trivial&&) noexcept -> operators_test_trivial& = default;

    operators_test_trivial() = default;
    ~operators_test_trivial() = default;

    operators_test_trivial(operators_test_trivial&) = default;
    operators_test_trivial(operators_test_trivial&&) noexcept = default;

    operators_test_trivial(int in, float flo) : i(in), f(flo) {}
};
