#ifndef CERBERUS_CLASS_OPERATORS_HPP
#define CERBERUS_CLASS_OPERATORS_HPP

#ifndef CERBERUS_OPERATORS_HPP
#  error Cerblib class operators must be included adter cerberus/operators
#endif /* CERBERUS_OPERATORS_HPP */

#define CERBLIB_CLASS_ARITHMETIC(CLASS, ATTRIBUTES, EXT_ATTRIBUTES,                                                    \
                                    left_arg_name, right_arg_name, operator_name, CODE)                                \
    template<cerb::operators::arithmetic operator_name>                                                                \
    ATTRIBUTES friend                                                                                                  \
    auto CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE(                                                                        \
                const CLASS &left_arg_name,                                                                            \
                const CLASS &right_arg_name                                                                            \
    ) -> CLASS                                                                                                         \
        CODE                                                                                                           \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator+(const CLASS &left_arg_name, const CLASS &right_arg_name) -> CLASS {                                 \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::addition>(                           \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator-(const CLASS &left_arg_name, const CLASS &right_arg_name) -> CLASS {                                 \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::subtraction>(                        \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator*(const CLASS &left_arg_name, const CLASS &right_arg_name) -> CLASS {                                 \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::multiplication>(                     \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator/(const CLASS &left_arg_name, const CLASS &right_arg_name) -> CLASS {                                 \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::division>(                           \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
   EXT_ATTRIBUTES  CERBLIB_INLINE friend                                                                                \
   auto operator%(const CLASS &left_arg_name, const CLASS &right_arg_name) -> CLASS {                                  \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::modulo>(                             \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }

#define CERBLIB_CLASS_ARITHMETIC_FOR_OTHER_TYPE(CLASS, SECOND_CLASS, ATTRIBUTES, EXT_ATTRIBUTES, left_arg_name,        \
                                            right_arg_name, operator_name, CODE)                                       \
    template<cerb::operators::arithmetic operator_name>                                                                \
    ATTRIBUTES friend                                                                                                  \
    auto CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE(                                                                        \
                const CLASS &left_arg_name,                                                                            \
                const SECOND_CLASS &right_arg_name                                                                     \
    ) -> CLASS                                                                                                         \
        CODE                                                                                                           \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator+(const CLASS &left_arg_name, const SECOND_CLASS &right_arg_name) -> CLASS {                          \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::addition>(                           \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator-(const CLASS &left_arg_name, const SECOND_CLASS &right_arg_name) -> CLASS {                          \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::subtraction>(                        \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator*(const CLASS &left_arg_name, const SECOND_CLASS &right_arg_name) -> CLASS {                          \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::multiplication>(                     \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator/(const CLASS &left_arg_name, const SECOND_CLASS &right_arg_name) -> CLASS {                          \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::division>(                           \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES  CERBLIB_INLINE friend                                                                               \
    auto operator%(const CLASS &left_arg_name, const SECOND_CLASS &right_arg_name) -> CLASS {                          \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::modulo>(                             \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator+(const SECOND_CLASS &right_arg_name, const CLASS &left_arg_name) -> CLASS {                          \
        return operator+(left_arg_name, right_arg_name);                                                               \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator-(const SECOND_CLASS &right_arg_name, const CLASS &left_arg_name) -> CLASS {                          \
        return operator-(left_arg_name, right_arg_name);                                                               \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator*(const SECOND_CLASS &right_arg_name, const CLASS &left_arg_name) -> CLASS {                          \
        return operator*(left_arg_name, right_arg_name);                                                               \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator/(const SECOND_CLASS &right_arg_name, const CLASS &left_arg_name) -> CLASS {                          \
        return operator/(left_arg_name, right_arg_name);                                                               \
    }                                                                                                                  \
   EXT_ATTRIBUTES  CERBLIB_INLINE friend                                                                                \
   auto operator%(const SECOND_CLASS &right_arg_name, const CLASS &left_arg_name) -> CLASS {                           \
        return operator+(left_arg_name, right_arg_name);                                                               \
    }

#define CERBLIB_CLASS_ARITHMETIC_FOR_ANY_TYPE(CLASS, TYPENAME, ATTRIBUTES, EXT_ATTRIBUTES, left_arg_name,              \
                                            right_arg_name, operator_name, CODE)                                       \
    template<cerb::operators::arithmetic operator_name, typename TYPENAME>                                             \
    ATTRIBUTES friend                                                                                                  \
    auto CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE(                                                                        \
                const CLASS &left_arg_name,                                                                            \
                const TYPENAME &right_arg_name                                                                         \
    ) -> CLASS                                                                                                         \
        CODE                                                                                                           \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator+(const CLASS &left_arg_name, const TYPENAME &right_arg_name) -> CLASS {                              \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::addition>(                           \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator-(const CLASS &left_arg_name, const TYPENAME &right_arg_name) -> CLASS {                              \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::subtraction>(                        \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator*(const CLASS &left_arg_name, const TYPENAME &right_arg_name) -> CLASS {                              \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::multiplication>(                     \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator/(const CLASS &left_arg_name, const TYPENAME &right_arg_name) -> CLASS {                              \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::division>(                           \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES  CERBLIB_INLINE friend                                                                               \
    auto operator%(const CLASS &left_arg_name, const TYPENAME &right_arg_name) -> CLASS {                              \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE<cerb::operators::arithmetic::modulo>(                             \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator+(const TYPENAME &right_arg_name, const CLASS &left_arg_name) -> CLASS {                              \
        return operator+(left_arg_name, right_arg_name);                                                               \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator-(const TYPENAME &right_arg_name, const CLASS &left_arg_name) -> CLASS {                              \
        return operator-(left_arg_name, right_arg_name);                                                               \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator*(const TYPENAME &right_arg_name, const CLASS &left_arg_name) -> CLASS {                              \
        return operator*(left_arg_name, right_arg_name);                                                               \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator/(const TYPENAME &right_arg_name, const CLASS &left_arg_name) -> CLASS {                              \
        return operator/(left_arg_name, right_arg_name);                                                               \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES  CERBLIB_INLINE friend                                                                               \
    auto operator%(const TYPENAME &right_arg_name, const CLASS &left_arg_name) -> CLASS {                              \
        return operator+(left_arg_name, right_arg_name);                                                               \
    }

#define CERBLIB_CLASS_ARITHMETIC_ON_SELF(CLASS, ATTRIBUTES, EXT_ATTRIBUTES,                                            \
                                                right_arg_name, operator_name, CODE)                                   \
    template<cerb::operators::arithmetic operator_name>                                                                \
    ATTRIBUTES auto CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL(const CLASS &right_arg_name) -> CLASS&                 \
        CODE                                                                                                           \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator+=(const CLASS &right_arg_name) -> CLASS& {                                                           \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::addition>(                     \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator-=(const CLASS &right_arg_name) -> CLASS& {                                                           \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::subtraction>(                  \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator*=(const CLASS &right_arg_name) -> CLASS& {                                                           \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::multiplication>(               \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator/=(const CLASS &right_arg_name) -> CLASS& {                                                           \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::division>(                     \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator%=(const CLASS &right_arg_name) -> CLASS& {                                                           \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::modulo>(                       \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }

#define CERBLIB_CLASS_ARITHMETIC_ON_SELF_FOR_OTHER_TYPE(CLASS, SECOND_CLASS, ATTRIBUTES, EXT_ATTRIBUTES,               \
                                                right_arg_name, operator_name, CODE)                                   \
    template<cerb::operators::arithmetic operator_name>                                                                \
    ATTRIBUTES auto CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL(const SECOND_CLASS &right_arg_name) -> CLASS&          \
        CODE                                                                                                           \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator+=(const SECOND_CLASS &right_arg_name) -> CLASS& {                                                    \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::addition>(                     \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator-=(const SECOND_CLASS &right_arg_name) -> CLASS& {                                                    \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::subtraction>(                  \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator*=(const SECOND_CLASS &right_arg_name) -> CLASS& {                                                    \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::multiplication>(               \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator/=(const SECOND_CLASS &right_arg_name) -> CLASS& {                                                    \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::division>(                     \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator%=(const SECOND_CLASS &right_arg_name) -> CLASS& {                                                    \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::modulo>(                       \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }

#define CERBLIB_CLASS_ARITHMETIC_ON_SELF_FOR_ANY_TYPE(CLASS, TYPENAME, ATTRIBUTES, EXT_ATTRIBUTES,                     \
                                                right_arg_name, operator_name, CODE)                                   \
    template<cerb::operators::arithmetic operator_name, typename TYPENAME>                                             \
    ATTRIBUTES auto CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL(const TYPENAME &right_arg_name) -> CLASS&              \
        CODE                                                                                                           \
    template<typename TYPENAME>                                                                                        \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator+=(const TYPENAME &right_arg_name) -> CLASS& {                                                        \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::addition>(                     \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator-=(const TYPENAME &right_arg_name) -> CLASS& {                                                        \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::subtraction>(                  \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator*=(const TYPENAME &right_arg_name) -> CLASS& {                                                        \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::multiplication>(               \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator/=(const TYPENAME &right_arg_name) -> CLASS& {                                                        \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::division>(                     \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    CERBLIB_INLINE EXT_ATTRIBUTES                                                                                       \
    auto operator%=(const TYPENAME &right_arg_name) -> CLASS& {                                                        \
        return CERBLIB_PROTOTYPE_FOR_ARITHMETIC_BASE_EQUAL<cerb::operators::arithmetic::modulo>(                       \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }

#define CERBLIB_CLASS_ARITHMETIC_INCREMENT(CLASS, ATTRIBUTE, CODE)      \
    ATTRIBUTE                                                           \
    auto operator++() -> CLASS& {                                       \
        CODE                                                            \
        return *this;                                                   \
    }                                                                   \
    ATTRIBUTE auto operator++(int) -> CLASS {                           \
        CLASS cerblib_old = *this;                                      \
        operator++();                                                   \
        return cerblib_old;                                             \
    }

#define CERBLIB_CLASS_ARITHMETIC_DECREMENT(CLASS, ATTRIBUTE, CODE)      \
    ATTRIBUTE auto operator--() -> CLASS& {                             \
        CODE                                                            \
        return *this;                                                   \
    }                                                                   \
    ATTRIBUTE auto operator--(int) -> CLASS {                           \
        CLASS cerblib_old = *this;                                      \
        operator--();                                                   \
        return cerblib_old;                                             \
    }

#define CERBLIB_CLASS_COMPARISON(CLASS, ATTRIBUTE, EXT_ATTRIBUTES,                                                     \
                                        left_arg_name, right_arg_name, operator_name, CODE)                            \
    template<cerb::operators::comparison operator_name>                                                                \
    ATTRIBUTE friend                                                                                                   \
    auto CERBLIB_COMPARISON_RULE(const CLASS &left_arg_name, const CLASS &right_arg_name)                              \
        CODE                                                                                                           \
                                                                                                                       \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator>(const CLASS &left_arg_name, const CLASS &right_arg_name) -> bool {                                  \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater>(                                          \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator>=(const CLASS &left_arg_name, const CLASS &right_arg_name) -> bool {                                 \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater_equal>(                                    \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator<(const CLASS &left_arg_name, const CLASS &right_arg_name) -> bool {                                  \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less>(                                             \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator<=(const CLASS &left_arg_name, const CLASS &right_arg_name) -> bool {                                 \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less_equal>(                                       \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator==(const CLASS &left_arg_name, const CLASS &right_arg_name) -> bool {                                 \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::equal>(                                            \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator!=(const CLASS &left_arg_name, const CLASS &right_arg_name) -> bool {                                 \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::not_equal>(                                        \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }

#define CERBLIB_CLASS_COMPARISON_FOR_OTHER_TYPE(CLASS, SECOND_TYPE, ATTRIBUTE, EXT_ATTRIBUTES,                         \
                                        left_arg_name, right_arg_name, operator_name, CODE)                            \
    template<cerb::operators::comparison operator_name>                                                                \
    ATTRIBUTE friend                                                                                                   \
    auto CERBLIB_COMPARISON_RULE(const CLASS &left_arg_name, const SECOND_TYPE &right_arg_name)                        \
        CODE                                                                                                           \
                                                                                                                       \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator>(const CLASS &left_arg_name, const SECOND_TYPE &right_arg_name) -> bool {                            \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater>(                                          \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator>=(const CLASS &left_arg_name, const SECOND_TYPE &right_arg_name) -> bool {                           \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater_equal>(                                    \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator<(const CLASS &left_arg_name, const SECOND_TYPE &right_arg_name) -> bool {                            \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less>(                                             \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator<=(const CLASS &left_arg_name, const SECOND_TYPE &right_arg_name) -> bool {                           \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less_equal>(                                       \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator==(const CLASS &left_arg_name, const SECOND_TYPE &right_arg_name) -> bool {                           \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::equal>(                                            \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator!=(const CLASS &left_arg_name, const SECOND_TYPE &right_arg_name) -> bool {                           \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::not_equal>(                                        \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator>(const SECOND_TYPE &left_arg_name, const CLASS &right_arg_name) -> bool {                            \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater>(                                          \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator>=(const SECOND_TYPE &left_arg_name, const CLASS &right_arg_name) -> bool {                           \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater_equal>(                                    \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator<(const SECOND_TYPE &left_arg_name, const CLASS &right_arg_name) -> bool {                            \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less>(                                             \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator<=(const SECOND_TYPE &left_arg_name, const CLASS &right_arg_name) -> bool {                           \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less_equal>(                                       \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator==(const SECOND_TYPE &left_arg_name, const CLASS &right_arg_name) -> bool {                           \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::equal>(                                            \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }                                                                                                                  \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator!=(const SECOND_TYPE &left_arg_name, const CLASS &right_arg_name) -> bool {                           \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::not_equal>(                                        \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }

#define CERBLIB_CLASS_COMPARISON_FOR_ANY_TYPE(CLASS, TYPENAME, ATTRIBUTE, EXT_ATTRIBUTES,                              \
                                        left_arg_name, right_arg_name, operator_name, CODE)                            \
    template<cerb::operators::comparison operator_name, typename TYPENAME>                                             \
    ATTRIBUTE friend                                                                                                   \
    auto CERBLIB_COMPARISON_RULE(const CLASS &left_arg_name, const TYPENAME &right_arg_name)                           \
        CODE                                                                                                           \
                                                                                                                       \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator>(const CLASS &left_arg_name, const TYPENAME &right_arg_name) -> bool {                               \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater>(                                          \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator>=(const CLASS &left_arg_name, const TYPENAME &right_arg_name) -> bool {                              \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater_equal>(                                    \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator<(const CLASS &left_arg_name, const TYPENAME &right_arg_name) -> bool {                               \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less>(                                             \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator<=(const CLASS &left_arg_name, const TYPENAME &right_arg_name) -> bool {                              \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less_equal>(                                       \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator==(const CLASS &left_arg_name, const TYPENAME &right_arg_name) -> bool {                              \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::equal>(                                            \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator!=(const CLASS &left_arg_name, const TYPENAME &right_arg_name) -> bool {                              \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::not_equal>(                                        \
                left_arg_name,                                                                                         \
                right_arg_name                                                                                         \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator>(const TYPENAME &left_arg_name, const CLASS &right_arg_name) -> bool {                               \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater>(                                          \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator>=(const TYPENAME &left_arg_name, const CLASS &right_arg_name) -> bool {                              \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::greater_equal>(                                    \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator<(const TYPENAME &left_arg_name, const CLASS &right_arg_name) -> bool {                               \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less>(                                             \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator<=(const TYPENAME &left_arg_name, const CLASS &right_arg_name) -> bool {                              \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::less_equal>(                                       \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator==(const TYPENAME &left_arg_name, const CLASS &right_arg_name) -> bool {                              \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::equal>(                                            \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }                                                                                                                  \
    template<typename TYPENAME>                                                                                        \
    EXT_ATTRIBUTES CERBLIB_INLINE friend                                                                                \
    auto operator!=(const TYPENAME &left_arg_name, const CLASS &right_arg_name) -> bool {                              \
        return CERBLIB_COMPARISON_RULE<cerb::operators::comparison::not_equal>(                                        \
                right_arg_name,                                                                                        \
                left_arg_name                                                                                          \
        );                                                                                                             \
    }

#endif /* CERBERUS_CLASS_OPERATORS_HPP */
