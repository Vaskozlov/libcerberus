#ifndef CERBERUS_CALCULATORIMPL_HPP
#define CERBERUS_CALCULATORIMPL_HPP

#include "calculator.hpp"

CalculatorTemplate struct CalculatorImp final : public Calculator<>
{
    CalculatorAccess;

    u32 current_item{};
    cerb::Vector<cerb::Pair<u32, TokenType>> lr_stack{ { { 0, EMPTY } } };
    static constexpr u32 SyntaxError = std::numeric_limits<u32>::max();

    struct LR0_Action
    {
        bool reduce{ false };
        u32 state{};
        TokenType reduced_token{};

        constexpr LR0_Action() = default;
        constexpr LR0_Action(bool reduce_, u32 state_, TokenType reduced_token_)
          : reduce(reduce_), state(state_), reduced_token(reduced_token_)
        {}
    };

    static constexpr std::array<LR0_Action, 10> TableActions{
        LR0_Action{},
        LR0_Action{},
        LR0_Action{ true, 0, EoF },
        LR0_Action{},
        LR0_Action{ true, 0, EXPR },
        LR0_Action{ true, 6, TERM },
        LR0_Action{ true, 1, EXPR },
        LR0_Action{},
        LR0_Action{},
        LR0_Action{ true, 6, TERM }
    };

    constexpr static cerb::gl::Map<unsigned, cerb::gl::Map<TokenType, u32, 7>, 10>
        Table{ { 0,
                 { { EXPR, 1 },
                   { TERM, 6 },
                   { INT, 5 },
                   { ADD, SyntaxError },
                   { LEFT_PARENTHESIS, 7 },
                   { RIGHT_PARENTHESIS, SyntaxError },
                   { EoF, SyntaxError } } },
               { 1,
                 { { EXPR, SyntaxError },
                   { TERM, SyntaxError },
                   { INT, SyntaxError },
                   { ADD, 3 },
                   { LEFT_PARENTHESIS, SyntaxError },
                   { RIGHT_PARENTHESIS, SyntaxError },
                   { EoF, 2 } } },
               { 2,
                 { { EXPR, SyntaxError },
                   { TERM, SyntaxError },
                   { INT, SyntaxError },
                   { ADD, SyntaxError },
                   { LEFT_PARENTHESIS, SyntaxError },
                   { RIGHT_PARENTHESIS, SyntaxError },
                   { EoF, SyntaxError } } },
               { 3,
                 { { EXPR, SyntaxError },
                   { TERM, 4 },
                   { INT, 5 },
                   { ADD, SyntaxError },
                   { LEFT_PARENTHESIS, 7 },
                   { RIGHT_PARENTHESIS, SyntaxError },
                   { EoF, SyntaxError } } },
               { 4,
                 { { EXPR, SyntaxError },
                   { TERM, SyntaxError },
                   { INT, SyntaxError },
                   { ADD, SyntaxError },
                   { LEFT_PARENTHESIS, SyntaxError },
                   { RIGHT_PARENTHESIS, SyntaxError },
                   { EoF, SyntaxError } } },
               { 5,
                 { { EXPR, SyntaxError },
                   { TERM, SyntaxError },
                   { INT, SyntaxError },
                   { ADD, SyntaxError },
                   { LEFT_PARENTHESIS, SyntaxError },
                   { RIGHT_PARENTHESIS, SyntaxError },
                   { EoF, SyntaxError } } },
               { 6,
                 { { EXPR, SyntaxError },
                   { TERM, SyntaxError },
                   { INT, SyntaxError },
                   { ADD, SyntaxError },
                   { LEFT_PARENTHESIS, SyntaxError },
                   { RIGHT_PARENTHESIS, SyntaxError },
                   { EoF, SyntaxError } } },
               { 7,
                 { { EXPR, 8 },
                   { TERM, 6 },
                   { INT, 5 },
                   { ADD, SyntaxError },
                   { LEFT_PARENTHESIS, 7 },
                   { RIGHT_PARENTHESIS, SyntaxError },
                   { EoF, SyntaxError } } },
               { 8,
                 { { EXPR, SyntaxError },
                   { TERM, SyntaxError },
                   { INT, SyntaxError },
                   { ADD, 3 },
                   { LEFT_PARENTHESIS, SyntaxError },
                   { RIGHT_PARENTHESIS, 9 },
                   { EoF, SyntaxError } } },
               { 9,
                 { { EXPR, SyntaxError },
                   { TERM, SyntaxError },
                   { INT, SyntaxError },
                   { ADD, SyntaxError },
                   { LEFT_PARENTHESIS, SyntaxError },
                   { RIGHT_PARENTHESIS, SyntaxError },
                   { EoF, SyntaxError } } } };

    constexpr auto yield(const token_t &token) -> bool override
    {
        fmt::print(
            "{:<16} {:<20} ", token.repr.to_string(),
            CalculatorItemItemsNames[token.type].to_string());

        std::cout << token.pos << std::endl;

        if (!TableActions[current_item].reduce) {
            lr_stack.back().second = token.type;
            current_item           = Table[current_item][token.type];
            lr_stack.emplace_back(current_item);
        }

        fmt::print("Stack: \n");
        CERBLIB_UNROLL_N(2)
        for (const auto &elem : lr_stack) {
            if (CalculatorItemItemsNames.contains(elem.second)) {
                fmt::print(
                    "{{ {}: {} }}, ", elem.first,
                    CalculatorItemItemsNames[elem.second].to_string());
            }
        }
        std::cout << std::endl;

        CERBLIB_UNROLL_N(1)
        while (TableActions[current_item].reduce) {
            lr_stack[lr_stack.size() - 2].second =
                TableActions[current_item].reduced_token;
            current_item = TableActions[current_item].state;

            fmt::print("Stack: \n");
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : lr_stack) {
                if (CalculatorItemItemsNames.contains(elem.second)) {
                    fmt::print(
                        "{{ {}: {} }}, ", elem.first,
                        CalculatorItemItemsNames[elem.second].to_string());
                }
            }
            std::cout << std::endl;
        }

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
