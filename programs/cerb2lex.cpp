#include <fstream>
#include <sstream>
#include <iostream>
#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <cerberus/analyzation/lex/lex.hpp>

using namespace cerb::literals;
using namespace std::string_literals;

enum Lex4LexTypes : u32
{
    IDENTIFIER,
    ASSIGN,           // '='
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
    WORD,             // '|'
    EoF               // "%%"
};


CERBERUS_LEX_TEMPLATES
struct Lex4Lex : public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS

    struct BlockOfItems
    {
        size_t strength{};
        size_t power{};
        string_view_t block_name{};
        cerb::Vector<cerb::Pair<string_view_t, string_view_t>> words{};
        cerb::Vector<cerb::Pair<string_view_t, string_view_t>> rules{};
        cerb::Vector<cerb::Pair<string_view_t, string_view_t>> operators{};
    };

    enum RuleState
    {
        NIL,
        BLOCK,
        ELEM_DECLARATION,
        CLASS_DECLARATION,
    };

    enum struct State
    {
        ZERO,
        ONE,
        TWO
    };

private:
    bool is_word{ false };
    RuleState current_state{ NIL };
    State sub_state{ State::ZERO };
    BlockOfItems *current_block{ nullptr };
    string_view_t identifier{};

    size_t m_current_index{};
    size_t m_strength{ 64 };
    token_t m_token{};
    cerb::Deque<token_t> m_tokens{};
    cerb::Map<string_view_t, BlockOfItems> m_blocks{};
    string_view_t m_directive{};
    cerb::gl::Map<string_view_t, string_view_t, 14> m_directives{
        { "CHAR_TYPE"_sv, "char" },
        { "CLASS_NAME"_sv, ""_sv },
        { "MAY_THROW"_sv, "true"_sv },
        { "STRING_PREFIX"_sv, ""_sv },
        { "STRING_POSTFIX"_sv, ""_sv },
        { "CHAR_PREFIX"_sv, ""_sv },
        { "CHAR_POSTFIX"_sv, ""_sv },
        { "ALLOW_COMMENTS"_sv, "true" },
        { "MAX_TERMINALS"_sv, "128"_sv },
        { "MAX_SIZE_FOR_TERMINAL"_sv, "4"_sv },
        { "SINGLE_LINE_COMMENT"_sv, "//"_sv },
        { "MULTILINE_COMMENT_BEGIN"_sv, "/*"_sv },
        { "MULTILINE_COMMENT_END"_sv, "*/"_sv },
        { "ALLOW_STRING_LITERALS"_sv, "true"_sv }
    };

    std::string generated_string{};
    static constexpr size_t MaxPriority = 64;

private:
    static constexpr auto check_token(const string_view_t &token) -> bool
    {
        CERBLIB_UNROLL_N(4)
        for (size_t i = 0; i < token.size(); ++i) {
            if (token[i] == '[' && token[i + 1] != '[') {// it is rule
                return true;
            }
        }
        return false;
    }

    constexpr auto throw_if_can(bool condition, const char *message) -> void
    {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }

    constexpr auto
        throw_if_can(bool condition, const token_t &token, const char *message)
            -> void
    {
        if (!condition) {
            syntax_error(*head(), token.repr, message);
        }
    }

