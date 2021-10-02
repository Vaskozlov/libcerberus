#ifndef CERBERUS_LEXICAL_GENERATOR_HPP
#define CERBERUS_LEXICAL_GENERATOR_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include <cerberus/types.h>
#include <cerberus/deque.hpp>
#include <cerberus/lex/lex.hpp>

enum Lex4LexTypes : u32
{
    IDENTIFIER,
    CURLY_OPENING,    // '{'
    CURLY_CLOSING,    // '}'
    LEFT_PARENTHESIS, // '('
    RIGHT_PARENTHESIS,// ')'
    ANGLE_OPENING,    // '['
    ANGLE_CLOSING,    // ']'
    COMMA,            // ','
    SEPARATOR,        // ';'
    PREPROCESSOR,     // '#'
    COLON,            // ':'
    EoF               // "%%"
};

CERBERUS_LEX_TEMPLATES
struct Lex4Lex : public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS

    struct BlockOfItems
    {
        size_t power{};
        cerb::Vector<string_view_t> elems{};
    };

    enum RuleState
    {
        NIL,
        BLOCK,
        ELEM_DECLARATION
    };

    enum struct BlockState
    {
        ZERO,
        ONE,
        TWO,
        TREE,
    };

    RuleState current_state{ NIL };
    BlockState block_state{ BlockState::ZERO };
    BlockOfItems *current_block{ nullptr };

    size_t m_current_index{};
    size_t m_current_power{ 6 };
    token_t m_token{};
    cerb::Deque<token_t> m_tokens{};
    cerb::Map<string_view_t, BlockOfItems> m_blocks{};

    constexpr auto throw_if_can(bool condition, const char *message) -> void
    {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }

    constexpr auto process_block(const token_t &token) -> void
    {
        switch (block_state) {
        case BlockState::ZERO:
            throw_if_can(
                token.type == ANGLE_OPENING,
                "process_block has been called, but parser can't find '[' to start "
                "BLOCK!");
            block_state = BlockState::ONE;
            return;

        case BlockState::ONE: {
            throw_if_can(
                token.type == IDENTIFIER,
                "Parser error! Unable to find IDENTIFIER for BLOCK declaration!");
            auto &block = m_blocks.insert({ token.repr, {} });
            current_block = &block;
            block_state       = BlockState::TWO;
        } break;

        case BlockState::TWO:
            throw_if_can(
                token.type == ANGLE_CLOSING,
                "Parser error! Unable to find ']' to close BLOCK declaration!");
            block_state   = BlockState::ZERO;
            current_state = NIL;
            break;
        }
    }

    constexpr bool yield(const token_t &token) override
    {
        std::cout << std::setw(8) << std::left << token.repr << ' ' << std::setw(4)
                  << std::left << token.type << ' ' << token.pos << std::setw(0)
                  << std::endl;
        m_tokens.emplace_back(token);

        switch (current_state) {
        case NIL:
            switch (token.type) {
            case ANGLE_OPENING:
                process_block(token);
                break;

            case IDENTIFIER:

                break;
            }
            break;

        case BLOCK:
            process_block(token);
            break;

        case ELEM_DECLARATION:

            break;
        }

        return token.type != EoF;
    }

    constexpr void error(
        const position_t &pos, const string_view_t &line,
        const string_view_t &repr) override
    {
        fmt::print(
            "Lexical analyzer error! At: file: {}, line: {}, column: {}\n",
            pos.filename.to_string(), pos.line_number, pos.char_number);

        fmt::print(fmt::emphasis::italic, "{}\n", line.to_string());
        CERBLIB_UNROLL_N(4)
        for (int i = 0; i < static_cast<int>(repr.begin() - line.begin()); ++i) {
            putchar(' ');
        }
        fmt::print(fmt::fg(fmt::color::green), "^\n");
        fmt::print("Representation of token: {}", repr.to_string());
        std::cout << std::endl;
        throw std::runtime_error("");
    }

    constexpr CERBERUS_LEX_INITIALIZER(Lex4Lex)
    {}
};

static Lex4Lex<char, Lex4LexTypes> lex{
    {},
    { { IDENTIFIER, "[a-zA-Z_]+[a-zA-Z0-9_]*" } },
    { { { ANGLE_OPENING, '[' }, { ANGLE_CLOSING, ']' }, { COLON, ':' } },
      { { EoF, "%%" } } }
};

auto main2(int argc, char *argv[]) -> int
{
    std::ifstream t(argv[1]);
    std::stringstream buffer{};
    buffer << t.rdbuf();

    std::string data         = buffer.str();
    size_t offset            = data.find_first_of("%%");
    std::string lexical_body = data.substr(offset + 2);

    lex.scan(lexical_body.c_str(), argv[1]);
    return 0;
}

#endif /* CERBERUS_LEXICAL_GENERATOR_HPP */
