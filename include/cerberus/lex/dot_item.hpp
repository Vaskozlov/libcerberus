#ifndef CERBERUS_DOT_ITEM_HPP
#define CERBERUS_DOT_ITEM_HPP

#include <iomanip>
#include <iostream>
#include <cerberus/map.hpp>
#include <cerberus/set.hpp>
#include <cerberus/vector.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/lex/file.hpp>
#include <cerberus/lex/token.hpp>
#include <cerberus/lex/char.hpp>
#include <cerberus/string_view.hpp>
#include <cerberus/lex/lex_string.hpp>

namespace cerb::lex {
    constexpr size_t MAX_RANGES = 4;
    constexpr gl::Map<char, char, 8> UnprintableChars{ { 't', '\t' },
                                                       { 'n', '\n' },
                                                       { 'r', '\r' },
                                                       { 't', '\t' } };

    enum ItemRule : u16
    {
        BASIC,
        OPTIONAL,
        ONE_OR_MORE_TIMES,
        ZERO_OR_MORE_TIMES
    };

    enum ItemState : u16
    {
        UNABLE_TO_MATCH,
        NEED_TO_SCAN,
        NEED_TO_SWITCH_RANGE,
        NEED_TO_SWITCH_RANGE_AND_CHAR,
        SCAN_FINISHED,
    };

    constexpr gl::Map<ItemState, cerb::string_view, 4> ItemStateRepr{
        { UNABLE_TO_MATCH, "UNABLE_TO_MATCH" },
        { NEED_TO_SCAN, "NEED_TO_SCAN" },
        { NEED_TO_SWITCH_RANGE, "NEED_TO_SWITCH_RANGE" },
        { SCAN_FINISHED, "SCAN_FINISHED" }
    };

    template<
        typename CharT, typename TokenType, bool MayThrow = true, size_t UID = 0,
        bool AllowStringLiterals = true, bool AllowComments = true,
        size_t MaxTerminals = 128, size_t MaxSize4Terminals = 8>
    struct DotItem
    {
        using string_checker_t = TerminalContainer<
            CharT, TokenType, MaxTerminals, MaxSize4Terminals, MayThrow>;
        using string_view_t        = typename string_checker_t::string_view_t;
        using token_t              = Token<CharT, string_view_t, TokenType>;
        using result_t             = gl::Set<token_t, 2, MayThrow>;
        using position_t           = PositionInFile<CharT>;
        using string_view_iterator = typename string_view_t::iterator;

        struct DotItemInitializer
        {
            bool word{ false };
            u32 priority{};
            TokenType type{};
            string_view_t rule{};

        public:
            constexpr DotItemInitializer()                               = default;
            constexpr ~DotItemInitializer()                              = default;
            constexpr DotItemInitializer(DotItemInitializer &&) noexcept = default;
            constexpr DotItemInitializer(const DotItemInitializer &) noexcept =
                default;

            constexpr DotItemInitializer(
                TokenType type_, const string_view_t &rule_, bool word_,
                u32 priority_ = 0) noexcept
              : word(word_), priority(priority_), type(type_), rule(rule_)
            {}

            constexpr DotItemInitializer(
                TokenType type_, const string_view_t &rule_) noexcept
              : word(false), priority(0), type(type_), rule(rule_)
            {}

            constexpr auto operator     =(DotItemInitializer &&) noexcept
                -> DotItemInitializer & = default;
            constexpr auto operator     =(const DotItemInitializer &) noexcept
                -> DotItemInitializer & = default;
        };

    private:
        enum SkipStatus : u16
        {
            EMPTY,
            SINGLE_LINE_COMMENT,
            MULTILINE_COMMENT
        };

        struct Range
        {
            constexpr auto rebind() -> void
            {
                times = 0;
            }

            [[nodiscard]] constexpr auto at(CharT index) const -> u8
            {
                return bitmap.template at<0>(to_unsigned(index));
            }

            [[nodiscard]] constexpr auto operator[](CharT index) const -> u8
            {
                return at(index);
            }

            template<u8 Value>
            constexpr auto set(CharT index) -> void
            {
                return bitmap.template set<1, 0>(to_unsigned(index));
            }

            [[nodiscard]] constexpr auto can_end() const -> bool
            {
                switch (rule) {
                case BASIC:
                    return times == 1;

                case OPTIONAL:
                    return times <= 1;

                case ONE_OR_MORE_TIMES:
                    return times >= 1;

                default:// ZERO_OR_MORE_TIMES
                    return true;
                }
            }

