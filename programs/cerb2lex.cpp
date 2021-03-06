#include <fstream>
#include <sstream>
#include <iostream>
#include <fmt/format.h>
#include <fmt/color.h>
#include <cerberus/string_view.hpp>
#include <cerberus/analyzation/lex/lex.hpp>
#include "Lex4Lex.hpp"

using namespace cerb::literals;
using namespace std::string_literals;
using namespace std::string_view_literals;

enum Lex4LexMode : u32
{
    NORMAL,
    GENERATE_YACC,
    UPDATE_YACC,
};

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
    using item_initializer = typename parent::item_initializer;

    using enum Lex4LexBlock;
    using enum Lex4LexItem;

    template<typename T>
    static constexpr auto char_cast(T chr) noexcept -> CharT
    {
        return static_cast<CharT>(chr);
    }

    struct BlockOfItems
    {
        class BlockOfItemsNode
        {
            string_view_t m_name{};
            string_view_t m_rule{};
            std::string_view m_std_name{};
            std::string_view m_std_rule{};
            size_t m_id{};
            bool m_hidden{ false };

        public:
            CERBLIB_DECL auto name() const noexcept
            {
                return m_std_name;
            }

            CERBLIB_DECL auto rule() const noexcept
            {
                return m_std_rule;
            }

            CERBLIB_DECL auto c_name() const noexcept
            {
                return m_name;
            }

            CERBLIB_DECL auto c_rule() const noexcept
            {
                return m_rule;
            }

            CERBLIB_DECL auto id() const noexcept
            {
                return m_id;
            }

            CERBLIB_DECL auto hidden() const noexcept
            {
                return m_hidden;
            }

            constexpr auto set_id(size_t new_id) -> void
            {
                m_id = new_id;
            }

            constexpr auto hide() -> void
            {
                m_hidden = true;
            }

            constexpr auto show() -> void
            {
                m_hidden = false;
            }

            CERBLIB_DECL auto name_size() const noexcept
            {
                return m_name.size();
            }

            CERBLIB_DECL auto rule_size() const noexcept
            {
                return m_rule.size();
            }

            constexpr BlockOfItemsNode(
                string_view_t name_, string_view_t rule_, size_t id_ = 0,
                bool hidden_ = false)
              : m_name(name_), m_rule(rule_), m_std_name(name_.to_string()),
                m_std_rule(rule_.to_string()), m_id(id_), m_hidden(hidden_)
            {}
        };

        cerb::Vector<BlockOfItemsNode> words{};
        cerb::Vector<BlockOfItemsNode> rules{};
        cerb::Vector<BlockOfItemsNode> operators{};
        size_t strength{};
        string_view_t block_name{};
        size_t power{};
        size_t id{};
        bool generalized{ false };
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
        CERBLIB_DECL auto check_type(TokenType token) const noexcept -> bool
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

        CERBLIB_DECL auto to_string() const noexcept -> std::basic_string_view<CharT>
        {
            return repr.to_string();
        }

    public:
        constexpr Directive() noexcept = default;

        constexpr Directive(
            string_view_t repr_,
            const std::initializer_list<const TokenType> &types)
          : allowed_types(types), repr(repr_)
        {}
    };

