#ifndef CERBERUS_LEXER4C_IMP_HPP
#define CERBERUS_LEXER4C_IMP_HPP

#include "C_lexer.hpp"

Lex4CTemplate struct Lex4CImp final : public Lex4C<>
{
    Lex4CAccess;

    constexpr auto yield(const token_t &token) -> bool override
    {
        fmt::print(
            "{:<16} {:<20} ", token.repr.to_string(),
            Lex4CItemItemsNames[token.type].to_string());

        std::cout << token.pos << std::endl;

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

#endif /* CERBERUS_LEXER4C_IMP_HPP */
