#ifndef CERBERUS_FILE_HPP
#define CERBERUS_FILE_HPP

#include <cerberus/types.h>
#include <cerberus/string_view.hpp>

namespace cerb::lex {
    template<typename CharT = char>
    struct PositionInFile
    {
        size_t line_number{ 0 };
        size_t char_number{ 0 };
        cerb::basic_string_view<CharT> filename{};

    public:
        constexpr auto operator+=(size_t value) -> PositionInFile &
        {
            char_number += value;
            return *this;
        }

        constexpr auto operator-=(size_t value) -> PositionInFile &
        {
            char_number -= value;
            return *this;
        }

        constexpr friend auto operator+(const PositionInFile &pos, size_t value)
            -> PositionInFile
        {
            return { pos.line_number, pos.char_number + value, pos.filename };
        }

        constexpr auto new_line() -> void
        {
            ++line_number;
            char_number = 0;
        }

    public:
        constexpr auto operator =(const PositionInFile &)
            -> PositionInFile & = default;
        constexpr auto operator =(PositionInFile &&) noexcept
            -> PositionInFile & = default;

    public:
        constexpr PositionInFile()  = default;
        constexpr ~PositionInFile() = default;

        constexpr PositionInFile(const PositionInFile &)     = default;
        constexpr PositionInFile(PositionInFile &&) noexcept = default;

        constexpr PositionInFile(
            size_t line, size_t char_n, const cerb::basic_string_view<CharT> &s)
          : line_number(line), char_number(char_n), filename(s)
        {}
    };
}// namespace cerb::lex

template<typename T, typename CharT>
constexpr auto operator<<(T &os, const cerb::lex::PositionInFile<CharT> &pos) -> T &
{
    os << "File: " << pos.filename << ", line: " << pos.line_number + 1
       << ", char: " << pos.char_number + 1;
    return os;
}

#endif /* CERBERUS_FILE_HPP */
