#ifndef CERBERUS_CALCULATORIMPL_HPP
#define CERBERUS_CALCULATORIMPL_HPP

#include "calculator.hpp"

CalculatorTemplate struct CalculatorImp final : public Calculator<>
{
    CalculatorAccess;

    constexpr auto yield(const token_t &token) -> bool override
    {
        fmt::print(
            "{:<16} {:<20} ", token.repr.to_string(),
            CalculatorItemItemsNames[token.type].to_string());

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

#endif /* CERBERUS_CALCULATORIMPL_HPP */