public:
    [[nodiscard]] constexpr auto get_result() const noexcept -> const std::string &
    {
        return generated_string;
    }

    constexpr auto process_block(const token_t &token) -> void
    {
        switch (sub_state) {
        case State::ZERO:
            throw_if_can(
                token.type == ANGLE_OPENING,
                token,
                "process_block has been called, but parser can't find '[' to start "
                "BLOCK!");
            sub_state     = State::ONE;
            current_state = BLOCK;
            return;

        case State::ONE: {
            throw_if_can(
                token.type == IDENTIFIER,
                token,
                "Parser error! Unable to find IDENTIFIER for BLOCK declaration!");
            auto &block    = m_blocks.insert({ token.repr, {} });
            current_block  = &block;
            sub_state      = State::TWO;
            block.strength = m_strength--;
        } break;

        case State::TWO:
            throw_if_can(
                token.type == ANGLE_CLOSING,
                token,
                "Parser error! Unable to find ']' to close BLOCK declaration!");
            sub_state     = State::ZERO;
            current_state = NIL;
            break;
        }
    }

    constexpr auto process_declaration(const token_t &token) -> void
    {
        switch (sub_state) {
        case State::ZERO:
            throw_if_can(
                token.type == IDENTIFIER,
                token,
                "Parser error! Unable to find IDENTIFIER to process declaration!");
            identifier    = token.repr;
            sub_state     = State::ONE;
            current_state = ELEM_DECLARATION;
            m_directive   = token.repr;
            break;

        case State::ONE:
            if (token.type == ASSIGN) {
                throw_if_can(
                    m_directives.contains(m_directive),
                    "Unable to recognize directive.");
                sub_state     = State::TWO;
                current_state = CLASS_DECLARATION;
                break;
            }

            throw_if_can(
                token.type == COLON,
                token,
                "Parser error! Unable to find colon in declaration (':')!");
            sub_state = State::TWO;
            break;

        case State::TWO:
            if (token.type == WORD) {
                is_word = true;
                break;
            }

            current_state = NIL;
            sub_state     = State::ZERO;

            throw_if_can(
                current_block != nullptr,
                token,
                "Please open any block to define elems. [BLOCK NAME] "
                "<declarations>");

            throw_if_can(
                token.type == static_cast<TokenType>(100) ||
                    token.type == static_cast<TokenType>(101),
                token,
                "Rule must be string or char");

            if (check_token(token.repr)) {
                current_block->rules.emplace_back(identifier, token.repr);
            } else if (is_word) {
                current_block->words.emplace_back(identifier, token.repr);
            } else {
                current_block->operators.emplace_back(identifier, token.repr);
            }

            is_word = false;
            break;
        }
    }

    constexpr auto process_class(const token_t &token) -> void
    {
        switch (sub_state) {
        case State::TWO:
            throw_if_can(
                token.type == 100 || token.type == 101, token,
                "Unable to find name for CLASS_NAME.");
            current_state = NIL;
            sub_state     = State::ZERO;

            m_directives[m_directive] = token.repr;
            m_directive               = { m_directive.begin(), m_directive.begin() };
            break;

        default:
            throw_if_can(false, "Unable to process directive.");
        }
    }

    constexpr auto yield(const token_t &token) -> bool override
    {
        m_tokens.emplace_back(token);

        switch (current_state) {
        case NIL:
            switch (token.type) {
            case ANGLE_OPENING:
                process_block(token);
                break;

            case IDENTIFIER:
                process_declaration(token);
                break;

            case EoF:
                return false;

            default:
                syntax_error(
                    *head(), token.repr,
                    "Unable to recognize any action with this token!");
            }
            break;

        case BLOCK:
            process_block(token);
            break;

        case ELEM_DECLARATION:
            process_declaration(token);
            break;

        case CLASS_DECLARATION:
            process_class(token);
            break;

        default:
            syntax_error(
                *head(), token.repr,
                "Unable to recognize any action with this token!");
        }

        return token.type != EoF;
    }

    constexpr void syntax_error(
        const item_t &item, const string_view_t &repr, const char *message)
    {
        cerb::analysis::basic_syntax_error(item, repr, message);
    }

    constexpr void error(const item_t &item, const string_view_t &repr) override
    {
        cerb::analysis::basic_lexical_error(
            item, repr, "Unable to find suitable dot item for: ");
    }

    constexpr void finish() override
    {
        constexpr unsigned long MinimumPower = 6;
        cerb::gl::Set<u16, bitsizeof(uintmax_t)> taken_powers{};

        if (m_directives["CLASS_NAME"].size() == 0) {
            throw std::logic_error("CLASS_NAME directive must be used.");
        }

        for (auto &elem : m_blocks) {
            auto power = cerb::max<unsigned long>(
                MinimumPower,
                cerb::log2(
                    elem.second.words.size() + elem.second.rules.size() +
                    elem.second.operators.size()) +
                    1);

            CERBLIB_UNROLL_N(2)
            while (taken_powers.contains(power)) {
                throw_if_can(
                    power < bitsizeof(u64),
                    "Lexical analyzer can't hold so many items");
                ++power;
            }

            taken_powers.insert(power);
            elem.second.power = power;
        }

        generated_string = fmt::format(
            "enum {}Blocks : size_t\n{{\n", m_directives["CLASS_NAME"].to_string());

        CERBLIB_UNROLL_N(2)
        for (auto &elem : m_blocks) {
            generated_string += fmt::format(
                "    {:<16} = {}UL,\n", elem.first.to_string(),
                (1UL << elem.second.power));// std::string(elem.first.to_string());
        }

        generated_string += fmt::format(
            "}};\n\nenum {}Items : size_t\n{{\n",
            m_directives["CLASS_NAME"].to_string());

        for (auto &elem : m_blocks) {
            size_t j = 0;

            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.words) {
                generated_string += fmt::format(
                    "    {:<16} = static_cast<size_t>({}) + {}UL,\n",
                    i.first.to_string(), elem.first.to_string(), j++);
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.rules) {
                generated_string += fmt::format(
                    "    {:<16} = static_cast<size_t>({}) + {}UL,\n",
                    i.first.to_string(), elem.first.to_string(), j++);
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.operators) {
                generated_string += fmt::format(
                    "    {:<16} = static_cast<size_t>({}) + {}UL,\n",
                    i.first.to_string(), elem.first.to_string(), j++);
            }
        }

        generated_string += fmt::format(
            R"(}};

template<
    typename CharT = {1},
    typename TokenType = {0}Items,
    bool MayThrow = {2},
    size_t UID = 0,
    bool AllowStringLiterals = {3},
    bool AllowComments = {4},
    size_t MaxTerminals = {5},
    size_t MaxSize4Terminals = {6}>
struct {0}: public CERBERUS_LEX_PARENT_CLASS
{{
    CERBERUS_LEX_PARENT_CLASS_ACCESS
)",
            m_directives["CLASS_NAME"].to_string(),
            m_directives["CHAR_TYPE"].to_string(),
            m_directives["MAY_THROW"].to_string(),
            m_directives["ALLOW_STRING_LITERALS"].to_string(),
            m_directives["ALLOW_COMMENTS"].to_string(),
            m_directives["MAX_TERMINALS"].to_string(),
            m_directives["MAX_SIZE_FOR_TERMINAL"].to_string());

        generated_string += fmt::format(
            R"(
    constexpr {}()
    : parent(
    {{)",
            m_directives["CLASS_NAME"].to_string());

        for (auto &elem : m_blocks) {
            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.words) {
                generated_string += fmt::format(
                    "\n        {{ {}, {}\"{}\"{}, true, {} }},", i.first.to_string(),
                    m_directives["STRING_PREFIX"].to_string(), i.second.to_string(),
                    m_directives["STRING_POSTFIX"].to_string(), MinimumPower - 1);
            }
        }

        for (auto &elem : m_blocks) {
            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.rules) {
                generated_string += fmt::format(
                    "\n        {{ {}, {}\"{}\"{} }},", i.first.to_string(),
                    m_directives["STRING_PREFIX"].to_string(), i.second.to_string(),
                    m_directives["STRING_POSTFIX"].to_string());
            }
        }

        generated_string.pop_back();
        generated_string += "\n    },\n    {\n        { ";

        for (auto &elem : m_blocks) {
            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.operators) {
                if (i.second.size() == 1) {
                    generated_string += fmt::format(
                        "\n            {{ {}, {}\'{}\'{} }},", i.first.to_string(),
                        m_directives["CHAR_PREFIX"].to_string(),
                        i.second.to_string(),
                        m_directives["CHAR_POSTFIX"].to_string());
                }
            }
        }

        generated_string += "\n        },\n       {";
        for (auto &elem : m_blocks) {
            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.operators) {
                if (i.second.size() > 1) {
                    generated_string += fmt::format(
                        "\n            {{ {}, {}\"{}\"{} }},", i.first.to_string(),
                        m_directives["STRING_PREFIX"].to_string(),
                        i.second.to_string(),
                        m_directives["STRING_POSTFIX"].to_string());
                }
            }
        }

        generated_string += "\n       }";
        generated_string += "\n    }\n    )\n    {}";
    }

    constexpr CERBERUS_LEX_INITIALIZER(Lex4Lex)
    {}
};

Lex4Lex<char, Lex4LexTypes> lex{ { { IDENTIFIER, "[a-zA-Z_]+[a-zA-Z0-9_]*" } },
                                 { { { ANGLE_OPENING, '[' },
                                     { ANGLE_CLOSING, ']' },
                                     { COLON, ':' },
                                     { ASSIGN, '=' },
                                     { WORD, '|' } },
                                   {
                                       { EoF, "%%" },
                                   } } };


auto main(int argc, char *argv[]) -> int
{
    std::string filename = argv[1];
    std::ifstream t(argv[1]);
    std::stringstream buffer{};
    buffer << t.rdbuf();
    t.close();

    std::string data       = buffer.str();
    size_t offset          = data.find_first_of("%%") + 2;
    std::string rest_class = data.substr(data.find("%%", offset) + 2);
    std::string rest       = rest_class.substr(rest_class.find("%%") + 2);
    rest_class.erase(rest_class.find("%%"));

    lex.scan(data.c_str() + offset, argv[1]);
    data.erase(offset - 2);

    data += lex.get_result() + rest_class + "};\n" + rest;

    filename.erase(filename.find_last_of('.'));
    filename += ".hpp";

    std::ofstream out(filename);
    out << data;
    out.close();

    return 0;
}