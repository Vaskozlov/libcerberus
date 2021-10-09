#include <fstream>
#include <sstream>
#include <iostream>
#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <cerberus/string_view.hpp>
#include <cerberus/analyzation/lex/lex.hpp>
#include "Lex4Lex.hpp"

using namespace cerb::literals;
using namespace std::string_literals;

Lex4LexTemplate struct Lex4LexImpl : Lex4Lex<>
{
    using Lex4Lex<>::parent;
    using parent::head;
    using item_t           = typename parent::item_t;
    using storage_t        = typename parent::storage_t;
    using token_t          = typename parent::token_t;
    using result_t         = typename parent::result_t;
    using position_t       = typename parent::position_t;
    using string_t         = typename parent::string_t;
    using string_view_t    = typename parent::string_view_t;
    using string_checker_t = typename parent::string_checker_t;
    using item_initilizer  = typename parent::item_initilizer;

    template<typename T>
    static constexpr auto char_cast(T chr) noexcept -> CharT
    {
        return static_cast<CharT>(chr);
    }

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

    struct Directive
    {
        cerb::gl::Set<TokenType, 4> allowed_types{};
        string_view_t repr{};

    public:
        [[nodiscard]] constexpr auto check_type(TokenType token) const noexcept
            -> bool
        {
            return allowed_types.contains(token);
        }

        constexpr auto operator==(const Directive &other) const noexcept -> bool
        {
            return repr == other.repr;
        }

        constexpr auto operator<=>(const Directive &other) const noexcept
        {
            return repr <=> other.repr;
        }

    public:
        constexpr Directive() noexcept = default;

        constexpr Directive(
            string_view_t repr_,
            const std::initializer_list<const TokenType> &types)
          : allowed_types(types), repr(repr_)
        {}
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

    cerb::gl::Map<string_view_t, Directive, 16> m_directives{
        { "STRING", { "\\0", { CHAR, STRING } } },
        { "CHAR", { "\\0", { CHAR, STRING } } },
        { "CHAR_TYPE", { "char", { CHAR, STRING } } },
        { "CLASS_NAME", { "", { IDENTIFIER } } },
        { "MAY_THROW", { "true", { TRUE, FALSE } } },
        { "STRING_PREFIX", { "", { CHAR, STRING, IDENTIFIER } } },
        { "STRING_POSTFIX", { "", { CHAR, STRING, IDENTIFIER } } },
        { "CHAR_PREFIX", { "", { CHAR, STRING, IDENTIFIER } } },
        { "CHAR_POSTFIX", { "", { CHAR, STRING, IDENTIFIER } } },
        { "ALLOW_COMMENTS", { "true", { TRUE, FALSE } } },
        { "MAX_TERMINALS", { "128", { INT } } },
        { "MAX_SIZE_FOR_TERMINAL", { "4", { INT } } },
        { "SINGLE_LINE_COMMENT", { "//", { CHAR, STRING } } },
        { "MULTILINE_COMMENT_BEGIN", { "/*", { CHAR, STRING } } },
        { "MULTILINE_COMMENT_END", { "*/", { CHAR, STRING } } },
        { "ALLOW_STRING_LITERALS", { "true", { TRUE, FALSE } } }
    };

    std::string generated_string{};
    static constexpr size_t MaxPriority = 64;

private:
    static constexpr auto check_token(const string_view_t &token) -> bool
    {
        return token.contains('[') != std::numeric_limits<size_t>::max();
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
                "process_block has been called, but parser can't find '[' to "
                "start BLOCK!");
            sub_state     = State::ONE;
            current_state = BLOCK;
            return;

        case State::ONE: {
            throw_if_can(
                token.type == IDENTIFIER,
                token,
                "Parser error! Unable to find IDENTIFIER for BLOCK "
                "declaration!");
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
                "Parser error! Unable to find IDENTIFIER to process "
                "declaration!");
            identifier    = token.repr;
            sub_state     = State::ONE;
            current_state = ELEM_DECLARATION;
            m_directive   = token.repr;
            break;

        case State::ONE:
            if (token.type == ASSIGN) {
                throw_if_can(
                    m_directives.contains(m_directive),
                    token,
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
                (static_cast<size_t>(token.type) & static_cast<size_t>(GENERAL)) ==
                    static_cast<size_t>(GENERAL),
                token,
                "Rule must be a string, a char or a identifier");

            if (m_directive == "STRING" || m_directive == "CHAR") {
                throw_if_can(
                    token.type == CHAR || token.type == STRING,
                    token,
                    "STRING and CHAR are unique directives. They can accept only "
                    "character");
                m_directives[m_directive].repr = token.repr;
            }

            if (check_token(token.repr) && token.type == STRING) {
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
        Directive &current_directive = m_directives[m_directive];

        switch (sub_state) {
        case State::TWO:
            throw_if_can(
                current_directive.check_type(token.type), token,
                "Unable to match type for directive.");
            current_state = NIL;
            sub_state     = State::ZERO;

            current_directive.repr = token.repr;
            m_directive            = { m_directive.begin(), m_directive.begin() };
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
            is_word = false;

            switch (token.type) {
            case ANGLE_OPENING:
                process_block(token);
                break;

            case IDENTIFIER:
                process_declaration(token);
                break;

            case EoR:
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

        return token.type != EoR;
    }

    constexpr auto syntax_error(
        const item_t &item, const string_view_t &repr, const char *message) -> void
    {
        cerb::analysis::basic_syntax_error(item, repr, message);
    }

    constexpr auto error(const item_t &item, const string_view_t &repr)
        -> void override
    {
        cerb::analysis::basic_lexical_error(
            item, repr, "Unable to find suitable dot item for: ");
    }

    constexpr auto finish() -> void override
    {
        constexpr unsigned long MinimumPower = 6;
        cerb::gl::Set<u16, bitsizeof(uintmax_t)> taken_powers{};

        if (m_directives["CLASS_NAME"].repr.size() == 0) {
            throw std::logic_error("CLASS_NAME directive must be used.");
        }

        string_view_t char_enum_name   = "CHAR";
        string_view_t string_enum_name = "STRING";

        if (m_directives["CHAR"].repr == "\\0") {
            char_enum_name = "UNDEFINED";
        }

        if (m_directives["STRING"].repr == "\\0") {
            string_enum_name = "UNDEFINED";
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
            "enum {}Blocks : size_t\n{{\n    {:<16} = 8UL,\n",
            m_directives["CLASS_NAME"].repr.to_string(), "RESERVED");

        CERBLIB_UNROLL_N(2)
        for (auto &elem : m_blocks) {
            generated_string += fmt::format(
                "    {:<16} = {}UL,\n", elem.first.to_string(),
                (1UL << elem.second.power));
        }

        generated_string += fmt::format(
            "}};\n\nenum {}Items : size_t\n{{\n    {:<16} = "
            "static_cast<size_t>(RESERVED) + 0UL,\n",
            m_directives["CLASS_NAME"].repr.to_string(), "UNDEFINED");

        size_t rules_count = 0;
        for (auto &elem : m_blocks) {
            size_t j = 0;

            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.words) {
                generated_string += fmt::format(
                    "    {:<16} = static_cast<size_t>({}) + {}UL,\n",
                    i.first.to_string(), elem.first.to_string(), j++);
            }
            rules_count += elem.second.words.size();

            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.rules) {
                generated_string += fmt::format(
                    "    {:<16} = static_cast<size_t>({}) + {}UL,\n",
                    i.first.to_string(), elem.first.to_string(), j++);
            }
            rules_count += elem.second.rules.size();

            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.operators) {
                generated_string += fmt::format(
                    "    {:<16} = static_cast<size_t>({}) + {}UL,\n",
                    i.first.to_string(), elem.first.to_string(), j++);
            }

            rules_count += elem.second.operators.size();
        }
        generated_string += "};\n\n";

        generated_string += fmt::format(
            "constexpr cerb::gl::Map<{0}Blocks, cerb::string_view, {1}> "
            "{0}BlockNames{{\n    true, {{\n",
            m_directives["CLASS_NAME"].repr.to_string(), m_blocks.size());

        CERBLIB_UNROLL_N(2)
        for (const auto &elem : m_blocks) {
            generated_string += fmt::format(
                "        {{ {0}, \"{0}\"_sv }},\n", elem.first.to_string());
        }

        generated_string += "    }\n};\n\n";

        generated_string += fmt::format(
            "constexpr cerb::gl::Map<{0}Items, cerb::string_view, {1}> "
            "{0}ItemsNames{{\n    true, {{\n",
            m_directives["CLASS_NAME"].repr.to_string(), rules_count);

        CERBLIB_UNROLL_N(2)
        for (const auto &elem : m_blocks) {
            for (const auto &i : elem.second.words) {
                generated_string += fmt::format(
                    "        {{ {0}, \"{0}\"_sv }},\n", i.first.to_string());
            }

            for (const auto &i : elem.second.rules) {
                generated_string += fmt::format(
                    "        {{ {0}, \"{0}\"_sv }},\n", i.first.to_string());
            }

            for (const auto &i : elem.second.operators) {
                generated_string += fmt::format(
                    "        {{ {0}, \"{0}\"_sv }},\n", i.first.to_string());
            }
        }

        generated_string += "    }\n};\n\n";

        generated_string += fmt::format(
            R"(
#define {0}Template                             \
    template<                                   \
        typename CharT = {1},                       \
        typename TokenType = {0}Items,              \
        bool MayThrow = {2},                        \
        size_t UID = 0,                             \
        bool AllowStringLiterals = {3},             \
        bool AllowComments = {4},                   \
        size_t MaxTerminals = {5},                  \
        size_t MaxSize4Terminals = {6}>

{0}Template
struct {0}: public CERBERUS_LEX_PARENT_CLASS
{{
    CERBERUS_LEX_PARENT_CLASS_ACCESS
)",
            m_directives["CLASS_NAME"].repr.to_string(),
            m_directives["CHAR_TYPE"].repr.to_string(),
            m_directives["MAY_THROW"].repr.to_string(),
            m_directives["ALLOW_STRING_LITERALS"].repr.to_string(),
            m_directives["ALLOW_COMMENTS"].repr.to_string(),
            m_directives["MAX_TERMINALS"].repr.to_string(),
            m_directives["MAX_SIZE_FOR_TERMINAL"].repr.to_string(),
            '/');

        generated_string += fmt::format(
            R"(
    constexpr {0}()
    : parent(
        {3}'{1}'{4},
        {3}'{2}'{4},
        {5},
        {6},
        {{)",
            m_directives["CLASS_NAME"].repr.to_string(),
            m_directives["STRING"].repr.to_string(),
            m_directives["CHAR"].repr.to_string(),
            m_directives["CHAR_PREFIX"].repr.to_string(),
            m_directives["CHAR_POSTFIX"].repr.to_string(),
            string_enum_name.to_string(), char_enum_name.to_string());

        for (auto &elem : m_blocks) {
            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.words) {
                generated_string += fmt::format(
                    "\n            {{ {}, {}\"{}\"{}, true, 2 }},",
                    i.first.to_string(),
                    m_directives["STRING_PREFIX"].repr.to_string(),
                    i.second.to_string(),
                    m_directives["STRING_POSTFIX"].repr.to_string());
            }
        }

        for (auto &elem : m_blocks) {
            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.rules) {
                generated_string += fmt::format(
                    "\n            {{ {}, {}\"{}\"{}, false, {} }},",
                    i.first.to_string(),
                    m_directives["STRING_PREFIX"].repr.to_string(),
                    i.second.to_string(),
                    m_directives["STRING_POSTFIX"].repr.to_string(),
                    elem.second.power);
            }
        }

        generated_string.pop_back();
        generated_string += "\n        },\n        {\n            { ";

        for (auto &elem : m_blocks) {
            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.operators) {
                if (i.second.size() == 1) {
                    if (i.first == "STRING" || i.first == "CHAR") {
                        continue;
                    }

                    generated_string += fmt::format(
                        "\n                {{ {}, {}\'{}\'{} }},",
                        i.first.to_string(),
                        m_directives["CHAR_PREFIX"].repr.to_string(),
                        i.second.to_string(),
                        m_directives["CHAR_POSTFIX"].repr.to_string());
                }
            }
        }

        generated_string += "\n            },\n           {";
        for (auto &elem : m_blocks) {
            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.operators) {
                if (i.second.size() > 1) {
                    if (i.first == "STRING" || i.first == "CHAR") {
                        continue;
                    }

                    generated_string += fmt::format(
                        "\n                {{ {}, {}\"{}\"{} }},",
                        i.first.to_string(),
                        m_directives["STRING_PREFIX"].repr.to_string(),
                        i.second.to_string(),
                        m_directives["STRING_POSTFIX"].repr.to_string());
                }
            }
        }

        generated_string += "\n           }\n";
        generated_string += "        },\n";
        generated_string += fmt::format(
            "        {3}\"{0}\",\n        {3}\"{1}\",\n        {3}\"{2}\"\n    ",
            m_directives["SINGLE_LINE_COMMENT"].repr.to_string(),
            m_directives["MULTILINE_COMMENT_BEGIN"].repr.to_string(),
            m_directives["MULTILINE_COMMENT_END"].repr.to_string(),
            m_directives["STRING_PREFIX"].repr.to_string());
        generated_string += ")\n    {}";
    }

    constexpr Lex4LexImpl() = default;
};


auto main(int argc, char *argv[]) -> int
{
    for (int i = 1; i < argc; ++i) {
        Lex4LexImpl lex{};

        std::string filename = argv[i];
        std::ifstream t(argv[i]);
        std::stringstream buffer{};
        buffer << t.rdbuf();
        t.close();

        std::string data = buffer.str();
        size_t offset    = data.find_first_of("%%") + 2;
        lex.scan(data.c_str() + offset, argv[i]);

        std::string rest_class = data.substr(
            static_cast<unsigned long>(lex.get_input().data() - data.c_str()));
        std::string rest = rest_class.substr(rest_class.find_last_of("%%") + 2);

        rest_class.erase(rest_class.find("%%"));
        data.erase(offset - 2);

        data += lex.get_result() + rest_class + "\n};\n" + rest;
        filename.erase(filename.find_last_of('.'));
        filename += ".hpp";

        std::ofstream out(filename);
        out << data;
        out.close();
    }

    return 0;
}