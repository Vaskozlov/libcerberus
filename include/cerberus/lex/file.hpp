#ifndef CERBERUS_FILE_HPP
#define CERBERUS_FILE_HPP

#include <cerberus/types.h>

namespace cerb::lex {
    struct PositionInFile
    {
        size_t line_number{ 0 };
        size_t char_number{ 0 };
        std::string_view filename{};

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

        constexpr PositionInFile(size_t line, size_t char_n, const std::string_view &s)
          : line_number(line), char_number(char_n), filename(s)
        {}
    };
}// namespace cerb::lex

#endif /* CERBERUS_FILE_HPP */
