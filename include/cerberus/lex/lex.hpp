#ifndef CERBERUS_LEX_HPP
#define CERBERUS_LEX_HPP

#include <cerberus/lex/dot_item.hpp>

#define CERBERUS_LEX_TEMPLATES                                                      \
    template<                                                                       \
        typename CharT, typename TokenType, bool MayThrow = true, size_t UID = 0,   \
        bool AllowStringLiterals = true, bool AllowComments = true,                 \
        size_t MaxTerminals = 128, size_t MaxSize4Terminals = 8>

#define CERBERUS_LEX_PARENT_CLASS                                                   \
    cerb::lex::experimental::LexicalAnalyzer<                                       \
        CharT, TokenType, MayThrow, UID, AllowStringLiterals, AllowComments,        \
        MaxTerminals, MaxSize4Terminals>

#define CERBERUS_LEX_PARENT_CLASS_ACCESS                                            \
    using parent = cerb::lex::experimental::LexicalAnalyzer<                        \
        CharT, TokenType, MayThrow, UID, AllowStringLiterals, AllowComments,        \
        MaxTerminals, MaxSize4Terminals>;                                           \
    using parent::m_items;                                                          \
    using parent::m_tokens;                                                         \
    using token_t          = typename parent::token_t;                              \
    using result_t         = typename parent::result_t;                             \
    using position_t       = typename parent::position_t;                           \
    using string_view_t    = typename parent::string_view_t;                        \
    using string_checker_t = typename parent::string_checker_t;                     \
    using item_initilizer  = typename parent::item_initilizer;

#define CERBERUS_LEX_INITIALIZER(class_name)                                        \
    class_name(                                                                     \
        const std::initializer_list<const item_initilizer> keywords,                \
        const std::initializer_list<const item_initilizer> rules,                   \
        const string_checker_t &terminals,                                          \
        const string_view_t &single_line_comment     = "//",                        \
        const string_view_t &multiline_comment_begin = "/*",                        \
        const string_view_t &multiline_comment_end   = "*/")                        \
      : parent(                                                                     \
            keywords, rules, terminals, single_line_comment,                        \
            multiline_comment_begin, multiline_comment_end)

namespace cerb::lex::experimental {
    template<
        typename CharT, typename TokenType, bool MayThrow = true, size_t UID = 0,
        bool AllowStringLiterals = true, bool AllowComments = true,
        size_t MaxTerminals = 128, size_t MaxSize4Terminals = 4>
    struct LexicalAnalyzer
    {
        using item_t = DotItem<
            CharT, TokenType, MayThrow, UID, AllowStringLiterals, AllowComments,
            MaxTerminals, MaxSize4Terminals>;

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
        Vector<token_t> m_tokens{};

    public:
        constexpr virtual void yield(size_t, size_t) = 0;
        constexpr virtual void error(
            const position_t &pos,
            const string_view_t &line,
            const string_view_t &repr) = 0;

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
                        m_tokens.emplace_back(result);
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
                        error(
                            m_items.begin()->get_token_pos(),
                            m_items.begin()->get_line(),
                            m_items.begin()->isolate_token());
                    }
                }
                item = nullptr;
            }
        }

    public:
        constexpr ~LexicalAnalyzer() = default;

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
