#ifndef CERBERUS_EXCEPTIONS_HPP
#define CERBERUS_EXCEPTIONS_HPP

#include <utility>
#include <stdexcept>
#include <cerberus/types.h>

using namespace cerb::literals;
using namespace std::string_literals;

namespace cerb::analysis {
    class lexical_analysis_error final : public std::exception
    {
        std::string message;

    public:
        explicit lexical_analysis_error(std::string str) : message(std::move(str))
        {}

        explicit lexical_analysis_error(const char *message) : message(message)
        {}

        lexical_analysis_error(lexical_analysis_error &&) noexcept      = default;
        lexical_analysis_error(const lexical_analysis_error &) noexcept = default;

        auto operator                   =(lexical_analysis_error &&) noexcept
            -> lexical_analysis_error & = default;
        auto operator                   =(const lexical_analysis_error &) noexcept
            -> lexical_analysis_error & = default;

        ~lexical_analysis_error() override = default;

        [[nodiscard]] virtual const char *what() const noexcept
        {
            return message.c_str();
        }
    };

    class syntax_analysis_error final : public std::exception
    {
        std::string message;

    public:
        explicit syntax_analysis_error(std::string str) : message(std::move(str))
        {}

        explicit syntax_analysis_error(const char *message) : message(message)
        {}

        syntax_analysis_error(syntax_analysis_error &&) noexcept      = default;
        syntax_analysis_error(const syntax_analysis_error &) noexcept = default;

        ~syntax_analysis_error() override = default;

        auto operator                  =(syntax_analysis_error &&) noexcept
            -> syntax_analysis_error & = default;
        auto operator                  =(const syntax_analysis_error &) noexcept
            -> syntax_analysis_error & = default;

        [[nodiscard]] virtual const char *what() const noexcept
        {
            return message.c_str();
        }
    };

    constexpr auto
        basic_syntax_error(const auto &item, const auto &repr, const char *message)
            -> void
    {
        const auto &pos  = item.get_token_pos();
        const auto &line = item.get_line();

        fmt::print(
            "Syntax analysis error! At: file: {}, line: {}, column: {}\n",
            pos.filename.to_string(), pos.line_number, pos.char_number);
        fmt::print(fmt::emphasis::italic, "{}\n", line.to_string());

        CERBLIB_UNROLL_N(4)
        for (int i = 0;
             i < static_cast<int>(item.get_input().begin() - line.begin());
             ++i) {
            putchar(' ');
        }

        fmt::print(fmt::fg(fmt::color::green), "^\n");
        fmt::print(
            "Representation of token, which cause error: {}\nDescription: {}",
            repr.to_string(), message);
        std::cout << std::endl;

        throw cerb::analysis::syntax_analysis_error("Syntax error: "s + message);
    }

    constexpr auto
        basic_lexical_error(const auto &item, const auto &repr, const char *message)
            -> void
    {
        const auto &pos  = item.get_token_pos();
        const auto &line = item.get_line();

        fmt::print(
            "Lexical analysis error! At: file: {}, line: {}, column: {}\n",
            pos.filename.to_string(), pos.line_number, pos.char_number);

        fmt::print(fmt::emphasis::italic, "{}\n", line.to_string());

        CERBLIB_UNROLL_N(4)
        for (int i = 0;
             i < static_cast<int>(repr.begin() - item.get_input().begin());
             ++i) {
            putchar(' ');
        }

        fmt::print(fmt::fg(fmt::color::green), "^\n");
        fmt::print("Representation of token: {}", repr.to_string());
        std::cout << std::endl;

        throw cerb::analysis::lexical_analysis_error(
            std::string(message) + std::string(repr.to_string()));
    }
}// namespace cerb::analysis

#endif /* CERBERUS_EXCEPTIONS_HPP */
