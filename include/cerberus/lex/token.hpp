#ifndef CERBERUS_TOKEN_HPP
#define CERBERUS_TOKEN_HPP

#include <cerberus/lex/file.hpp>

namespace cerb::lex {
    template<typename Repr, typename TokenType>
    struct Token
    {
        Repr repr{};
        TokenType type{};
        PositionInFile<char> pos{};
    };
}// namespace cerb::lex

#endif /* CERBERUS_TOKEN_HPP */
