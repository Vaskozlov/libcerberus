#ifndef CERBERUS_FILE_HPP
#define CERBERUS_FILE_HPP

#include <cerberus/types.h>

namespace cerb::lex {
    template<typename CharT = char>
    struct PositionInFile
    {
        size_t line_number{ 0 };
        size_t char_number{ 0 };
        std::basic_string_view<CharT> filename{};

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
            size_t line, size_t char_n, const std::basic_string_view<CharT> &s)
          : line_number(line), char_number(char_n), filename(s)
        {}
    };
}// namespace cerb::lex

#endif /* CERBERUS_FILE_HPP */
