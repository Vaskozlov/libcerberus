#ifndef CERBERUS_CHAR_HPP
#define CERBERUS_CHAR_HPP

#include <cerberus/types.h>

namespace cerb::lex {
    template<typename CharT>
    constexpr auto is_end_of_input(CharT ch) -> bool
    {
        return ch == static_cast<CharT>(0);
    }

    template<typename CharT>
    constexpr auto is_layout(CharT ch) -> bool
    {
        return !is_end_of_input(ch) && ch <= static_cast<CharT>(' ');
    }

    template<typename CharT>
    constexpr auto is_uc_letter(CharT ch) -> bool
    {
        return ch >= static_cast<CharT>('A') && ch <= static_cast<CharT>('Z');
    }

    template<typename CharT>
    constexpr auto is_lc_letter(CharT ch) -> bool
    {
        return ch >= static_cast<CharT>('a') && ch <= static_cast<CharT>('z');
    }

    template<typename CharT>
    constexpr auto is_letter(CharT ch) -> bool
    {
        return is_lc_letter<CharT>(ch) || is_uc_letter<CharT>(ch);
    }

    template<typename CharT>
    constexpr auto is_digit(CharT ch) -> bool
    {
        return ch >= static_cast<CharT>('0') && ch <= static_cast<CharT>('9');
    }

    template<typename CharT>
    constexpr auto is_letter_or_digit(CharT ch) -> bool
    {
        return is_letter<CharT>(ch) || is_digit<CharT>(ch);
    }

    template<typename CharT>
    constexpr auto is_underscore(CharT ch) -> bool
    {
        return ch == static_cast<CharT>('_');
    }
}// namespace cerb::lex

#endif /* CERBERUS_CHAR_HPP */
