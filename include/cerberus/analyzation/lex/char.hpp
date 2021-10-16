#ifndef CERBERUS_CHAR_HPP
#define CERBERUS_CHAR_HPP

#include <cerberus/types.h>

namespace cerb::lex {
    template<typename CharT>
    constexpr auto is_end_of_input(CharT chr) -> bool
    {
        return chr == static_cast<CharT>(0);
    }

    template<typename CharT>
    constexpr auto is_layout(CharT chr) -> bool
    {
        return !is_end_of_input(chr) && chr <= static_cast<CharT>(' ');
    }

    template<typename CharT>
    constexpr auto is_layout_or_end_of_input(CharT chr) -> bool
    {
        return chr <= static_cast<CharT>(' ');
    }

    template<typename CharT>
    constexpr auto is_uc_letter(CharT chr) -> bool
    {
        return chr >= static_cast<CharT>('A') && chr <= static_cast<CharT>('Z');
    }

    template<typename CharT>
    constexpr auto is_lc_letter(CharT chr) -> bool
    {
        return chr >= static_cast<CharT>('a') && chr <= static_cast<CharT>('z');
    }

    template<typename CharT>
    constexpr auto is_letter(CharT chr) -> bool
    {
        return is_lc_letter<CharT>(chr) || is_uc_letter<CharT>(chr);
    }

    template<typename CharT>
    constexpr auto is_digit(CharT chr) -> bool
    {
        return chr >= static_cast<CharT>('0') && chr <= static_cast<CharT>('9');
    }

    template<typename CharT>
    constexpr auto is_letter_or_digit(CharT chr) -> bool
    {
        return is_letter<CharT>(chr) || is_digit<CharT>(chr);
    }

    template<typename CharT>
    constexpr auto is_underscore(CharT chr) -> bool
    {
        return chr == static_cast<CharT>('_');
    }
}// namespace cerb::lex

#endif /* CERBERUS_CHAR_HPP */