            [[nodiscard]] constexpr auto check(CharT elem) -> ItemState
            {
                u8 contains = at(elem);

                if (contains == 1) {
                    ++times;

                    switch (rule) {
                    case BASIC:
                    case OPTIONAL:
                        return NEED_TO_SWITCH_RANGE_AND_CHAR;

                    default:// ZERO_OR_MORE_TIMES, ONE_OR_MORE_TIMES
                        return NEED_TO_SCAN;
                    }
                }

                switch (rule) {
                case BASIC:
                    return UNABLE_TO_MATCH;

                case ONE_OR_MORE_TIMES:
                    return cmov(times >= 1, NEED_TO_SWITCH_RANGE, UNABLE_TO_MATCH);

                default:// OPTIONAL, ZERO_OR_MORE_TIMES
                    return NEED_TO_SWITCH_RANGE;
                }
            }

            constexpr Range()                  = default;
            constexpr ~Range()                 = default;
            constexpr Range(const Range &)     = default;
            constexpr Range(Range &&) noexcept = default;

            constexpr auto operator=(const Range &) -> Range & = default;
            constexpr auto operator=(Range &&) noexcept -> Range & = default;

        public:
            ItemRule rule{};
            size_t times{};
            ConstBitmap<1, string_checker_t::MaxChars> bitmap{};
        };

        using storage_t = gl::Set<Range, MAX_RANGES, MayThrow>;
        using iterator  = typename storage_t::iterator;

    public:
        u32 priority{};

    private:
        bool m_is_word{ false };
        string_view_t m_word_repr{};

        size_t m_dot{};
        size_t m_input_index{};
        const TokenType m_token_type{};

        position_t m_token_pos{};
        position_t m_current_pos{};

        result_t result_of_check{};

        storage_t m_ranges{};
        iterator m_current_range{ m_ranges.begin() };

        string_view_iterator m_token_begin{};

        static inline string_checker_t m_checker{};
        static inline string_view_t m_input{};
        static inline string_view_t m_single_line_comment{};
        static inline string_view_t m_multiline_comment_begin{};
        static inline string_view_t m_multiline_comment_end{};
        static inline position_t m_global_position{};

    public:
        constexpr auto operator==(const DotItem &other) const -> bool
        {
            return priority == other.priority && m_token_type == other.m_token_type;
        }

        constexpr auto operator<=>(const DotItem &other) const
        {
            if (priority == other.priority) {
                return m_token_type <=> other.m_token_type;
            }
            return priority <=> other.priority;
        }

        constexpr auto operator==(const TokenType &other) const -> bool
        {
            return m_token_type == other;
        }

        constexpr auto operator<=>(const TokenType &other) const
        {
            return m_token_type <=> other;
        }

    public:
        constexpr auto dot() -> size_t
        {
            return m_dot;
        }

        constexpr auto result() const -> const result_t &
        {
            return result_of_check;
        }

        constexpr auto get_token_pos() -> position_t &
        {
            return m_current_pos;
        }

        template<size_t Offset = 0>
        constexpr auto get_char() -> CharT
        {
            return m_input[m_dot + Offset];
        }

        constexpr auto get_char(size_t offset) -> CharT
        {
            return m_input[m_dot + offset];
        }

        constexpr auto isolate_token() const -> const string_view_t
        {
            size_t index         = m_dot;
            string_view_t result = { m_input.begin() + index,
                                     m_input.size() - index };

            CERBLIB_UNROLL_N(2)
            while (!result.empty()) {
                CharT elem = result[index];

                if (is_layout(elem) || elem == char_cast(0) ||
                    check_substring(index, m_input, m_single_line_comment) ||
                    check_substring(index, m_input, m_multiline_comment_begin)) {
                    break;
                }
                ++index;
            }
            return { result.begin(), m_input.begin() + index };
        }

        template<typename T>
        static constexpr auto char_cast(T value) -> CharT
        {
            return static_cast<CharT>(value);
        }

        static constexpr auto add2input(size_t offset) -> void
        {
            auto &input = m_input;
            m_input     = { cerb::min(input.begin() + offset, input.end()),
                        input.end() };
        }

        static constexpr auto add2input(string_view_iterator first) -> void
        {
            auto &input = m_input;
            m_input     = { cerb::min(first, input.end()), input.end() };
        }

