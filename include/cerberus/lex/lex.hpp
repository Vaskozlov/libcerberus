#ifndef CERBERUS_LEX_HPP
#define CERBERUS_LEX_HPP

#include <cerberus/lex/dot_item.hpp>

#define CERBERUS_LEX_TEMPLATES                                                      \
    template<                                                                       \
        typename CharT,                                                             \
        typename TokenType,                                                         \
        TokenType ValueForNonTerminal = 0,                                          \
        bool MayThrow                 = true,                                       \
        size_t UID                    = 0,                                          \
        bool AllowStringLiterals      = true,                                       \
        bool AllowComments            = true,                                       \
        size_t MaxSize4Terminals      = 4>

#define CERBERUS_LEX_PARENT_CLASS                                                   \
    cerb::lex::experimental::LexicalAnalyzer<                                       \
        CharT,                                                                      \
        TokenType,                                                                  \
        ValueForNonTerminal,                                                        \
        MayThrow,                                                                   \
        UID,                                                                        \
        AllowStringLiterals,                                                        \
        AllowComments,                                                              \
        MaxSize4Terminals>

#define CERBERUS_LEX_PARENT_CLASS_ACCESS                                            \
    using parent = cerb::lex::experimental::LexicalAnalyzer<CharT, TokenType>;      \
    using parent::m_items;                                                          \
    using parent::m_tokens;                                                         \
    using token_t          = typename parent::token_t;                              \
    using result_t         = typename parent::result_t;                             \
    using string_view_t    = typename parent::string_view_t;                        \
    using string_checker_t = typename parent::string_checker_t;

#define CERBERUS_LEX_INITIALIZER(class_name)                                        \
    class_name(                                                                     \
        const std::initializer_list<                                                \
            const cerb::Pair<TokenType, const string_view_t>>                       \
            keywords,                                                               \
        const std::initializer_list<                                                \
            const cerb::Pair<TokenType, const string_view_t>>                       \
            rules,                                                                  \
        const string_checker_t &terminals,                                          \
        size_t input_index                           = 0,                           \
        const string_view_t &single_line_comment     = "//",                        \
        const string_view_t &multiline_comment_begin = "/*",                        \
        const string_view_t &multiline_comment_end   = "*/")                        \
      : parent(                                                                     \
            keywords,                                                               \
            rules,                                                                  \
            terminals,                                                              \
            input_index,                                                            \
            single_line_comment,                                                    \
            multiline_comment_begin,                                                \
            multiline_comment_end)

namespace cerb::lex {
    template<
        typename CharT,
        typename TokenType,
        TokenType ValueForNonTerminal = 0,
        bool MayThrow                 = true,
        size_t UID                    = 0,
        bool AllowStringLiterals      = true,
        bool AllowComments            = true,
        size_t MaxSize4Terminals      = 4>
    class LexAnalyzer
    {
        using Item = DotItem<
            CharT,
            TokenType,
            ValueForNonTerminal,
            MayThrow,
            UID,
            AllowStringLiterals,
            AllowComments,
            MaxSize4Terminals>;
        using result_t         = typename Item::result_t;
        using string_view_t    = typename Item::string_view_t;
        using string_checker_t = typename Item::string_checker_t;

        Set<Item> m_items{};

    public:
        constexpr auto
            scan(const string_view_t &input, const string_view_t &filename)
        {
            m_items.begin()->set_input(input, filename);
            m_items.begin()->skip_comments_and_layout();
            m_items.begin()->dump();

            size_t times = 0;
            Item *item   = nullptr;
            Deque<typename Item::result_t> results{};

            while (!Item::empty()) {
                ItemState state = UNABLE_TO_MATCH;

                for (auto &elem : m_items) {
                    elem.rebind();

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
                    auto i = item->result();
                    results.template emplace_back(item->result());
                    item->add2input(item->dot());
                    item->skip_comments_and_layout();
                    item->dump();
                    times = 0;

                    for (auto &elem : i) {
                        std::cout << std::setw(8) << std::left << elem.repr << ' '
                                  << elem.type << ' '
                                  << "line: " << elem.pos.line_number + 1 << ' '
                                  << elem.pos.char_number << std::endl;
                    }
                } else {
                    m_items.begin()->skip_comments_and_layout();
                    m_items.begin()->dump();
                    ++times;

                    if (times > 1) {
                        if constexpr (MayThrow) {
                            throw std::runtime_error(
                                "Unable to recognize token at " +
                                std::to_string(
                                    m_items.begin()->get_token_pos().char_number) +
                                " " +
                                std::to_string(
                                    m_items.begin()->get_token_pos().line_number));
                        }
                    }
                }
                item = nullptr;
            }
        }

