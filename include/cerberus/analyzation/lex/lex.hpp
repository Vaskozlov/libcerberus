#ifndef CERBERUS_LEX_HPP
#define CERBERUS_LEX_HPP

#include <cerberus/deque.hpp>
#include <cerberus/analyzation/lex/dot_item.hpp>
#include <cerberus/analyzation/exceptions.hpp>

#define CERBERUS_LEX_TEMPLATES                                                      \
    template<                                                                       \
        typename CharT           = char,                                            \
        typename TokenType       = unsigned,                                        \
        bool MayThrow            = true,                                            \
        size_t UID               = 0,                                               \
        bool AllowStringLiterals = true,                                            \
        bool AllowComments       = true,                                            \
        size_t MaxTerminals      = 128,                                             \
        size_t MaxSize4Terminals = 8>

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
    using item_t           = typename parent::item_t;                               \
    using storage_t        = typename parent::storage_t;                            \
    using token_t          = typename parent::token_t;                              \
    using result_t         = typename parent::result_t;                             \
    using position_t       = typename parent::position_t;                           \
    using string_view_t    = typename parent::string_view_t;                        \
    using string_checker_t = typename parent::string_checker_t;                     \
    using item_initilizer  = typename parent::item_initilizer;


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
        using item_t = DotItem<
            CharT,
            TokenType,
            MayThrow,
            UID,
            AllowStringLiterals,
            AllowComments,
            MaxTerminals,
            MaxSize4Terminals>;

        using storage_t        = Set<item_t, MayThrow, less<void>>;
        using token_t          = typename item_t::token_t;
        using result_t         = typename item_t::result_t;
        using position_t       = typename item_t::position_t;
        using string_view_t    = typename item_t::string_view_t;
        using string_checker_t = typename item_t::string_checker_t;
        using item_initilizer  = typename item_t::DotItemInitializer;

        enum PriorityLevel
        {
            LOW    = 2,
            MEDIUM = 1,
            HIGH   = 0
        };

        storage_t m_items{};
        Deque<std::basic_string<CharT>, 8, MayThrow> m_strings{};
        typename storage_t::iterator m_head{ nullptr };
        const TokenType m_string_type{};
        const TokenType m_char_type{};
        const CharT m_string_separator{};
        const CharT m_char_separator{};

        static constexpr gl::Map<CharT, u16, 22, MayThrow> hex_chars{
            { static_cast<CharT>('0'), 0 },  { static_cast<CharT>('1'), 1 },
            { static_cast<CharT>('2'), 2 },  { static_cast<CharT>('3'), 3 },
            { static_cast<CharT>('4'), 4 },  { static_cast<CharT>('5'), 5 },
            { static_cast<CharT>('6'), 6 },  { static_cast<CharT>('7'), 7 },
            { static_cast<CharT>('8'), 8 },  { static_cast<CharT>('9'), 9 },
            { static_cast<CharT>('a'), 10 }, { static_cast<CharT>('b'), 11 },
            { static_cast<CharT>('c'), 12 }, { static_cast<CharT>('d'), 13 },
            { static_cast<CharT>('e'), 14 }, { static_cast<CharT>('f'), 15 },
            { static_cast<CharT>('A'), 10 }, { static_cast<CharT>('B'), 11 },
            { static_cast<CharT>('C'), 12 }, { static_cast<CharT>('D'), 13 },
            { static_cast<CharT>('E'), 14 }, { static_cast<CharT>('F'), 15 }
        };

    private:
        constexpr auto process_char(
            CharT separator,
            size_t index,
            item_t &item,
            std::basic_string<CharT> &result) -> Pair<bool, size_t>
        {
            switch (item.get_char(index)) {
            case item_t::char_cast('\\'):
                switch (item_t::char_cast(item.get_char(index + 1))) {
                case item_t::char_cast('0'): {
                    index += 2;
                    ByteMask<CharT> mask{ 0 };

                    CERBLIB_UNROLL_N(2)
                    for (size_t j = 0; j < 2; ++j) {
                        if (item.get_char(index) >= item_t::char_cast('0') &&
                            item.get_char(index) <= item_t::char_cast('7')) {
                            mask.getAsIntegral() <<= 3;
                            mask.getAsIntegral() +=
                                item.get_char() - item_t::char_cast('0');
                        } else {
                            if (j == 1 && mask.getAsIntegral() == 0) {
                                break;
                            }
                            throw_if_can(false, "Unable to recognize char!");
                        }
                        index++;
                    }
                    result.push_back(mask.getAsIntegral());
                } break;

                case item_t::char_cast('x'): {
                    index += 2;
                    ByteMask<CharT> mask{ 0 };

                    CERBLIB_UNROLL_N(2)
                    for (size_t j = 0; j < 2; ++j) {
                        if (hex_chars.contains(item.get_char(index))) {
                            mask.getAsIntegral() <<= 4;
                            mask.getAsIntegral() += hex_chars[item.get_char(index)];
                        } else {
                            throw_if_can(false, "Unable to recognize char!");
                        }
                        index++;
                    }
                    result.push_back(mask.getAsIntegral());
                } break;

                case item_t::char_cast('u'): {
                    index += 2;
                    ByteMask<CharT> mask{ 0 };

                    CERBLIB_UNROLL_N(2)
                    for (size_t j = 0; j < 4; ++j) {
                        if (hex_chars.contains(item.get_char(index))) {
                            mask.getAsIntegral() <<= 4;
                            mask.getAsIntegral() +=
                                (hex_chars[item.get_char(index)]);
                        } else {
                            throw_if_can(false, "Unable to recognize char!");
                        }
                        index++;
                    }
                    result.push_back(mask.getAsIntegral());
                } break;

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
                        false,
                        "Unable to recognize action after '\\' in string token");
                }
                break;

            default:
                if (item.get_char(index) == separator) {
                    return { true, index + 1 };
                }

                result.push_back(item.get_char(index++));
                break;
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

    public:
        constexpr virtual void finish()                    = 0;
        constexpr virtual bool yield(const token_t &token) = 0;
        constexpr virtual void
            error(const item_t &item, const string_view_t &repr) = 0;

        [[nodiscard]] constexpr auto head() const noexcept
        {
            return m_head;
        }

        constexpr virtual auto process_string(item_t &item)
            -> Pair<size_t, std::basic_string<CharT>>
        {
            throw_if_can(
                item.get_char() == item_t::char_cast('"'),
                "process string has been called, but input does not contain any "
                "strings");

            size_t index = 1U;
            std::basic_string<CharT> result;

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
                    return { index + 1, std::move(result) };
                }
            }

            return { index + 1, std::move(result) };
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
            auto &head = *m_items.begin();
            head.set_input(input, filename);
            head.skip_comments_and_layout();
            head.dump();

            size_t times = 0;
            item_t *item = nullptr;

            while (!item_t::empty()) {
                ItemState state = UNABLE_TO_MATCH;
                head.rebind();

                if constexpr (AllowStringLiterals) {
                    if (m_string_separator != item_t::char_cast(0) &&
                        head.get_char() == m_string_separator) {
                        auto result = process_string(head);
                        m_strings.emplace_back(result.second);
                        auto &str = m_strings.back();
                        token_t token{ { str.data(), str.size() },
                                       m_string_type,
                                       head.get_token_pos() };

                        if (!yield(token)) {
                            finish();
                            return;
                        }

                        head.add2input(result.first);
                        head.skip_comments_and_layout();
                        head.dump();
                        times = 0;
                        continue;
                    }
                    if (m_char_separator != item_t::char_cast(0) &&
                        head.get_char() == m_char_separator) {
                        std::basic_string<CharT> chars;
                        auto result = process_char(m_char_separator, 1, head, chars);
                        throw_if_can(
                            head.get_char(result.second) + 1 != m_char_separator,
                            "Char can contain only one elem");

                        m_strings.emplace_back(chars);
                        auto &str = m_strings.back();
                        token_t token{ { str.data(), str.size() },
                                       m_char_type,
                                       head.get_token_pos() };

                        if (!yield(token)) {
                            finish();
                            return;
                        }

                        head.add2input(result.second + 1);
                        head.skip_comments_and_layout();
                        head.dump();
                        times = 0;
                        continue;
                    }
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
                    head.skip_comments_and_layout();
                    head.dump();
                    ++times;

                    if (times > 1) {
                        error(head, head.isolate_token());
                    }
                }
                item = nullptr;
            }
            finish();
        }

    public:
        constexpr virtual ~LexicalAnalyzer() = default;

        constexpr LexicalAnalyzer(
            const CharT string_separator,
            const CharT char_separator,
            const TokenType string_type,
            const TokenType char_type,
            const std::initializer_list<const item_initilizer>
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
