#ifndef CERBERUS_CALCULATORIMPL_HPP
#define CERBERUS_CALCULATORIMPL_HPP

#include "calculator.hpp"

CalculatorTemplate struct CalculatorImp final : public Calculator<>
{
    CalculatorAccess;

    constexpr static u32 SyntaxError = std::numeric_limits<u32>::max();

    template<size_t N>
    struct LR0_Item
    {
        bool reduce{};
        u32 size{};
        TokenType type{};
        cerb::gl::Map<TokenType, u32, N> Table{};
        constexpr static std::array types = {
            EXPR, TERM, INT, ADD, LEFT_PARENTHESIS, RIGHT_PARENTHESIS, EoF
        };

        constexpr LR0_Item() = default;

        constexpr LR0_Item(TokenType type_, u32 size_)
          : reduce(true), size(size_), type(type_)
        {
            CERBLIB_UNROLL_N(4)
            for (const auto &elem : types) {
                Table.emplace(elem, SyntaxError);
            }
        }

        constexpr LR0_Item(const std::initializer_list<u32> &data)
        {
            typename std::initializer_list<u32>::iterator it = data.begin();

            CERBLIB_UNROLL_N(2)
            for (const auto &elem : types) {
                Table.emplace(elem, *it);
                ++it;
            }
        }

        constexpr auto operator[](const TokenType &key) const noexcept
        {
            return Table.at(key);
        }
    };

    static constexpr cerb::gl::Map<u32, LR0_Item<7>, 10> Table{
        { 0, { 1, 6, 5, SyntaxError, 7, SyntaxError, 2 } },
        { 1,
          { SyntaxError, SyntaxError, SyntaxError, 3, SyntaxError, SyntaxError,
            2 } },
        { 2, { EoF, 1 } },
        { 3, { SyntaxError, 4, 5, SyntaxError, 7, SyntaxError, SyntaxError } },
        { 4, { EXPR, 3 } },
        { 5, { TERM, 1 } },
        { 6, { EXPR, 1 } },
        { 7, { 8, 6, 5, SyntaxError, 7, SyntaxError, SyntaxError } },
        { 8,
          { SyntaxError, SyntaxError, SyntaxError, 3, SyntaxError, 9,
            SyntaxError } },
        { 9, { TERM, 3 } }
    };

    u32 current_state{};
    cerb::Vector<cerb::Pair<u32, token_t>> lr_stack{};

    constexpr auto yield(const token_t &token) -> bool override
    {
        fmt::print(
            "{:<16} {:<20} ", token.repr.to_string(),
            CalculatorItemItemsNames[token.type].to_string());

        std::cout << token.pos << std::endl;

        if (Table[current_state][token.type] == SyntaxError) {
            cerb::analysis::basic_syntax_error(*head(), token.repr, "Syntax error!");
        } else {
            if (!Table[current_state].reduce) {
                lr_stack.emplace_back(current_state, token);
                current_state = Table[current_state][token.type];

                fmt::print("Stack: \n");
                CERBLIB_UNROLL_N(2)
                for (const auto &elem : lr_stack) {
                    if (CalculatorItemItemsNames.contains(elem.second)) {
                        fmt::print(
                            "{{ {}: {} }}, ", elem.first,
                            CalculatorItemItemsNames[elem.second.type].to_string());
                    }
                }
                std::cout << std::endl;
            }
            while (Table[current_state].reduce) {
                lr_stack.back().second.type = Table[current_state].type;

                for (size_t i = 1; i < Table[current_state].size; ++i) {
                    lr_stack.pop_back();
                }

                current_state =
                    Table[lr_stack.back().first][lr_stack.back().second.type];

                fmt::print("Stack: \n");
                CERBLIB_UNROLL_N(2)
                for (const auto &elem : lr_stack) {
                    if (CalculatorItemItemsNames.contains(elem.second)) {
                        fmt::print(
                            "{{ {}: {} }}, ", elem.first,
                            CalculatorItemItemsNames[elem.second.type].to_string());
                    }
                }
                std::cout << std::endl;

                if (current_state == SyntaxError || current_state == 2) {
                    break;
                }
            }
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
