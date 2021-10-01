#ifndef CERBERUS_LEX_HPP
#define CERBERUS_LEX_HPP

#include <cerberus/deque.hpp>
#include <cerberus/lex/dot_item.hpp>

#define CERBERUS_LEX_TEMPLATES                                                      \
    template<                                                                       \
        typename CharT,                                                             \
        typename TokenType,                                                         \
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
    using token_t          = typename parent::token_t;                              \
    using result_t         = typename parent::result_t;                             \
    using position_t       = typename parent::position_t;                           \
    using string_view_t    = typename parent::string_view_t;                        \
    using string_checker_t = typename parent::string_checker_t;                     \
    using item_initilizer  = typename parent::item_initilizer;

#define CERBERUS_LEX_INITIALIZER(class_name)                                        \
    class_name(                                                                     \
        const std::initializer_list<const item_initilizer> keywords,                \
        const std::initializer_list<const item_initilizer>                          \
            rules,                                                                  \
        const string_checker_t &terminals,                                          \
        const string_view_t &single_line_comment     = "//",                        \
        const string_view_t &multiline_comment_begin = "/*",                        \
        const string_view_t &multiline_comment_end   = "*/")                        \
      : parent(                                                                     \
            keywords,                                                               \
            rules,                                                                  \
            terminals,                                                              \
            single_line_comment,                                                    \
            multiline_comment_begin,                                                \
            multiline_comment_end)

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

        Set<item_t, MayThrow> m_items{};
        Deque<Vector<CharT>, 8, MayThrow> m_strings{};

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

    public:
        static constexpr auto throw_if_can(bool condition, const char *message)
            -> void
        {
            if constexpr (MayThrow) {
                if (!condition) {
                    throw std::runtime_error(message);
                }
            }
        }

        constexpr virtual void yield(const token_t &token) = 0;
        constexpr virtual void error(
            const position_t &pos,
            const string_view_t &line,
            const string_view_t &repr) = 0;
        constexpr virtual auto process_string(item_t &item)
            -> Pair<size_t, Vector<CharT>>
        {
            throw_if_can(
                item.get_char() == item_t::char_cast('"'),
                "process string has been called, but input does not contain any "
                "strings");

            size_t index = 1U;
            Vector<CharT> result;

            while (item.get_char(index) != item_t::char_cast('"')) {
                throw_if_can(
                    item.get_char(index) != item_t::char_cast('\0'),
                    "End of file reached, however end of string hasn't found");
                throw_if_can(
                    !(item.get_char(index) == item_t::char_cast('\n') &&
                      item.get_char(index - 1) == item_t::char_cast('\\')),
                    "String hasn't been closed on a line");

                switch (item.get_char(index)) {
                case item_t::char_cast('\\'):
                    switch (item_t::char_cast(item.get_char(index + 1))) {
                    case item_t::char_cast('0'): {
                        index += 2;
                        ByteMask<CharT> mask{ 0 };

                        CERBLIB_UNROLL_N(2) for (size_t j = 0; j < 2; ++j)
                        {
                            if (item.get_char(index) >= item_t::char_cast('0') &&
                                item.get_char(index) <= item_t::char_cast('7')) {
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
                    } break;

                    case item_t::char_cast('x'): {
                        index += 2;
                        ByteMask<CharT> mask{ 0 };

                        CERBLIB_UNROLL_N(2)
                        for (size_t j = 0; j < 2; ++j) {
                            if (hex_chars.contains(item.get_char(index))) {
                                mask.getAsIntegral() +=
                                    hex_chars[item.get_char(index)];
                            } else {
                                throw_if_can(false, "Unable to recognize char!");
                            }
                            index++;
                        }
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

                    case item_t::char_cast('\\'):
                        index += 2;
                        result.push_back(item_t::char_cast('\\'));
                        break;

                    default:
                        throw_if_can(
                            false,
                            "Unable to recognize action after '\\' in string token");
                    }

                case item_t::char_cast('"'):
                    return { index + 1, std::move(result) };

                default:
                    result.push_back(item.get_char(index++));
                    break;
                }
            }

            return { index + 1, std::move(result) };
        }

    public:
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
                    if (head.get_char() == item_t::char_cast('"')) {
                        auto result = process_string(head);
                        m_strings.push_back(std::move(result.second));
                        auto &str = m_strings.back();
                        token_t token{ { str.data(), str.size() },
                                       static_cast<TokenType>(100),
                                       head.get_token_pos() };
                        yield(token);
                        head.add2input(result.first);
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
                        yield(result);
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
                        error(
                            head.get_token_pos(),
                            head.get_line(),
                            head.isolate_token());
                    }
                }
                item = nullptr;
            }
        }

    public:
        constexpr virtual ~LexicalAnalyzer() = default;

        constexpr LexicalAnalyzer(
            const std::initializer_list<const item_initilizer>
                keywords,
            const std::initializer_list<const item_initilizer>
                rules,
            const string_checker_t &terminals,
            const string_view_t &single_line_comment     = "//",
            const string_view_t &multiline_comment_begin = "/*",
            const string_view_t &multiline_comment_end   = "*/")
        {
            CERBLIB_UNROLL_N(2)
            for (auto &elem : keywords) {
                item_initilizer item = elem;
                item.word            = true;
                item.priority        = HIGH;
                m_items.emplace(item);
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &elem : rules) {
                m_items.emplace(elem);
            }

            item_t::set_terminals(terminals);
            item_t::set_comments(
                single_line_comment, multiline_comment_begin, multiline_comment_end);
        }
    };
}// namespace cerb::lex::experimental

#endif /* CERBERUS_LEX_HPP */
