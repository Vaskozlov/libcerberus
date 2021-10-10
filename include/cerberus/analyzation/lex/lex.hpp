#ifndef CERBERUS_LEX_HPP
#define CERBERUS_LEX_HPP

#include <cerberus/deque.hpp>
#include <cerberus/analyzation/lex/dot_item.hpp>
#include <cerberus/analyzation/exceptions.hpp>

#define CERBERUS_LEX_PARENT_CLASS                                                   \
    cerb::lex::experimental::LexicalAnalyzer<                                       \
        CharT,                                                                      \
        TokenType,                                                                  \
        MayThrow,                                                                   \
        UID,                                                                        \
        AllowStringLiterals,                                                        \
        AllowComments,                                                              \
        MaxTerminals,                                                               \
        MaxSize4Terminals>

#define CERBERUS_LEX_PARENT_CLASS_ACCESS                                            \
    using parent = cerb::lex::experimental::LexicalAnalyzer<                        \
        CharT,                                                                      \
        TokenType,                                                                  \
        MayThrow,                                                                   \
        UID,                                                                        \
        AllowStringLiterals,                                                        \
        AllowComments,                                                              \
        MaxTerminals,                                                               \
        MaxSize4Terminals>;                                                         \
    using parent::m_items;                                                          \
    using parent::head;                                                             \
    using item_t             = typename parent::item_t;                             \
    using storage_t          = typename parent::storage_t;                          \
    using token_t            = typename parent::token_t;                            \
    using result_t           = typename parent::result_t;                           \
    using position_t         = typename parent::position_t;                         \
    using string_t           = typename parent::string_t;                           \
    using string_view_t      = typename parent::string_view_t;                      \
    using string_checker_t   = typename parent::string_checker_t;                   \
    using item_initializer   = typename parent::item_initializer;                   \
    using string_container_t = typename parent::string_container_t;

namespace cerb::lex::experimental {
    template<
        typename CharT,
        typename TokenType,
        bool MayThrow            = true,
        size_t UID               = 0,
        bool AllowStringLiterals = true,
        bool AllowComments       = true,
        size_t MaxTerminals      = 128,
        size_t MaxSize4Terminals = 4>
    struct LexicalAnalyzer
    {
        template<typename T>
        static constexpr auto char_cast(T chr) noexcept -> CharT
        {
            return static_cast<CharT>(chr);
        }

        using item_t = DotItem<
            CharT,
            TokenType,
            MayThrow,
            UID,
            AllowStringLiterals,
            AllowComments,
            MaxTerminals,
            MaxSize4Terminals>;

        using storage_t          = Set<item_t, MayThrow, less<void>>;
        using token_t            = typename item_t::token_t;
        using result_t           = typename item_t::result_t;
        using position_t         = typename item_t::position_t;
        using string_view_t      = typename item_t::string_view_t;
        using string_checker_t   = typename item_t::string_checker_t;
        using item_initializer   = typename item_t::DotItemInitializer;
        using storage_iterator   = typename storage_t::iterator;
        using string_t           = std::basic_string<CharT>;
        using string_container_t = Deque<string_t, 8, MayThrow>;

        enum PriorityLevel
        {
            LOW    = 2,
            MEDIUM = 1,
            HIGH   = 0
        };

        storage_t m_items{};
        string_container_t m_strings{};
        storage_iterator m_head{ nullptr };
        const TokenType m_string_type{};
        const TokenType m_char_type{};
        const CharT m_string_separator{};
        const CharT m_char_separator{};
        constexpr static string_view_t repr4EoF = "$";

    private:
        enum StringScanState
        {
            CONTINUE,
            FINISH,
            NONE
        };

        static constexpr gl::Map<CharT, u8, 22, MayThrow> hex_chars{
            { char_cast('0'), 0 },  { char_cast('1'), 1 },  { char_cast('2'), 2 },
            { char_cast('3'), 3 },  { char_cast('4'), 4 },  { char_cast('5'), 5 },
            { char_cast('6'), 6 },  { char_cast('7'), 7 },  { char_cast('8'), 8 },
            { char_cast('9'), 9 },  { char_cast('a'), 10 }, { char_cast('b'), 11 },
            { char_cast('c'), 12 }, { char_cast('d'), 13 }, { char_cast('e'), 14 },
            { char_cast('f'), 15 }, { char_cast('A'), 10 }, { char_cast('B'), 11 },
            { char_cast('C'), 12 }, { char_cast('D'), 13 }, { char_cast('E'), 14 },
            { char_cast('F'), 15 }
        };
        static constexpr size_t RESERVED = 8UL;
        static constexpr size_t EoF      = RESERVED + 1UL;