public:
    Lex4LexMode mode{ NORMAL };
    std::string yacc_file{};
    std::string filename{};

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
    std::string generated_string{};
    std::string m_name_of_block{};
    std::string m_name_of_items{};

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

    static constexpr size_t MaxPriority = 64;
    cerb::gl::Map<std::string_view, size_t, 2> m_reserved_types{ { "UNDEFINED"sv,
                                                                   0 },
                                                                 { "EoF"sv, 0 } };

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

    template<typename F1, typename F2, typename F3, typename F4>
    auto for_each_rule(
        F1 &&function4reserved, F2 &&function4rules,
        F3 &&reserved_completion = cerb::empty(),
        F4 &&iteration_function  = cerb::empty()) -> void
    {
        CERBLIB_UNROLL_N(2)
        for (auto &elem : m_reserved_types) {
            function4reserved(elem);
        }
        cerb::call(std::forward<F3>(reserved_completion));

        for (auto &block : m_blocks) {
            cerb::call(std::forward<F4>(iteration_function), block);

            CERBLIB_UNROLL_N(2)
            for (auto &elem : block.second.words) {
                function4rules(0, block, elem);
            }

            CERBLIB_UNROLL_N(2)
            for (auto &elem : block.second.rules) {
                function4rules(1, block, elem);
            }

            CERBLIB_UNROLL_N(2)
            for (auto &elem : block.second.operators) {
                function4rules(2, block, elem);
            }
        }
    }

    auto set_power() -> void
    {
        constexpr unsigned long MinimumPower = 6;
        cerb::gl::Set<u16, bitsizeof(uintmax_t)> taken_powers{};

        for (auto &block : m_blocks) {
            auto power = cerb::max<unsigned long>(
                MinimumPower,
                cerb::log2(
                    block.second.words.size() + block.second.rules.size() +
                    block.second.operators.size()) +
                    1);

            CERBLIB_UNROLL_N(2)
            while (taken_powers.contains(power)) {
                throw_if_can(
                    power < bitsizeof(u64),
                    "Lexical analyzer can't hold so many items");
                ++power;
            }

            taken_powers.insert(power);
            block.second.power = power;
            block.second.id    = (1ULL << power);
        }
    }

    auto give_id() -> size_t
    {
        size_t id_offset   = 0;
        size_t rules_count = 0;

        CERBLIB_UNROLL_N(2)
        for (auto &block : m_blocks) {
            if (block.second.generalized) {
                block.second.rules.emplace_back(
                    block.first, block.first, block.second.id, true);
            }
        }

        for_each_rule(
            [&](auto &elem) {
                if (elem.first == "EoF") {
                    elem.second = 0;
                } else {
                    elem.second = parent::RESERVED + id_offset;
                }
                ++id_offset;
            },
            [&](auto /* unused */, auto &block, auto &elem) {
                if (block.first != elem.c_name()) {
                    elem.set_id(block.second.id + id_offset);
                    ++id_offset;
                }
            },
            cerb::empty(),
            [&](const auto &block) {
                id_offset =
                    static_cast<size_t>(cerb::cmov(block.second.generalized, 1, 0));
                rules_count += block.second.words.size();
                rules_count += block.second.rules.size();
                rules_count += block.second.operators.size();
            });

        return rules_count;
    }

    auto generate_block_enum() -> void
    {
        generated_string += fmt::format(
            "enum struct {} : size_t\n{{\n    {:<16} = {}UL,\n", m_name_of_block,
            "RESERVED", parent::RESERVED);

        CERBLIB_UNROLL_N(2)
        for (auto &elem : m_blocks) {
            generated_string += fmt::format(
                "    {:<16} = {}UL,\n", elem.first.to_string(), elem.second.id);
        }
        generated_string += "};\n\n";
    }

    auto generate_item_enum() -> void
    {
        generated_string +=
            fmt::format("enum struct {} : size_t\n{{\n", m_name_of_items);

        for_each_rule(
            [this](const auto &elem) {
                generated_string +=
                    fmt::format("    {:<16} = {}UL,\n", elem.first, elem.second);
            },
            [this](auto /* unused */, const auto & /*block*/, const auto &elem) {
                generated_string +=
                    fmt::format("    {:<16} = {}UL,\n", elem.name(), elem.id());
            },
            cerb::empty(), cerb::empty());
        generated_string += "};\n\n";
    }

    auto generate_comment_for_yacc() -> void
    {
        generated_string += "/*\n";
        std::string tokens_string{};
        std::string yacc_info;

        yacc_info.reserve(1024);
        tokens_string.reserve(1024);

        yacc_info = fmt::format(
            "cerb::Map<{0}, yytokentype> "
            "{1}ItemsNamesConverter(\n    {{\n",
            m_name_of_items,
            m_directives["CLASS_NAME"].to_string());

        for_each_rule(
            [&](const auto &elem) {
                tokens_string +=
                    fmt::format("%token {:<16} {}\n", elem.first, elem.second);
                yacc_info += fmt::format(
                    "        {{{0}::{1:<20}, yytokentype::{1}}},\n", m_name_of_items,
                    elem.first);
            },
            [&](auto rule, const auto &block, const auto &elem) {
                if (rule == 1) {
                    tokens_string +=
                        fmt::format("%token {:<16} {}\n", elem.name(), elem.id());
                } else {
                    if (elem.c_name() != "STRING" && elem.c_name() != "CHAR") {
                        tokens_string += fmt::format(
                            "%token {:<16} \"{}\"\n", elem.name(), elem.rule());
                    } else {
                        tokens_string += fmt::format(
                            "%token {:<16} {}\n", elem.name(), elem.id());
                    }
                }
                yacc_info += fmt::format(
                    "        {{{0}::{1:<20}, yytokentype::{2}}},\n", m_name_of_items,
                    elem.name(),
                    cerb::cmov(
                        block.second.generalized, block.first.to_string(),
                        elem.name()));
            },
            cerb::empty(), cerb::empty());
        generated_string += tokens_string;
        yacc_info += "    }\n);\n";
        generated_string += "\n\n" + yacc_info;
        generated_string += "*/\n\n";

        if ((static_cast<u32>(mode) & static_cast<u32>(GENERATE_YACC)) ==
            static_cast<u32>(GENERATE_YACC)) {
            filename.erase(filename.find_last_of('.'));
            filename += "YACC.hpp";
            std::ofstream out(filename);
            out << yacc_info;
            out.close();
        }

        if ((static_cast<u32>(mode) & static_cast<u32>(UPDATE_YACC)) ==
            static_cast<u32>(UPDATE_YACC)) {
            tokens_string.pop_back();

            std::ifstream t(yacc_file);
            std::stringstream buffer{};
            buffer << t.rdbuf();
            t.close();
            std::string yacc_data = buffer.str();
            auto first_token = yacc_data.find("%token"s);
            auto last_token  = yacc_data.rfind("%token"s);

            if (first_token == std::numeric_limits<size_t>::max()) {
                fmt::print("Error! Unable to find %token in {}\n!", yacc_file);
                exit(1);
            }

            last_token = yacc_data.find_first_of('\n', last_token);
            yacc_data.replace(
                first_token, last_token - first_token, tokens_string, 0);

            std::ofstream out(yacc_file);
            out << yacc_data;
            out.close();
            return;
        }
    }

    auto generate_block_names() -> void
    {
        generated_string += fmt::format(
            "inline cerb::Map<{0}, cerb::string_view> "
            "{1}BlockNames(\n    {{\n",
            m_name_of_block,
            m_directives["CLASS_NAME"].to_string());

        generated_string += fmt::format(
            "        {{ {0}::{1}, \"{1}\"_sv }},\n", m_name_of_block, "RESERVED");

        CERBLIB_UNROLL_N(2)
        for (const auto &elem : m_blocks) {
            generated_string += fmt::format(
                "        {{ {0}::{1}, \"{1}\"_sv }},\n", m_name_of_block,
                elem.first.to_string());
        }

        generated_string += "    }\n);\n\n";
    }

    auto generate_item_names() -> void
    {
        generated_string += fmt::format(
            "inline cerb::Map<{0}, cerb::string_view> "
            "{0}ItemsNames(\n    {{\n",
            m_name_of_items);

        for_each_rule(
            [this](const auto &elem) {
                generated_string += fmt::format(
                    "        {{ {0}::{1}, \"{1}\"_sv }},\n", m_name_of_items,
                    elem.first);
            },
            [this](auto /* unused*/, const auto & /*block*/, const auto &elem) {
                generated_string += fmt::format(
                    "        {{ {0}::{1}, \"{1}\"_sv }},\n", m_name_of_items,
                    elem.name());
            },
            cerb::empty(), cerb::empty());

        generated_string += "    }\n);\n";
    }

    auto generate_converters_and_printers() -> void
    {
        generated_string += fmt::format(
            R"(
namespace cerb::lex {{
    constexpr auto convert({0} value) -> cerb::string_view
    {{
        if ({1}BlockNames.count(value) != 0) {{
            return {1}BlockNames[value];
        }}
        return "UNDEFINED"_sv;
    }}

    constexpr auto convert({2} value) -> cerb::string_view
    {{
        if ({2}ItemsNames.count(value) != 0) {{
            return {2}ItemsNames[value];
        }}
        return "UNDEFINED"_sv;
    }}
}}

template<typename T>
auto operator<<(T &os, {0} value) -> T &
{{
    os << cerb::lex::convert(value);
    return os;
}}

template<typename T>
auto operator<<(T &os, {2} value) -> T &
{{
    os << cerb::lex::convert(value);
    return os;
}}

)",
            m_name_of_block, m_directives["CLASS_NAME"].to_string(),
            m_name_of_items);
    }

    auto generate_defines_and_class_declaration() -> void
    {
        generated_string += fmt::format(
            R"(

namespace cerb {{
    constexpr auto convert({7} value) -> cerb::string_view
    {{
        if ({7}ItemsNames.count(value) != 0) {{
            return {7}ItemsNames[value];
        }}

        return "";
    }}
}}

#define {0}Template                             \
    template<                                   \
        typename CharT = {1},                       \
        typename TokenType = {7},              \
        bool MayThrow = {2},                        \
        size_t UID = 0,                             \
        bool AllowStringLiterals = {3},             \
        bool AllowComments = {4},                   \
        size_t MaxTerminals = {5},                  \
        size_t MaxSize4Terminals = {6}>

#define {0}Definition(CLASS) CalculatorTemplate struct CLASS : public {0}<>

#define {0}Access using {0}<>::parent;               \
        using parent::head;                                                         \
        using item_t           = typename parent::item_t;                           \
        using storage_t        = typename parent::storage_t;                        \
        using token_t          = typename parent::token_t;                          \
        using result_t         = typename parent::result_t;                         \
        using position_t       = typename parent::position_t;                       \
        using string_t         = typename parent::string_t;                         \
        using string_view_t    = typename parent::string_view_t;                    \
        using string_checker_t = typename parent::string_checker_t;                 \
        using item_initializer  = typename parent::item_initializer;                \
        using enum {7};

{0}Template
struct {0}: public CERBERUS_LEX_PARENT_CLASS
{{
    CERBERUS_LEX_PARENT_CLASS_ACCESS
    using enum {7};
)",
            m_directives["CLASS_NAME"].to_string(),
            m_directives["CHAR_TYPE"].to_string(),
            m_directives["MAY_THROW"].to_string(),
            m_directives["ALLOW_STRING_LITERALS"].to_string(),
            m_directives["ALLOW_COMMENTS"].to_string(),
            m_directives["MAX_TERMINALS"].to_string(),
            m_directives["MAX_SIZE_FOR_TERMINAL"].to_string(),
            m_name_of_items);
    }

    auto generate_class_body(
        const string_view_t &char_enum_name, const string_view_t &string_enum_name)
        -> void
    {
        generated_string += fmt::format(
            R"(
    constexpr {0}()
    : parent(
        {3}'{1}'{4},
        {3}'{2}'{4},
        {5},
        {6},
        {{)",
            m_directives["CLASS_NAME"].to_string(),
            m_directives["STRING"].to_string(), m_directives["CHAR"].to_string(),
            m_directives["CHAR_PREFIX"].to_string(),
            m_directives["CHAR_POSTFIX"].to_string(), string_enum_name.to_string(),
            char_enum_name.to_string());

        std::string words_str{};
        std::string rules_str{};
        std::string operators1{};
        std::string operators2{};

        words_str.reserve(512);
        rules_str.reserve(512);
        operators1.reserve(512);
        operators2.reserve(512);

        for (auto &elem : m_blocks) {
            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.words) {
                words_str += fmt::format(
                    "\n            {{ {}, {}\"{}\"{}, true, 2 }},",
                    i.name(),
                    m_directives["STRING_PREFIX"].to_string(),
                    i.rule(),
                    m_directives["STRING_POSTFIX"].to_string());
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.rules) {
                rules_str += fmt::format(
                    "\n            {{ {}, {}\"{}\"{}, false, {} }},",
                    i.name(),
                    m_directives["STRING_PREFIX"].to_string(),
                    i.rule(),
                    m_directives["STRING_POSTFIX"].to_string(),
                    elem.second.power);
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &i : elem.second.operators) {
                if (i.c_name() == "STRING" || i.c_name() == "CHAR") {
                    continue;
                }

                if (i.rule_size() == 1) {
                    operators1 += fmt::format(
                        "\n                {{ {}, {}\'{}\'{} }},",
                        i.name(),
                        m_directives["CHAR_PREFIX"].to_string(),
                        i.rule(),
                        m_directives["CHAR_POSTFIX"].to_string());
                } else if (i.rule_size() > 1) {
                    operators2 += fmt::format(
                        "\n                {{ {}, {}\"{}\"{} }},",
                        i.name(),
                        m_directives["STRING_PREFIX"].to_string(),
                        i.rule(),
                        m_directives["STRING_POSTFIX"].to_string());
                }
            }
        }

        generated_string += words_str + rules_str;
        generated_string.pop_back();
        generated_string += "\n        },\n        {\n            { ";
        generated_string += operators1;

        generated_string += "\n            },\n           {";
        generated_string += operators2;

        generated_string += "\n           }\n";
        generated_string += "        },\n";
        generated_string += fmt::format(
            "        {3}\"{0}\",\n        {3}\"{1}\",\n        {3}\"{2}\"\n    ",
            m_directives["SINGLE_LINE_COMMENT"].to_string(),
            m_directives["MULTILINE_COMMENT_BEGIN"].to_string(),
            m_directives["MULTILINE_COMMENT_END"].to_string(),
            m_directives["STRING_PREFIX"].to_string());
        generated_string += ")\n    {}";
    }

public:
    CERBLIB_DECL auto get_result() const noexcept -> const std::string &
    {
        return generated_string;
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

            case GENERALIZED:
                current_block->generalized = true;
                break;

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

    auto finish() -> void override
    {
        string_view_t char_enum_name   = "CHAR";
        string_view_t string_enum_name = "STRING";

        if (m_directives["CLASS_NAME"].repr.size() == 0) {
            throw std::logic_error("CLASS_NAME directive must be used.");
        }

        m_name_of_block =
            std::string(m_directives["CLASS_NAME"].to_string()) + "Block";
        m_name_of_items =
            std::string(m_directives["CLASS_NAME"].to_string()) + "Item";

        if (m_directives["CHAR"].repr == "\\0") {
            char_enum_name = "UNDEFINED";
        }

        if (m_directives["STRING"].repr == "\\0") {
            string_enum_name = "UNDEFINED";
        }

        set_power();
        size_t rules_count = give_id();
        generate_block_enum();
        generate_item_enum();
        generate_comment_for_yacc();
        generate_block_names();
        generate_item_names();
        generate_converters_and_printers();
        generate_defines_and_class_declaration();
        generate_class_body(char_enum_name, string_enum_name);
    }

    Lex4LexImpl()
    : Lex4Lex::Lex4Lex()
    {
        generated_string.reserve(16384);
    }
};

auto generate_file(Lex4LexImpl<> &lex, std::string &filename) -> void
{
    lex.filename = filename;
    std::ifstream t(filename);
    std::stringstream buffer{};
    buffer << t.rdbuf();
    t.close();

    std::string data = buffer.str();
    size_t offset    = data.find_first_of("%%") + 2;
    lex.scan(data.c_str() + offset, filename.c_str());

    std::string rest_class = data.substr(
        static_cast<unsigned long>(lex.get_input().data() - data.c_str()));
    std::string rest = rest_class.substr(rest_class.find_last_of("%%") + 2);

    rest_class.erase(rest_class.find("%%"));
    data.erase(offset - 2);

    data.append(lex.get_result());
    data.append(rest_class);
    data.append("\n};\n");
    data.append(rest);

    filename.erase(filename.find_last_of('.'));
    filename += ".hpp";

    std::ofstream out(filename);
    out << data;
    out.close();
}

auto main(int argc, char *argv[]) -> int
{
    CERBLIB_UNROLL_N(1)
    for (int i = 1; i < argc; ++i) {
        Lex4LexImpl lex{};

        std::string filename = argv[i];
        if (filename == "-YACC") {
            lex.mode = GENERATE_YACC;
            filename = argv[++i];
            generate_file(lex, filename);
        } else if (filename == "-UPDATE") {
            lex.mode      = UPDATE_YACC;
            lex.yacc_file = argv[++i];
            filename      = argv[++i];
            generate_file(lex, filename);
        } else if (filename == "-UPDATE+") {
            lex.mode = static_cast<Lex4LexMode>(
                static_cast<u32>(UPDATE_YACC) | static_cast<u32>(GENERATE_YACC));
            lex.yacc_file = argv[++i];
            filename      = argv[++i];
            generate_file(lex, filename);
        } else if (filename == "-h" || filename == "--help") {
            fmt::print(
                R"(Welcome to cerb4lex generator!
cerb4lex can generate rules for cerberus lexical analyzer and help you to integrate lexical analyzer to yacc.
Available command:
    1) -h, --help to get some help
    2) -YACC generates header file with yacc tokens converter (cerberus tokens to yacc)
    3) -UPDATE updates next file (yacc file) from second file (cerberus rule)
        yacc file must contain at least one %token and tokens must be written together in order not to cause any damage to the file
    4) -UPDATE+ UPDATE and YACC together
    5) In normal mode cerb4lex generates header file from given rule
)");
        } else {
            generate_file(lex, filename);
        }
    }

    return 0;
}