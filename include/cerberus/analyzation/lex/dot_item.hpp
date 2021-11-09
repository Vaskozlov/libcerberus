#ifndef CERBERUS_DOT_ITEM_HPP
#define CERBERUS_DOT_ITEM_HPP

#include <cerberus/map.hpp>
#include <cerberus/set.hpp>
#include <cerberus/vector.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/analyzation/lex/file.hpp>
#include <cerberus/analyzation/lex/token.hpp>
#include <cerberus/analyzation/lex/char.hpp>
#include <cerberus/string_view.hpp>
#include <cerberus/analyzation/lex/lex_string.hpp>

namespace cerb::lex {
    constexpr size_t MAX_RANGES = 4;

    enum ItemRule : u32
    {
        BASIC,
        OPTIONAL,
        ONE_OR_MORE_TIMES,
        ZERO_OR_MORE_TIMES
    };

    enum ItemState : u32
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
        typename CharT, typename TokenType, bool MayThrow, size_t UID,
        bool AllowStringLiterals, bool AllowComments, size_t MaxTerminals,
        size_t MaxSize4Terminals>
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
            string_view_t rule{};
            TokenType type{};
            u32 priority{};
            bool word{ false };

        public:
            constexpr DotItemInitializer()  = default;
            constexpr ~DotItemInitializer() = default;

            constexpr DotItemInitializer(DotItemInitializer &&) noexcept = default;
            constexpr DotItemInitializer(const DotItemInitializer &) noexcept =
                default;

            constexpr DotItemInitializer(
                TokenType type_, const string_view_t &rule_, bool word_,
                u32 priority_ = 0) noexcept
              : rule(rule_), type(type_), priority(priority_), word(word_)
            {}

            constexpr DotItemInitializer(
                TokenType type_, const string_view_t &rule_) noexcept
              : rule(rule_), type(type_)
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

            CERBLIB_DECL auto at(CharT index) const -> u8
            {
                return bitmap.template at<0>(to_unsigned(index));
            }

            CERBLIB_DECL auto operator[](CharT index) const -> u8
            {
                return at(index);
            }

            template<u8 Value>
            constexpr auto set(CharT index) -> void
            {
                return bitmap.template set<1, 0>(to_unsigned(index));
            }

            CERBLIB_DECL auto can_end() const -> bool
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

            CERBLIB_DECL auto check(CharT elem) -> ItemState
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

        const TokenType m_token_type{};
        size_t m_dot{};
        size_t m_input_index{};

        position_t m_token_pos{};
        position_t m_current_pos{};

        result_t result_of_check{};

        string_view_t m_word_repr{};
        storage_t m_ranges{};
        iterator m_current_range{ m_ranges.begin() };

        string_view_iterator m_token_begin{};

        static inline string_checker_t m_checker{};
        static inline string_view_t m_input{};
        static inline string_view_t m_current_line{};
        static inline string_view_t m_single_line_comment{};
        static inline string_view_t m_multiline_comment_begin{};
        static inline string_view_t m_multiline_comment_end{};
        static inline position_t m_global_position{};

    private:
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