    private:
        template<size_t CharCount, size_t Mul, bool BreakOnZero, typename Func>
        static constexpr auto process_char_as_int(
            Func &&func,
            size_t index,
            item_t &item,
            string_t &result) -> size_t
        {
            index += 2;
            ByteMask<CharT> mask{ char_cast('\0') };

            CERBLIB_UNROLL_N(2)
            for (size_t j = 0; j < CharCount; ++j) {
                if (func(item.get_char(index))) {
                    mask.getAsIntegral() *= Mul;
                    mask.getAsIntegral() +=
                        to_unsigned(hex_chars[item.get_char(index)]);
                } else {
                    if constexpr (BreakOnZero) {
                        if (j == 0) {
                            break;
                        }
                    }
                    throw_if_can(false, "Unable to recognize char!");
                }
                index++;
            }
            result.push_back(bit_cast<CharT>(mask.getAsIntegral()));
            return index;
        }

        static constexpr auto
            process_unique_char(size_t index, item_t &item, string_t &result)
                -> size_t
        {
            switch (char_cast(item.get_char(index + 1))) {
            case char_cast('0'):
                index = process_char_as_int<2, 8, true>(
                    [](const auto &chr) {
                        return chr >= char_cast('0') && chr <= char_cast('7');
                    },
                    index,
                    item,
                    result);
                break;

            case item_t::char_cast('x'):
                index = process_char_as_int<2, 16, true>(
                    [](const auto &chr) { return hex_chars.contains(chr); },
                    index,
                    item,
                    result);
                break;

            case item_t::char_cast('u'):
                index = process_char_as_int<4, 16, true>(
                    [](const auto &chr) { return hex_chars.contains(chr); },
                    index,
                    item,
                    result);
                break;

            case item_t::char_cast('n'):
                index += 2;
                result.push_back(item_t::char_cast('\n'));
                break;

            case item_t::char_cast('r'):
                index += 2;
                result.push_back(item_t::char_cast('\r'));
                break;

            case item_t::char_cast('t'):
                index += 2;
                result.push_back(item_t::char_cast('\t'));
                break;

            case item_t::char_cast('\''):
                index += 2;
                result.push_back(item_t::char_cast('\''));
                break;

            case item_t::char_cast('\"'):
                index += 2;
                result.push_back(item_t::char_cast('\"'));
                break;

            case item_t::char_cast('\\'):
                index += 2;
                result.push_back(item_t::char_cast('\\'));
                break;

            default:
                throw_if_can(
                    false, "Unable to recognize action after '\\' in string token");
            }
            return index;
        }

        static constexpr auto process_char(
            CharT separator,
            size_t index,
            item_t &item,
            string_t &result) -> Pair<bool, size_t>
        {
            if (item.get_char(index) == char_cast('\\')) {
                index = process_unique_char(index, item, result);
            } else {
                if (item.get_char(index) == separator) {
                    return { true, index + 1 };
                }
                result.push_back(item.get_char(index++));
            }
            return { false, index };
        }

        static constexpr auto throw_if_can(bool condition, const char *message)
            -> void
        {
            if constexpr (MayThrow) {
                if (!condition) {
                    throw std::runtime_error(message);
                }
            }
        }

        constexpr auto
            throw_if_can(bool condition, const token_t &token, const char *message)
                -> void
        {
            if constexpr (MayThrow) {
                if (!condition) {
                    analysis::basic_syntax_error(*head(), token.repr, message);
                }
            }
        }

        constexpr auto evaluate_string() -> bool
        {
            m_strings.emplace_back();
            auto &str   = m_strings.back();
            auto result = process_string(*head(), str);
            token_t token{ { str.data(), str.size() },
                           cmov(
                               m_char_separator == m_string_separator &&
                                   str.size() == 1,
                               m_char_type,
                               m_string_type),
                           head()->get_token_pos() };

            if (!yield(token)) {
                finish();
                return false;
            }

            head()->add2input(result);
            head()->skip_comments_and_layout();
            head()->dump();
            return true;
        }

        constexpr auto evaluate_char() -> bool
        {
            m_strings.emplace_back();
            auto result =
                process_char(m_char_separator, 1, *head(), m_strings.back());

            auto &str = m_strings.back();
            token_t token{ { str.data(), str.size() },
                           m_char_type,
                           head()->get_token_pos() };

            throw_if_can(
                head()->get_char(result.second) == m_char_separator,
                token,
                "Char can contain only one elem");

            if (!yield(token)) {
                finish();
                return false;
            }

            head()->add2input(result.second + 1);
            head()->skip_comments_and_layout();
            head()->dump();

            return true;
        }

