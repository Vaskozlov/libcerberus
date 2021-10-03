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
        constexpr ~Token() noexcept = default;

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

#endif /* CERBERUS_TOKEN_HPP */
