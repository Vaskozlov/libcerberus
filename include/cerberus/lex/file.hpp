#ifndef CERBERUS_FILE_HPP
#define CERBERUS_FILE_HPP

#include <cerberus/types.h>

namespace cerb::lex {
    struct PositionInFile
    {
        u32 line_number{ 0 };
        u32 char_number{ 0 };
        const char *filename{ nullptr };

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

        constexpr explicit PositionInFile(const char *file)
          : filename(file)
        {}
    };
}// namespace cerb::lex

#endif /* CERBERUS_FILE_HPP */