        constexpr auto manage_char_and_string() -> StringScanState
        {
            if constexpr (AllowStringLiterals) {
                if (m_string_separator != item_t::char_cast(0) &&
                    head()->get_char() == m_string_separator) {
                    return cmov(evaluate_string(), CONTINUE, FINISH);
                }
                if (m_char_separator != item_t::char_cast(0) &&
                    head()->get_char() == m_char_separator) {
                    return cmov(evaluate_char(), CONTINUE, FINISH);
                }
            }
            return NONE;
        }

    public:
        constexpr virtual void finish()                    = 0;
        constexpr virtual bool yield(const token_t &token) = 0;
        constexpr virtual void
            error(const item_t &item, const string_view_t &repr) = 0;

        [[nodiscard]] constexpr auto head() const noexcept
        {
            return m_head;
        }

        constexpr virtual auto process_string(item_t &item, string_t &result)
            -> size_t
        {
            throw_if_can(
                item.get_char() == item_t::char_cast('"'),
                "process string has been called, but input does not contain any "
                "strings");

            size_t index = 1U;

            CERBLIB_UNROLL_N(2)
            while (item.get_char(index) != item_t::char_cast('"')) {
                throw_if_can(
                    item.get_char(index) != item_t::char_cast('\0'),
                    "End of file reached, however end of string hasn't found");
                throw_if_can(
                    !(item.get_char(index) == item_t::char_cast('\n') &&
                      item.get_char(index - 1) == item_t::char_cast('\\')),
                    "String hasn't been closed on a line");

                auto result_of_process =
                    process_char(m_string_separator, index, item, result);
                index = result_of_process.second;

                if (result_of_process.first) {
                    return index + 1;
                }
            }

            return index + 1;
        }

    public:
        [[nodiscard]] constexpr auto get_input() const noexcept
            -> const string_view_t &
        {
            return head()->get_input();
        }

        constexpr auto
            scan(const string_view_t &input, const string_view_t &filename)
        {
            head()->set_input(input, filename);
            head()->skip_comments_and_layout();
            head()->dump();

            size_t times = 0;
            item_t *item = nullptr;

            while (!item_t::empty()) {
                ItemState state = UNABLE_TO_MATCH;
                head()->rebind();
                auto tmp = manage_char_and_string();

                if (tmp == FINISH) {
                    return;
                }
                if (tmp == CONTINUE) {
                    times = 0;
                    continue;
                }

                for (auto &elem : m_items) {
                    elem.rebind();

                    CERBLIB_UNROLL_N(2)
                    do {
                        state = elem.check();
                    } while (state != UNABLE_TO_MATCH && state != SCAN_FINISHED);

                    if (state == SCAN_FINISHED &&
                        (item == nullptr || elem.dot() > item->dot())) {
                        item = &elem;
                        break;
                    }
                }

                if (item != nullptr) {
                    CERBLIB_UNROLL_N(1)
                    for (auto &result : item->result()) {
                        if (!yield(result)) {
                            finish();
                            return;
                        }
                    }

                    item->add2input(item->dot());
                    item->skip_comments_and_layout();
                    item->dump();
                    times = 0;
                } else {
                    head()->skip_comments_and_layout();
                    head()->dump();
                    ++times;

                    if (times > 1) {
                        error(*head(), head()->isolate_token());
                    }
                }
                item = nullptr;
            }
            yield(
                { repr4EoF, static_cast<TokenType>(EoF), head()->get_token_pos() });
            finish();
        }

    public:
        constexpr virtual ~LexicalAnalyzer() = default;

        constexpr LexicalAnalyzer(
            const CharT string_separator,
            const CharT char_separator,
            const TokenType string_type,
            const TokenType char_type,
            const std::initializer_list<const item_initializer>
                rules,
            const string_checker_t &terminals,
            const string_view_t &single_line_comment,
            const string_view_t &multiline_comment_begin,
            const string_view_t &multiline_comment_end)
          : m_items(rules), m_head(m_items.begin()), m_string_type(string_type),
            m_char_type(char_type), m_string_separator(string_separator),
            m_char_separator(char_separator)
        {
            item_t::set_terminals(terminals);
            item_t::set_comments(
                single_line_comment, multiline_comment_begin, multiline_comment_end);
        }
    };
}// namespace cerb::lex::experimental

#endif /* CERBERUS_LEX_HPP */
