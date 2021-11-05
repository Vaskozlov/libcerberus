#ifndef CERBERUS_CALCULATOR_IMP_YACC_HPP
#define CERBERUS_CALCULATOR_IMP_YACC_HPP

#include "calculator.hpp"
#include "cerberus/vector.hpp"

extern "C" int yylex(void);
extern "C" void yyerror(const char *message) {
    std::cerr << message << std::endl;
}

CalculatorTemplate struct CalculatorImpYacc final : public Calculator<>
{
    CalculatorAccess;
    cerb::Vector<token_t> m_tokens{};

    constexpr auto yield(const token_t &token) -> bool override
    {
        m_tokens.emplace_back(token);
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

#endif// CERBERUS_CALCULATOR_IMP_YACC_HPP