        constexpr auto throw_if_can(bool condition, const char *message) -> void
        {
            if constexpr (MayThrow) {
                if (!condition) {
                    throw std::runtime_error(message);
                }
            }
        }

        constexpr auto check_range_rule(iterator current_range) -> void
        {
            throw_if_can(
                current_range->rule == BASIC,
                "You can't change range's rule more than once!");
        }

        auto skip_comments_and_layout() -> void
        {
            m_dot             = 0;
            SkipStatus status = EMPTY;

            CERBLIB_UNROLL_N(2)
            while (!m_input.empty()) {
                // we will check for comments if there are allowed
                if constexpr (AllowComments) {
                    /*
                     * if we aren't inside a comment
                     * we need to check for comment beginning
                     * */
                    if (status == EMPTY) {
                        if (check_substring(0, m_input, m_single_line_comment)) {
                            status = SINGLE_LINE_COMMENT;
                            add2input(m_single_line_comment.size());
                            m_current_pos += m_single_line_comment.size();
                            continue;
                        }
                        if (check_substring(0, m_input, m_multiline_comment_begin)) {
                            status = MULTILINE_COMMENT;
                            add2input(m_multiline_comment_begin.size());
                            m_current_pos += m_multiline_comment_begin.size();
                            continue;
                        }
                    } else if (
                        status == MULTILINE_COMMENT &&
                        check_substring(0, m_input, m_multiline_comment_end)) {
                        status = EMPTY;
                        add2input(m_multiline_comment_begin.size());
                        m_current_pos += m_multiline_comment_begin.size();
                        continue;
                    } else {
                        if (get_char() == '\n') {
                            m_current_pos.new_line();
                            status =
                                cmov(status == SINGLE_LINE_COMMENT, EMPTY, status);
                        } else {
                            m_current_pos += 1;
                        }
                        add2input(1);
                        continue;
                    }
                }
                if (is_layout(get_char())) {
                    if (get_char() == '\n') {
                        m_current_pos.new_line();
                        status = cmov(status == SINGLE_LINE_COMMENT, EMPTY, status);
                    } else {
                        m_current_pos += 1;
                    }
                    add2input(1);
                    continue;
                }
                return;
            }
        }

        constexpr auto
            set_input(const string_view_t &input, const string_view_t &filename)
                -> void
        {
            m_input = input;
            skip_comments_and_layout();
            m_current_pos.filename = filename;
        }

        constexpr auto dump() const -> void
        {
            m_global_position = m_current_pos;
        }

        auto rebind(bool load_from_shared = true, bool reload_position = false)
            -> void
        {
            m_dot           = 0;
            m_current_range = m_ranges.begin();
            m_token_begin   = m_input.begin();

            if (reload_position) {
                m_current_pos.char_number = 0;
                m_current_pos.line_number = 0;
            }
            if (load_from_shared) {
                m_current_pos = m_global_position;
            }

            m_token_pos = m_current_pos;

            CERBLIB_UNROLL_N(2)
            for (auto &elem : m_ranges) {
                elem.rebind();
            }
        }

        static auto set_comments(
            const string_view_t &single_line_comment,
            const string_view_t &multiline_comment_begin,
            const string_view_t &multiline_comment_end) -> void
        {
            m_single_line_comment     = single_line_comment;
            m_multiline_comment_begin = multiline_comment_begin;
            m_multiline_comment_end   = multiline_comment_end;
        }

        static auto set_terminals(const string_checker_t &checker) -> void
        {
            m_checker = checker;
        }

        constexpr static auto empty() -> bool
        {
            return m_input.empty();
        }

        [[nodiscard]] auto can_end() const -> bool
        {
            if (m_is_word) {
                return m_dot == m_word_repr.size();
            }

            CERBLIB_UNROLL_N(2)
            for (auto i = m_ranges.end() - 1; i != m_current_range; --i) {
                switch (i->rule) {
                case OPTIONAL:
                case ZERO_OR_MORE_TIMES:
                    return true;

                default:
                    return false;
                }
            }
            return true;
        }

