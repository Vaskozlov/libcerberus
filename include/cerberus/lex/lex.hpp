#ifndef CERBERUS_LEX_HPP
#define CERBERUS_LEX_HPP

#include <cerberus/lex/dot_item.hpp>

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
            std::list<typename Item::result_t> results{};

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

            for (const auto &i : results) {
                for (auto &elem : i) {
                    /*std::cout << std::setw(8) << std::left << elem.repr << ' '
                              << elem.type << ' ' << "line: " << elem.pos.line_number
                              << ' ' << elem.pos.char_number << std::endl;
                    */
                }
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
            const string_view_t &single_line_comment     = "//"sv,
            const string_view_t &multiline_comment_begin = "/*"sv,
            const string_view_t &multiline_comment_end   = "*/"sv)
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

        constexpr LexAnalyzer() = default;
    };
}// namespace cerb::lex

#endif /* CERBERUS_LEX_HPP */
