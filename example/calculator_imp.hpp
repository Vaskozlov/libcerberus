#ifndef CERBERUS_CALCULATORIMPL_HPP
#define CERBERUS_CALCULATORIMPL_HPP

#include "calculator.hpp"
#include <cerberus/analyzation/parser/SLR1.hpp>

CalculatorTemplate struct CalculatorImp final : public Calculator<>
{
    CalculatorAccess;

    using parser   = cerb::analysis::parser::SLR1<CharT, TokenType, token_t, 7, 10>;
    using SLR1Item = typename parser::SLR1Item;
    constexpr static u32 SYE = std::numeric_limits<u32>::max();

    parser slr1{
        { INT, ADD, LEFT_PARENTHESIS, RIGHT_PARENTHESIS, EoF, EXPR, TERM },
        {
            { SLR1_S(5), SLR1_E, SLR1_S(7), SLR1_E, SLR1_E, SLR1_S(1), SLR1_S(6) },
            { SLR1_E, SLR1_S(3), SLR1_E, SLR1_E, SLR1_S(2), SLR1_E, SLR1_E },
            { SLR1_E, SLR1_E, SLR1_E, SLR1_E, SLR1_R(1, 2, EoF), SLR1_E, SLR1_E },
            { SLR1_S(5), SLR1_E, SLR1_S(7), SLR1_E, SLR1_E, SLR1_E, SLR1_S(4) },
            { SLR1_E, SLR1_R(3, 3, EXPR), SLR1_E, SLR1_R(3, 3, EXPR),
              SLR1_R(3, 3, EXPR), SLR1_E, SLR1_E },
            { SLR1_E, SLR1_R(4, 1, TERM), SLR1_E, SLR1_R(4, 1, TERM),
              SLR1_R(4, 1, TERM), SLR1_E, SLR1_E },
            { SLR1_E, SLR1_R(2, 1, EXPR), SLR1_E, SLR1_R(2, 1, EXPR),
              SLR1_R(2, 1, EXPR), SLR1_E, SLR1_E },
            { SLR1_S(5), SLR1_E, SLR1_S(7), SLR1_E, SLR1_E, SLR1_S(8), SLR1_S(6) },
            { SLR1_E, SLR1_S(3), SLR1_E, SLR1_S(9), SLR1_E, SLR1_E, SLR1_E },
            { SLR1_E, SLR1_R(5, 3, TERM), SLR1_E, SLR1_R(5, 3, TERM),
              SLR1_R(5, 3, TERM), SLR1_E, SLR1_E },
        }
    };

    constexpr auto yield(const token_t &token) -> bool override
    {
        /*fmt::print(
            "{:<16} {:<20} ", token.repr.to_string(),
            CalculatorItemItemsNames[token.type].to_string());

        std::cout << token.pos << std::endl;*/
        slr1.yield(token);

        return true;
    }

    constexpr auto error(const item_t &item, const string_view_t &repr)
        -> void override
    {
        cerb::analysis::basic_lexical_error(
            item, repr, "Unable to find suitable dot item for: ");
    }

    constexpr auto finish() -> void override
    {}
};

#endif /* CERBERUS_CALCULATORIMPL_HPP */
