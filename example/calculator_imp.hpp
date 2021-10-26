#ifndef CERBERUS_CALCULATORIMPL_HPP
#define CERBERUS_CALCULATORIMPL_HPP

#include "calculator.hpp"
#include <cerberus/analyzation/parser/SLR1.hpp>

/*
CalculatorTemplate struct CalculatorImp final : public Calculator<>
{
    CalculatorAccess;

    constexpr static u32 SYE = std::numeric_limits<u32>::max();

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
                Table.emplace(elem, SYE);
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
        { 0, { 1, 6, 5, SYE, 7, SYE, 2 } },
        { 1,
          { SYE, SYE, SYE, 3, SYE, SYE,
            2 } },
        { 2, { EoF, 1 } },
        { 3, { SYE, 4, 5, SYE, 7, SYE, SYE } },
        { 4, { EXPR, 3 } },
        { 5, { TERM, 1 } },
        { 6, { EXPR, 1 } },
        { 7, { 8, 6, 5, SYE, 7, SYE, SYE } },
        { 8,
          { SYE, SYE, SYE, 3, SYE, 9,
            SYE } },
        { 9, { TERM, 3 } }
    };

    u32 m_current_state{};
    u32 tokens_count{};
    token_t m_current_token{};
    cerb::Vector<cerb::Pair<u32, token_t>> lr_stack{};

    constexpr auto
        process_token(const token_t &current_token, const token_t &next_token)
            -> void
    {
        if (!Table[m_current_state].reduce) {
            lr_stack.emplace_back(m_current_state, current_token);
            m_current_state = Table[m_current_state][current_token.type];

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

        while (Table[m_current_state].reduce) {
            for (size_t i = 1; i < Table[m_current_state].size; ++i) {
                lr_stack.pop_back();
            }

            lr_stack.back().second.type = Table[m_current_state].type;
            m_current_state =
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
            std::cout << std::endl << std::endl;

            if (m_current_state == SYE || m_current_state == 2) {
                break;
            }
        }
    }

    constexpr auto yield(const token_t &token) -> bool override
    {
        fmt::print(
            "{:<16} {:<20} ", token.repr.to_string(),
            CalculatorItemItemsNames[token.type].to_string());

        std::cout << token.pos << std::endl;

        if (tokens_count > 0) {
            if (Table[m_current_state][m_current_token.type] == SYE) {
                cerb::analysis::basic_syntax_error(
                    *head(), m_current_token.repr, "Syntax error!");
            } else {
                process_token(m_current_token, token);
            }
        }
        ++tokens_count;
        m_current_token = token;
        return true;
    }

    constexpr auto error(const item_t &item, const string_view_t &repr)
        -> void override
    {
        cerb::analysis::basic_lexical_error(
            item, repr, "Unable to find suitable dot item for: ");
    }

    constexpr auto finish() -> void override
    {
        process_token(m_current_token, {});
    }
};*/


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
