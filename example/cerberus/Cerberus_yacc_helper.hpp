#ifndef CERBERUS_CERBERUS_YACC_HELPER_HPP
#define CERBERUS_CERBERUS_YACC_HELPER_HPP

#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include "Cerberus_lexer.hpp"
#include <cerberus/analyzation/parser/tree.hpp>

namespace CL {
    using namespace cerb;
    namespace cap    = cerb::analysis::parser;
    using token_t    = typename CerberusLexer<>::token_t;
    using ParserNode = cap::ParserNode<token_t>;

    CerberusLexerTemplate struct CerberusLexerImpl final : public CerberusLexer<>
    {
        CerberusLexerAccess;
        Vector<token_t> m_tokens{};

        constexpr auto yield(const token_t &token) -> bool override
        {
            m_tokens.emplace_back(token);
            return true;
        }

        constexpr auto error(const item_t &item, const string_view_t &repr)
            -> void override
        {
            analysis::basic_lexical_error(
                item, repr, "Unable to find suitable dot item for: ");
        }

        constexpr auto syntax_error(token_t *token) -> void
        {
            analysis::basic_syntax_error(*head(), token->repr, "Syntax error: ");
        }

        constexpr auto finish() -> void override
        {}
    };
}// namespace CL

extern "C" int yylex(void);
extern "C" void yyerror(const char *message);


inline CL::token_t *current_token;
inline CL::CerberusLexerImpl cerberus_lexer{};
inline CL::ParserNode *current_body{};
inline CL::token_t statement_token = { "Parser statements",
                                   CerberusLexerItem::STMT,
                                   {} };
inline CL::token_t auto_token      = { "Auto type generated by compiler",
                              CerberusLexerItem::BUILTIN_AUTO_T,
                              {} };

#endif /* CERBERUS_CERBERUS_YACC_HELPER_HPP */