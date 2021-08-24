#ifndef CERBERUS_CHAR_HPP
#define CERBERUS_CHAR_HPP

#include <cerberus/types.h>


namespace cerb::lex {
    constexpr auto is_end_of_input(char ch) -> bool
    {
        return ch == '\0';
    }

    constexpr auto is_layout(char ch) -> bool
    {
        return !is_end_of_input(ch) && ch <= ' ';
    }

    constexpr auto is_uc_letter(char ch) -> bool
    {
        return ch >= 'A' && ch <= 'Z';
    }

    constexpr auto is_lc_letter(char ch) -> bool
    {
        return ch >= 'a' && ch <= 'z';
    }

    constexpr auto is_letter(char ch) -> bool
    {
        return is_lc_letter(ch) || is_uc_letter(ch);
    }

    constexpr auto is_digit(char ch) -> bool
    {
        return ch >= '0' && ch <= '9';
    }

    constexpr auto is_letter_or_digit(char ch) -> bool
    {
        return is_letter(ch) || is_digit(ch);
    }

    constexpr auto is_underscore(char ch) -> bool
    {
        return ch == '_';
    }
}// namespace cerb::lex

#endif /* CERBERUS_CHAR_HPP */
