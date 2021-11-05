#ifndef CERBERUS_TOKEN_HPP
#define CERBERUS_TOKEN_HPP

#include <cerberus/analyzation/lex/file.hpp>

namespace cerb::lex {
    template<typename CharT, typename Repr, typename TokenType>
    struct Token
    {
        Repr repr{};
        TokenType type{};
        PositionInFile<CharT> pos{};

    public:
        constexpr Token()                       = default;
        constexpr ~Token() noexcept             = default;
        constexpr Token(Token &&) noexcept      = default;
        constexpr Token(const Token &) noexcept = default;

        constexpr Token(Repr repr_, TokenType type_, PositionInFile<CharT> pos_)
          : repr(repr_), type(type_), pos(pos_)
        {}

        constexpr auto operator=(Token &&) noexcept -> Token & = default;
        constexpr auto operator=(const Token &) noexcept -> Token & = default;

        constexpr auto operator==(const Token &other) const -> bool
        {
            return type == other.type;
        }

        constexpr auto operator<=>(const Token &other) const
        {
            return type <=> other.type;
        }

        constexpr auto operator==(const TokenType &other) const -> bool
        {
            return type == other;
        }

        constexpr auto operator<=>(const TokenType &other) const
        {
            return type <=> other;
        }

        constexpr auto operator==(const Repr &other) const -> bool
        {
            return type == other;
        }

        constexpr auto operator<=>(const Repr &other) const
        {
            return type <=> other;
        }
    };
}// namespace cerb::lex


template<typename T, typename CharT, typename Repr, typename TokenType>
auto operator<<(T &os, const cerb::lex::Token<CharT, Repr, TokenType> &token) -> T &
{
    os << "Type: " << token.type << ", \"" << token.repr << "\"";
    return os;
}

#endif /* CERBERUS_TOKEN_HPP */