    public:
        LexAnalyzer(
            const std::initializer_list<const Pair<TokenType, const string_view_t>>
                keywords,
            const std::initializer_list<const Pair<TokenType, const string_view_t>>
                rules,
            const string_checker_t terminals,
            size_t input_index                           = 0,
            const string_view_t &single_line_comment     = "//",
            const string_view_t &multiline_comment_begin = "/*",
            const string_view_t &multiline_comment_end   = "*/")
        {
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : keywords) {
                m_items.template emplaceKey(
                    elem.first, 0, elem.first, elem.second, input_index);
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &elem : rules) {
                m_items.template emplaceKey(
                    elem.first, elem.first, elem.second, input_index);
            }

            Item::set_terminals(terminals);
            Item::set_comments(
                single_line_comment, multiline_comment_begin, multiline_comment_end);
        }
    };
}// namespace cerb::lex

namespace cerb::lex::experimental {
    template<
        typename CharT,
        typename TokenType,
        TokenType ValueForNonTerminal = 0,
        bool MayThrow                 = true,
        size_t UID                    = 0,
        bool AllowStringLiterals      = true,
        bool AllowComments            = true,
        size_t MaxSize4Terminals      = 4>
    struct LexicalAnalyzer
    {
        using item_t = DotItem<
            CharT,
            TokenType,
            ValueForNonTerminal,
            MayThrow,
            UID,
            AllowStringLiterals,
            AllowComments,
            MaxSize4Terminals>;

        using token_t          = typename item_t::token_t;
        using result_t         = typename item_t::result_t;
        using string_view_t    = typename item_t::string_view_t;
        using string_checker_t = typename item_t::string_checker_t;

        Set<item_t> m_items{};
        Vector<token_t> m_tokens{};

    public:
        constexpr virtual void yield(size_t, size_t) = 0;

    public:
        constexpr auto
            scan(const string_view_t &input, const string_view_t &filename)
        {
            m_items.begin()->set_input(input, filename);
            m_items.begin()->skip_comments_and_layout();
            m_items.begin()->dump();

            size_t times = 0;
            item_t *item = nullptr;

            while (!item_t::empty()) {
                ItemState state = UNABLE_TO_MATCH;

                for (auto &elem : m_items) {
                    elem.rebind();

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
                    auto vector_begin = m_tokens.size();

                    CERBLIB_UNROLL_N(1)
                    for (const auto &result : item->result()) {
                        m_tokens.template emplace_back(result);
                    }

                    item->add2input(item->dot());
                    item->skip_comments_and_layout();
                    item->dump();
                    times = 0;
                    yield(vector_begin, m_tokens.size());
                } else {
                    m_items.begin()->skip_comments_and_layout();
                    m_items.begin()->dump();
                    ++times;

                    if (times > 1) {
                        if constexpr (MayThrow) {
                            throw std::runtime_error(
                                "Unable to recognize token at " +
                                std::to_string(
                                    m_items.begin()->get_token_pos().char_number) +
                                " " +
                                std::to_string(
                                    m_items.begin()->get_token_pos().line_number));
                        }
                    }
                }
                item = nullptr;
            }
        }

    public:
        LexicalAnalyzer(
            const std::initializer_list<const Pair<TokenType, const string_view_t>>
                keywords,
            const std::initializer_list<const Pair<TokenType, const string_view_t>>
                rules,
            const string_checker_t &terminals,
            size_t input_index                           = 0,
            const string_view_t &single_line_comment     = "//",
            const string_view_t &multiline_comment_begin = "/*",
            const string_view_t &multiline_comment_end   = "*/")
        {
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : keywords) {
                m_items.template emplaceKey(elem.first, 0, elem.first, elem.second);
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &elem : rules) {
                m_items.template emplaceKey(elem.first, elem.first, elem.second);
            }

            item_t::set_terminals(terminals);
            item_t::set_comments(
                single_line_comment, multiline_comment_begin, multiline_comment_end);
        }
    };
}// namespace cerb::lex::experimental

#endif /* CERBERUS_LEX_HPP */