        constexpr auto end_of_input_check() -> ItemState
        {
            if (is_layout_or_end_of_input(get_char()) ||
                (AllowComments &&
                 (check_substring(m_dot, m_input, m_single_line_comment) ||
                  check_substring(m_dot, m_input, m_multiline_comment_begin)))) {
                if (m_dot != 0 && can_end()) {
                    string_view_t repr = { m_token_begin, m_token_begin + m_dot };
                    result_of_check = { { repr, m_token_type, get_begin_of_token(),
                                          m_token_pos } };
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
                                          get_begin_of_token(), m_token_pos } };
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
                    result_of_check = { { repr1, m_token_type, get_begin_of_token(),
                                          m_token_pos },
                                        { repr2, terminal_repr.second,
                                          get_begin_of_token(), m_current_pos } };

                    m_dot += terminal_repr.first.size();
                    m_current_pos += terminal_repr.first.size();
                    return SCAN_FINISHED;
                }
                return UNABLE_TO_MATCH;
            }
            return UNABLE_TO_MATCH;
        }

    public:
        constexpr auto operator<=>(const TokenType &other) const
        {
            return m_token_type <=> other;
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

        constexpr auto operator==(const DotItem &other) const -> bool
        {
            return priority == other.priority && m_token_type == other.m_token_type;
        }

    public:
        CERBLIB_DECL auto dot() const -> size_t
        {
            return m_dot;
        }

        CERBLIB_DECL static auto empty() -> bool
        {
            return m_input.empty();
        }

        constexpr auto dump() const -> void
        {
            m_global_position = m_current_pos;
        }

        template<typename T>
        CERBLIB_DECL static auto char_cast(T value) -> CharT
        {
            return static_cast<CharT>(value);
        }

        CERBLIB_DECL auto result() const -> const result_t &
        {
            return result_of_check;
        }

        template<size_t Offset = 0>
        CERBLIB_DECL auto get_char() const -> CharT
        {
            return m_input[m_dot + Offset];
        }

        CERBLIB_DECL auto get_char(size_t offset) const -> CharT
        {
            return m_input[m_dot + offset];
        }

        CERBLIB_DECL static auto get_input() -> const string_view_t &
        {
            return m_input;
        }

        CERBLIB_DECL auto get_token_pos() const noexcept -> const position_t &
        {
            return static_cast<const position_t &>(m_current_pos);
        }

        CERBLIB_DECL auto get_begin_of_token() const noexcept -> string_view_iterator
        {
            return m_token_begin;
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

        CERBLIB_DECL static auto get_line() -> string_view_t
        {
            const auto &line = m_current_line;
            const auto pos   = line.contains('\n');
            return { line.begin(),
                     line.begin() + cmov(
                                        pos == std::numeric_limits<size_t>::max(),
                                        line.size(), pos) };
        }

        CERBLIB_DECL auto isolate_token() const -> string_view_t
        {
            size_t index         = m_dot;
            string_view_t result = { m_input.begin() + index,
                                     m_input.size() - index };

            CERBLIB_UNROLL_N(2)
            while (!result.empty()) {
                CharT elem = result[index];

                if (is_layout(elem) || elem == char_cast(0) ||
                    check_substring(index, m_input, m_single_line_comment) ||
                    check_substring(index, m_input, m_multiline_comment_begin) ||
                    !m_checker.check(index, result).first.empty()) {
                    break;
                }
                ++index;
            }
            return { result.begin(), m_input.begin() + index };
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
                            m_current_line = { m_input.begin() + 1, m_input.end() };
                            status =
                                cmov(status == SINGLE_LINE_COMMENT, EMPTY, status);
                        } else {
                            m_current_pos += get_char() != '\r';
                        }
                        add2input(1);
                        continue;
                    }
                }
                if (is_layout(get_char())) {
                    if (get_char() == '\n') {
                        m_current_pos.new_line();
                        m_current_line = { m_input.begin() + 1, m_input.end() };
                        status = cmov(status == SINGLE_LINE_COMMENT, EMPTY, status);
                    } else {
                        m_current_pos += get_char() != '\r';
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
            m_input        = input;
            m_current_line = input;
            skip_comments_and_layout();
            m_current_pos.filename = filename;
        }

        auto rebind(bool load_from_shared = true, bool reload_position = false)
            -> void
        {
            m_dot           = 0;
            m_token_begin   = m_input.begin();
            m_current_range = m_ranges.begin();

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

        CERBLIB_DECL auto can_end() const -> bool
        {
            if (m_is_word) {
                return m_dot == m_word_repr.size();
            }

            if (m_current_range == m_ranges.end()) {
                return true;
            }

            CERBLIB_UNROLL_N(1)
            for (auto i = m_ranges.end() - 1; i != m_current_range; --i) {
                switch (i->rule) {
                case OPTIONAL:
                case ZERO_OR_MORE_TIMES:
                    return true;

                default:
                    continue;
                }
            }
            return true;
        }

        auto check() -> ItemState
        {
            while (true) {
                if (m_is_word) {
                    if (m_word_repr[m_dot] == get_char() &&
                        m_dot < m_word_repr.size()) {
                        ++m_dot;
                        m_current_pos += 1;
                        continue;
                    }
                    result_of_check = { { { m_token_begin, m_token_begin + m_dot },
                                          m_token_type,
                                          m_token_begin,
                                          m_token_pos } };
                    return cmov(
                        m_dot == m_word_repr.size(), SCAN_FINISHED, UNABLE_TO_MATCH);
                }

                ItemState state = m_current_range->check(get_char());

                switch (state) {
                case NEED_TO_SCAN:
                    ++m_dot;
                    m_current_pos += 1;
                    break;

                case NEED_TO_SWITCH_RANGE_AND_CHAR:
                    ++m_dot;
                    m_current_pos += 1;
                    [[fallthrough]];

                case NEED_TO_SWITCH_RANGE:
                    ++m_current_range;

                    if (m_current_range != m_ranges.end()) {
                        break;
                    }
                    [[fallthrough]];

                default:
                    return end_of_input_check();
                }
            }
        }

    public:
        constexpr explicit DotItem(const DotItemInitializer &item)
          : priority(item.priority), m_is_word(item.word), m_token_type(item.type),
            m_word_repr(item.rule)
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