        auto check() -> ItemState
        {
            if (is_layout(get_char()) || get_char() == char_cast(0) ||
                check_substring(m_dot, m_input, m_single_line_comment) ||
                check_substring(m_dot, m_input, m_multiline_comment_begin)) {
                if (m_dot != 0 && can_end()) {
                    string_view_t repr = { m_token_begin, m_token_begin + m_dot };
                    result_of_check    = { { repr, m_token_type, m_token_pos } };
                    return SCAN_FINISHED;
                }

                return UNABLE_TO_MATCH;
            }

            auto terminal_repr = m_checker.check(m_dot, m_input);
            if (!terminal_repr.first.empty()) {
                if (m_dot == 0) {
                    string_view_t repr = {
                        m_token_begin, m_token_begin + terminal_repr.first.size()
                    };
                    result_of_check = { { repr, terminal_repr.second,
                                          m_token_pos } };
                    m_dot += terminal_repr.first.size();
                    m_current_pos += terminal_repr.first.size();
                    return SCAN_FINISHED;
                }
                if (m_current_range == m_ranges.end() ||
                    (m_current_range->can_end() && can_end())) {
                    string_view_t repr1 = { m_token_begin, m_token_begin + m_dot };
                    string_view_t repr2 = {
                        repr1.end(), repr1.end() + terminal_repr.first.size()
                    };
                    result_of_check = { { repr1, m_token_type, m_token_pos },
                                        { repr2, terminal_repr.second,
                                          m_current_pos } };

                    m_dot += terminal_repr.first.size();
                    m_current_pos += terminal_repr.first.size();
                    return SCAN_FINISHED;
                }
                return UNABLE_TO_MATCH;
            }

            if (get_char() == char_cast('"')) {}
            if (get_char() == char_cast('\'')) {}

            if (m_is_word) {
                if (m_word_repr[m_dot] == get_char() && m_dot < m_word_repr.size()) {
                    ++m_dot;
                    m_current_pos += 1;
                    return NEED_TO_SCAN;
                }
                return UNABLE_TO_MATCH;
            }

            ItemState state = m_current_range->check(get_char());

            switch (state) {
            case NEED_TO_SCAN:
                ++m_dot;
                m_current_pos += 1;
                return NEED_TO_SCAN;

            case NEED_TO_SWITCH_RANGE_AND_CHAR:
                ++m_dot;
                m_current_pos += 1;
                [[fallthrough]];

            case NEED_TO_SWITCH_RANGE:
                ++m_current_range;
                return NEED_TO_SCAN;

            default:
                return UNABLE_TO_MATCH;
            }
        }

    public:
        constexpr DotItem(const DotItemInitializer &item)
          : priority(item.priority), m_is_word(item.word), m_word_repr(item.rule),
            m_token_type(item.type)
        {
            if (item.word) {
                return;
            }

            bool is_range = false;// if we are looking for a range of chars
            bool are_brackets_opened = false;       // if range brackets are opened
            auto letter              = char_cast(0);// previous letter
            iterator current_range   = m_ranges.begin();// current range

            for (const auto elem : item.rule) {
                switch (elem) {
                case '[':
                    throw_if_can(
                        !are_brackets_opened,
                        "You can't open range more than once!");

                    letter              = char_cast(0);
                    is_range            = false;
                    are_brackets_opened = true;
                    current_range       = m_ranges.last();
                    break;

                case ']':
                    throw_if_can(
                        are_brackets_opened, "There aren't any ranges to close");

                    // set value to the bitmap, if it wasn't a range of elems
                    if (letter != char_cast(0)) {
                        current_range->template set<1>(letter);
                    }

                    are_brackets_opened = false;
                    break;

                case '-':
                    throw_if_can(
                        are_brackets_opened,
                        "You can't create range of chars outside of range!");
                    is_range = true;
                    break;

                case '+':
                    check_range_rule(current_range);
                    current_range->rule = ONE_OR_MORE_TIMES;
                    break;

                case '*':
                    check_range_rule(current_range);
                    current_range->rule = ZERO_OR_MORE_TIMES;
                    break;

                case '?':
                    check_range_rule(current_range);
                    current_range->rule = OPTIONAL;
                    break;

                default:
                    if (letter != char_cast(0) && is_range) {
                        CERBLIB_UNROLL_N(4)
                        for (; letter <= elem; ++letter) {
                            current_range->template set<1>(letter);
                        }
                    } else if (letter != char_cast(0)) {
                        current_range->template set<1>(letter);
                    }

                    letter   = elem;
                    is_range = false;
                    break;
                }
            }
        }

        consteval DotItem(bool /*constEval*/, const DotItemInitializer &item)
          : DotItem(item)
        {}
    };
}// namespace cerb::lex

#endif /* CERBERUS_DOT_ITEM_HPP */
