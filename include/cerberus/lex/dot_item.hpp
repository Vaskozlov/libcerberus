#ifndef CERBERUS_DOT_ITEM_HPP
#define CERBERUS_DOT_ITEM_HPP

#include <list>
#include <iomanip>
#include <iostream>
#include <cerberus/map.hpp>
#include <cerberus/set.hpp>
#include <cerberus/vector.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/lex/file.hpp>
#include <cerberus/lex/token.hpp>
#include <cerberus/lex/char.hpp>

namespace cerb::lex {
    constexpr size_t MAX_RANGES = 10;

    enum ItemRule : u16
    {
        BASIC,
        OPTIONAL,
        ONE_OR_MORE_TIMES,
        ZERO_OR_MORE_TIMES
    };

    enum ItemState : u16
    {
        UNINITIALIZED,
        OUT_OF_ELEMS,
        UNABLE_TO_MATCH,
        NEED_TO_SCAN,
        NEED_TO_SWITCH_RANGE,
        NEED_TO_SWITCH_RANGE_AND_CHAR,
        SCAN_FINISHED,
    };

    enum DotItemInputWay
    {
        LOCAL,
        GLOBAL,
        INDEXED
    };

    using namespace std::string_view_literals;

    constexpr gl::Map<ItemState, std::string_view, 6> ItemStateRepr{
        { UNINITIALIZED, "UNINITIALIZED"sv },
        { OUT_OF_ELEMS, "OUT_OF_ELEMS"sv },
        { UNABLE_TO_MATCH, "UNABLE_TO_MATCH"sv },
        { NEED_TO_SCAN, "NEED_TO_SCAN"sv },
        { NEED_TO_SWITCH_RANGE, "NEED_TO_SWITCH_RANGE"sv },
        { SCAN_FINISHED, "SCAN_FINISHED"sv }
    };

    template<typename T>
    constexpr auto to_unsigned(T value)
    {
        static_assert(std::is_integral_v<T>);

        if constexpr (std::is_unsigned_v<T>) {
            return value;
        }
        if constexpr (sizeof(T) == sizeof(u8)) {
            return bit_cast<u8>(value);
        }
        if constexpr (sizeof(T) == sizeof(u16)) {
            return bit_cast<u16>(value);
        }
        if constexpr (sizeof(T) == sizeof(u32)) {
            return bit_cast<u32>(value);
        }
        if constexpr (sizeof(T) == sizeof(u64)) {
            return bit_cast<u64>(value);
        }
    }

    template<typename CharT>
    constexpr auto check_substring(
        size_t index, const std::basic_string_view<CharT> &src,
        const std::basic_string_view<CharT> &substr) -> bool
    {
        size_t i           = 0;
        size_t src_size    = src.size();
        size_t substr_size = substr.size();

        CERBLIB_UNROLL_N(2)
        while (index + i < src_size && i < substr_size) {
            if (src[index + i] != substr[i]) {
                return false;
            }
            ++i;
        }

        return i == substr_size;
    }

    template<typename CharT, size_t MaxLength = 4, bool MayThrow = true>
    struct StringChecker
    {
        constexpr static size_t MaxChars = (1ULL << bitsizeof(CharT)) - 1;
        using string_view_t              = std::basic_string_view<CharT>;
        using bitmap_t                   = ConstBitmap<1, MaxChars>;
        using storage_t                  = std::array<bitmap_t, MaxLength>;

    private:
        storage_t m_bitmaps{};

    public:
        constexpr auto check(CharT elem) const -> bool
        {
            return static_cast<bool>(m_bitmaps[0].template at<0>(to_unsigned(elem)));
        }

        constexpr auto check(size_t index, const string_view_t &str) const
            -> string_view_t
        {
            size_t i = 0;

            CERBLIB_UNROLL_N(2)
            for (; i < str.size(); ++i) {
                if (m_bitmaps[i].template at<0>(to_unsigned(str[index + i])) == 0) {
                    return { str.begin(), str.begin() + i };
                }
            }

            return { str.begin(), str.begin() + i };
        }

    public:
        consteval StringChecker() = default;

        consteval StringChecker(
            const std::initializer_list<CharT> &chars,
            const std::initializer_list<const string_view_t> &strings)
        {
            CERBLIB_UNROLL_N(4)
            for (const auto &elem : chars) {
                m_bitmaps[0].template set<1, 0>(to_unsigned(elem));
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &elem : strings) {
                size_t counter = 0;

                if constexpr (MayThrow) {
                    if (MaxLength <= counter) {
                        throw std::out_of_range(
                            "String checker can't hold such a long string!");
                    }
                }

                CERBLIB_UNROLL_N(4)
                for (const auto chr : elem) {
                    m_bitmaps[counter++].template set<1, 0>(to_unsigned(chr));
                }
            }
        }
    };

    template<
        typename CharT, typename TokenType, TokenType ValueForNonTerminal = 0,
        bool MayThrow = true, size_t UID = 0, bool AllowStringLiterals = true,
        bool AllowComments = true, DotItemInputWay InputWay = GLOBAL,
        size_t MaxSize4Terminals = 4, size_t MaxInputIndex = 8>
    class DotItem
    {
        static_assert(
            MaxInputIndex > 0, "DotItem MaxInputIndex must be greater than 0");

    public:
        using string_checker_t = StringChecker<CharT, MaxSize4Terminals, MayThrow>;
        using string_view_t    = typename string_checker_t::string_view_t;
        using token_t          = Token<string_view_t, TokenType>;
        using result_t         = gl::Set<token_t, 2, MayThrow>;
        using string_view_iterator = typename string_view_t::iterator;

    private:
        enum SkipStatus
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

        public:
            ItemRule rule{};
            size_t times{};
            ConstBitmap<1, string_checker_t::MaxChars> bitmap{};
        };

        using storage_t = gl::Set<Range, MAX_RANGES, MayThrow>;
        using iterator  = typename storage_t::iterator;
        constexpr static size_t array_size =
            MaxInputIndex * static_cast<unsigned long>(InputWay == INDEXED) +
            static_cast<unsigned long>(InputWay == GLOBAL);

    private:
        size_t m_dot{};
        size_t m_input_index{};
        const TokenType m_token_type{};

        PositionInFile<CharT> m_token_pos{};
        PositionInFile<CharT> m_current_pos{};

        result_t result_of_check{};

        storage_t m_ranges{};
        iterator m_current_range{ m_ranges.begin() };

        string_view_iterator m_token_begin{};

        static inline string_checker_t m_checker{};
        static inline std::array<PositionInFile<CharT>, array_size>
            m_global_position{};
        static inline std::array<string_view_t, array_size> m_input{};
        static inline string_view_t m_single_line_comment{};
        static inline string_view_t m_multiline_comment_begin{};
        static inline string_view_t m_multiline_comment_end{};

    public:
        constexpr auto dot() -> size_t
        {
            return m_dot;
        }

        constexpr auto result() const -> const result_t &
        {
            return result_of_check;
        }

        constexpr auto get_input() -> string_view_t &
        {
            if constexpr (InputWay == INDEXED) {
                return m_input[m_input_index];
            }
            return m_input[0];
        }

        constexpr auto get_position() -> PositionInFile<CharT> &
        {
            if constexpr (InputWay == INDEXED) {
                return m_global_position[m_input_index];
            }
            return m_global_position[0];
        }

        constexpr auto get_token_pos() -> PositionInFile<CharT> &
        {
            return m_current_pos;
        }

        constexpr auto get_input() const -> string_view_t &
        {
            if constexpr (InputWay == INDEXED) {
                return m_input[m_input_index];
            }
            return m_input[0];
        }

        template<size_t Offset = 0>
        constexpr auto get_char() -> CharT
        {
            return get_input()[m_dot + Offset];
        }

        constexpr auto get_char(size_t offset) -> CharT
        {
            return get_input()[m_dot + offset];
        }

        template<typename T>
        constexpr auto char_cast(T value) -> CharT
        {
            return static_cast<CharT>(value);
        }

        constexpr auto add2input(size_t offset) -> void
        {
            auto &input = get_input();
            get_input() = { min(input.begin() + offset, input.end()), input.end() };
        }

        constexpr auto add2input(string_view_iterator first) -> void
        {
            auto &input = get_input();
            get_input() = { min(first, input.end()), input.end() };
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

        constexpr auto skip_comments_and_layout() -> void
        {
            m_dot = 0;
            SkipStatus status = EMPTY;

            CERBLIB_UNROLL_N(2)
            while (!get_input().empty()) {
                // we will check for comments if there are allowed
                if constexpr (AllowComments) {
                    /*
                     * if we aren't inside a comment
                     * we need to check for comment beginning
                     * */
                    if (status == EMPTY) {
                        if (check_substring(0, get_input(), m_single_line_comment)) {
                            status = SINGLE_LINE_COMMENT;
                            add2input(m_single_line_comment.size());
                            m_current_pos += m_single_line_comment.size();
                            continue;
                        }
                        if (check_substring(
                                0, get_input(), m_multiline_comment_begin)) {
                            status = MULTILINE_COMMENT;
                            add2input(m_multiline_comment_begin.size());
                            m_current_pos += m_multiline_comment_begin.size();
                            continue;
                        }
                    } else if (
                        status == MULTILINE_COMMENT &&
                        check_substring(0, get_input(), m_multiline_comment_end)) {
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
            get_input() = input;
            skip_comments_and_layout();
            m_current_pos.filename = filename;
        }

        constexpr auto dump() -> void
        {
            get_position() = m_current_pos;
        }

        constexpr auto
            rebind(bool load_from_shared = true, bool reload_position = false)
                -> void
        {
            m_dot           = 0;
            m_current_range = m_ranges.begin();
            m_token_begin   = get_input().begin();

            if (reload_position) {
                m_current_pos.char_number = 0;
                m_current_pos.line_number = 0;
            }
            if (load_from_shared) {
                m_current_pos = get_position();
            }

            m_token_pos = m_current_pos;

            CERBLIB_UNROLL_N(2)
            for (auto &elem : m_ranges) {
                elem.rebind();
            }
        }

        static constexpr auto set_comments(
            const string_view_t &single_line_comment,
            const string_view_t &multiline_comment_begin,
            const string_view_t &multiline_comment_end) -> void
        {
            m_single_line_comment     = single_line_comment;
            m_multiline_comment_begin = multiline_comment_begin;
            m_multiline_comment_end   = multiline_comment_end;
        }

        static constexpr auto set_terminals(const string_checker_t &checker) -> void
        {
            m_checker = checker;
        }

        constexpr static auto empty() -> bool
        {
            return m_input[0].empty();
        }

        constexpr auto can_end() const -> bool
        {
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

        constexpr auto check() -> ItemState
        {
            if (is_layout(get_char()) || get_char() == char_cast(0) ||
                check_substring(m_dot, get_input(), m_single_line_comment) ||
                check_substring(m_dot, get_input(), m_multiline_comment_begin)) {
                if (m_dot != 0) {
                    string_view_t repr = { m_token_begin, m_token_begin + m_dot };
                    result_of_check    = { { repr, m_token_type, m_token_pos } };

                    return SCAN_FINISHED;
                }

                return UNABLE_TO_MATCH;
            }

            auto terminal_repr = m_checker.check(m_dot, get_input());
            if (!terminal_repr.empty()) {
                if (m_dot == 0) {
                    m_current_pos += 1;
                    string_view_t repr = { m_token_begin,
                                           m_token_begin + terminal_repr.size() };
                    result_of_check = { { repr, ValueForNonTerminal, m_token_pos } };
                    m_dot += terminal_repr.size();
                    return SCAN_FINISHED;
                }
                if (m_current_range == m_ranges.end() ||
                    (m_current_range->can_end() && can_end())) {
                    string_view_t repr1 = { m_token_begin, m_token_begin + m_dot };
                    string_view_t repr2 = { repr1.end(),
                                            repr1.end() + terminal_repr.size() };
                    result_of_check     = { { repr1, m_token_type, m_token_pos },
                                        { repr2, ValueForNonTerminal,
                                          m_current_pos + terminal_repr.size() } };

                    m_dot += terminal_repr.size();
                    m_current_pos += m_dot;
                    return SCAN_FINISHED;
                }
                return UNABLE_TO_MATCH;
            }

            ItemState state = m_current_range->check(get_char());
            m_current_pos += 1;

            switch (state) {
            case NEED_TO_SCAN:
                ++m_dot;
                return NEED_TO_SCAN;

            case NEED_TO_SWITCH_RANGE_AND_CHAR:
                ++m_dot;
                [[fallthrough]];

            case NEED_TO_SWITCH_RANGE:
                ++m_current_range;
                return NEED_TO_SCAN;

            default:
                return UNABLE_TO_MATCH;
            }
        }

    public:
        constexpr DotItem(
            int /*keyword*/, TokenType token_type, const string_view_t &repr,
            size_t input_index)
          : m_input_index(input_index), m_token_type(token_type)
        {
            iterator current_range = m_ranges.begin();// current range

            CERBLIB_UNROLL_N(2)
            for (const auto elem : repr) {
                current_range->template set<1>(elem);
                ++current_range;
            }
        }

        constexpr DotItem(
            TokenType token_type, const string_view_t &input, size_t input_index = 0)
          : m_input_index(input_index), m_token_type(token_type)
        {
            bool is_range = false;// if we are looking for a range of chars
            bool are_brackets_opened = false;       // if range brackets are opened
            auto letter              = char_cast(0);// previous letter
            iterator current_range   = m_ranges.begin();// current range

            for (const auto elem : input) {
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

        consteval DotItem(
            bool constEval, TokenType token_type, const string_view_t &input,
            size_t input_index = 0)
          : DotItem(token_type, input, input_index)
        {}

        consteval DotItem(
            bool constEval, int keyword, TokenType token_type,
            const string_view_t &input, size_t input_index = 0)
          : DotItem(keyword, token_type, input, input_index)
        {}
    };

    template<
        typename CharT, typename TokenType, TokenType ValueForNonTerminal = 0,
        bool MayThrow = true, size_t UID = 0, bool AllowStringLiterals = true,
        bool AllowComments = true, DotItemInputWay InputWay = GLOBAL,
        size_t MaxSize4Terminals = 4, size_t MaxInputIndex = 8>
    class DotItemController
    {
        using Item = DotItem<
            CharT, TokenType, ValueForNonTerminal, MayThrow, UID,
            AllowStringLiterals, AllowComments, InputWay, MaxSize4Terminals,
            MaxInputIndex>;
        using string_view_t    = typename Item::string_view_t;
        using string_checker_t = typename Item::string_checker_t;

        std::list<Item> m_items{};

    public:
        constexpr auto
            scan(const string_view_t &input, const string_view_t &filename)
        {
            m_items.begin()->set_input(input, filename);
            m_items.begin()->skip_comments_and_layout();
            m_items.begin()->dump();

            size_t times = 0;
            Item *item = nullptr;
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
                    results.template emplace_back(item->result());
                    item->add2input(item->dot());
                    item->skip_comments_and_layout();
                    item->dump();
                    times = 0;
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
                    std::cout << std::setw(8) << std::left << elem.repr << ' '
                              << elem.type << ' ' << elem.pos.char_number << ' '
                              << elem.pos.line_number << std::endl;
                }
            }
        }

    public:
        DotItemController(
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
                m_items.template emplace_back(
                    0, elem.first, elem.second, input_index);
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &elem : rules) {
                m_items.template emplace_back(elem.first, elem.second, input_index);
            }

            Item::set_terminals(terminals);
            Item::set_comments(
                single_line_comment, multiline_comment_begin, multiline_comment_end);
        }
    };

}// namespace cerb::lex

namespace cerb::lex::experimental {
    using namespace std::string_view_literals;

    template<typename T>
    constexpr auto to_unsigned(T value)
    {
        static_assert(std::is_integral_v<T>);

        if constexpr (std::is_unsigned_v<T>) {
            return value;
        }
        if constexpr (sizeof(T) == 1) {
            return bit_cast<u8>(value);
        }
        if constexpr (sizeof(T) == sizeof(u16)) {
            return bit_cast<u16>(value);
        }
        if constexpr (sizeof(T) == sizeof(u32)) {
            return bit_cast<u32>(value);
        }
        if constexpr (sizeof(T) == sizeof(u64)) {
            return bit_cast<u64>(value);
        }
    }

    template<typename CharT>
    constexpr auto check_substring(
        size_t index, std::basic_string_view<CharT> src,
        std::basic_string_view<CharT> substr)
    {
        size_t i           = 0;
        size_t src_size    = src.size();
        size_t substr_size = substr.size();

        CERBLIB_UNROLL_N(2)
        while (index + i < src_size && i < substr_size) {
            if (src[index + i] != substr[i]) {
                return false;
            }
            ++i;
        }

        return i == substr_size;
    }

    template<typename CharT, size_t MAX_LENGTH = 4, bool MayThrow = true>
    struct StringChecker
    {
        static constexpr size_t MaxChars = (1ULL << bitsizeof(CharT)) - 1;
        using StringView                 = std::basic_string_view<CharT>;
        using BitmapType                 = ConstBitmap<1, MaxChars>;
        using StorageType                = std::array<BitmapType, MAX_LENGTH>;

    private:
        StorageType m_bitmaps{};

    public:
        constexpr auto check(CharT elem) const -> bool
        {
            return static_cast<bool>(m_bitmaps[0].template at<0>(elem));
        }

        constexpr auto check(size_t index, const StringView &str) const -> StringView
        {
            size_t i = 0;

            CERBLIB_UNROLL_N(2)
            for (; i < str.size(); ++i) {
                if (m_bitmaps[i].template at<0>(str[index + i]) == 0) {
                    return { str.begin(), str.begin() + i };
                }
            }

            return { str.begin(), str.begin() + i };
        }

    public:
        consteval StringChecker() = default;

        consteval StringChecker(
            const std::initializer_list<CharT> &chars,
            const std::initializer_list<StringView> &strings)
        {
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : chars) {
                m_bitmaps[0].template set<1, 0>(elem);
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &elem : strings) {
                size_t counter = 0;

                if constexpr (MayThrow) {
                    if (MAX_LENGTH <= counter) {
                        throw std::out_of_range(
                            "StringChecker can't hold such a long string!");
                    }
                }

                CERBLIB_UNROLL_N(1)
                for (const auto chr : elem) {
                    m_bitmaps[counter++].template set<1, 0>(chr);
                }
            }
        }
    };

    template<
        typename CharT, typename TokenType, bool MayThrow = true,
        bool AllowStringLiterals = true, bool AllowComments = true,
        DotItemInputWay InputWay = GLOBAL, size_t MaxSize4Terminals = 4,
        size_t MaxInputIndex = 8>// used for threading
    class DotItem
    {
        static_assert(
            MaxInputIndex > 0,
            "max number of input indexes must be greater than 0");

    public:
        using StringView     = std::basic_string_view<CharT>;
        using StringIterator = typename StringView::iterator;
        using StrChecker     = StringChecker<CharT, MaxSize4Terminals, MayThrow>;
        using token_t        = Token<StringView, TokenType>;
        using result_t       = gl::Set<token_t, 2, MayThrow>;

    private:
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
                return bitmap.template set<Value, 0>(to_unsigned(index));
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

        public:
            ItemRule rule{};
            u32 times{};
            ConstBitmap<1, StrChecker::MaxChars> bitmap{};
        };

        using storage_t = gl::Set<Range, cerb::lex::MAX_RANGES, MayThrow>;
        using iterator  = typename storage_t::iterator;

    private:
        size_t m_dot{};
        size_t m_input_index{};
        TokenType m_token_type{};
        StringView m_input{};

        storage_t m_ranges{};
        iterator m_current{ m_ranges.begin() };

        PositionInFile<char> m_token_pos{};
        PositionInFile<char> m_current_pos{};

        StringIterator m_token_begin{};

        const StringView m_single_line_comment{};
        const StringView m_multiline_comment_begin{};
        const StringView m_multiline_comment_end{};

        static inline StrChecker m_checker{};
        static inline std::array<StringView, MaxInputIndex> m_shared_input{};

    public:
        result_t result_of_check{};

    public:
        constexpr auto get_input() -> StringView &
        {
            if constexpr (InputWay == LOCAL) {
                return m_input;
            }
            if constexpr (InputWay == INDEXED) {
                return m_shared_input[m_input_index];
            }
            return m_shared_input[0];
        }

        constexpr auto get_input() const -> StringView &
        {
            if constexpr (InputWay == LOCAL) {
                return m_input;
            }
            if constexpr (InputWay == INDEXED) {
                return m_shared_input[m_input_index];
            }
            return m_shared_input[0];
        }

        template<size_t Offset = 0>
        constexpr auto get_char() -> CharT
        {
            return get_input()[m_dot + Offset];
        }

        constexpr auto get_char(size_t offset) const -> CharT
        {
            return get_input()[m_dot + offset];
        }

        template<typename T>
        constexpr static auto char_cast(T value) -> CharT
        {
            return static_cast<CharT>(value);
        }

        constexpr auto add2input(StringIterator first) -> void
        {
            auto &input = get_input();
            get_input() = { min(first, input.end()), input.end() };
        }

        constexpr auto add2input(size_t offset) -> void
        {
            auto &input = get_input();
            get_input() = { min(input.begin() + offset, input.end()), input.end() };
        }

        constexpr auto add2input_ans_set(size_t offset) -> void
        {
            auto &input = get_input();
            get_input() = { min(input.begin() + offset, input.end()), input.end() };
            set_input(get_input());
        }

        constexpr auto add2input_ans_set(StringIterator first) -> void
        {
            auto &input = get_input();
            get_input() = { min(first, input.end()), input.end() };
            set_input(get_input());
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

    private:
        enum SkipStatus
        {
            EMPTY,
            SINGLE_LINE_COMMENT,
            MULTILINE_COMMENT
        };

        constexpr auto skip_comments_and_layout() -> void
        {
            SkipStatus status = EMPTY;

            CERBLIB_UNROLL_N(2)
            while (!get_input().empty()) {
                // we will check for comments if there are allowed
                if constexpr (AllowComments) {
                    if (status ==
                        EMPTY) {// if it's not a comment we need to check for it
                        // single line comment
                        if (check_substring(0, get_input(), m_single_line_comment)) {
                            status = SINGLE_LINE_COMMENT;
                            add2input(m_single_line_comment.size());
                            m_current_pos.char_number +=
                                m_single_line_comment.size();
                            continue;
                        }
                        // multiline comment
                        if (check_substring(
                                0, get_input(), m_multiline_comment_begin)) {
                            status = MULTILINE_COMMENT;
                            add2input(m_multiline_comment_begin.size());
                            m_current_pos.char_number +=
                                m_multiline_comment_begin.size();
                            continue;
                        }
                    } else if (// if it is the end of multiline comment we need to
                               // finish it
                        status == MULTILINE_COMMENT &&
                        check_substring(0, get_input(), m_multiline_comment_end)) {
                        status = EMPTY;
                        add2input(m_multiline_comment_begin.size());
                        m_current_pos.char_number +=
                            m_multiline_comment_begin.size();
                        continue;
                    } else {// if we are inside a comment
                        if (get_char() == '\n') {
                            m_current_pos.char_number = 0;
                            ++m_current_pos.line_number;
                            status =
                                cmov(status == SINGLE_LINE_COMMENT, EMPTY, status);
                        } else {
                            ++m_current_pos.char_number;
                        }
                        add2input(1);
                        continue;
                    }
                }

                if (is_layout(get_char())) {
                    if (get_char() == '\n') {
                        m_current_pos.char_number = 0;
                        ++m_current_pos.line_number;
                        // this check won't acquire but just in case
                        status = cmov(status == SINGLE_LINE_COMMENT, EMPTY, status);
                    } else {
                        ++m_current_pos.char_number;
                    }
                    add2input(1);
                    continue;
                }
                return;
            }
        }

    public:
        constexpr auto set_input(StringView input) -> void
        {
            get_input() = input;
            skip_comments_and_layout();
        }

        constexpr auto set_checker(const StrChecker &checker) -> void
        {
            m_checker = checker;
        }

        constexpr auto rebind(StringView filename, bool reload_position = true)
            -> void
        {
            if (reload_position) {
                m_current_pos = { 0, 0, filename };
            }

            m_dot         = 0;
            m_token_pos   = m_current_pos;
            m_current     = m_ranges.begin();
            m_token_begin = get_input().begin();
        }

        constexpr auto get_dot() const -> size_t
        {
            return m_dot;
        }

        constexpr auto rebind(bool reload_position = false) -> void
        {
            if (reload_position) {
                m_current_pos = { 0, 0, m_current_pos.filename };
            }

            m_dot         = 0;
            m_token_pos   = m_current_pos;
            m_current     = m_ranges.begin();
            m_token_begin = get_input().begin();

            CERBLIB_UNROLL_N(2)
            for (auto &elem : m_ranges) {
                elem.rebind();
            }
        }

        constexpr auto empty() const -> bool
        {
            return get_input().empty();
        }

        constexpr auto can_end() const -> bool
        {
            CERBLIB_UNROLL_N(2)
            for (auto i = m_ranges.end() - 1; i != m_current; --i) {
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

        constexpr auto check() -> ItemState
        {
            bool layout        = is_layout(get_char());
            auto terminal_repr = m_checker.check(m_dot, get_input());

            /*
             * if we have layout char, or it is 'end' of file, or it is a comment we
             * need to stop checking
             */
            if (layout || get_char() == char_cast(0) ||
                check_substring(m_dot, get_input(), m_single_line_comment) ||
                check_substring(m_dot, get_input(), m_multiline_comment_begin)) {
                if (get_char() == '\n') {
                    m_current_pos.char_number = 0;
                    ++m_current_pos.line_number;
                } else {
                    ++m_current_pos.char_number;
                }

                // if it is not an empty token we need to return it
                if (m_token_begin != get_input().begin() + m_dot) {
                    StringView repr = { m_token_begin, m_token_begin + m_dot };
                    result_of_check = { { repr, m_token_type, m_token_pos } };
                    return SCAN_FINISHED;
                }

                return UNABLE_TO_MATCH;
            }

            if (!terminal_repr.empty()) {
                if (m_token_begin == m_token_begin + m_dot) {
                    m_current_pos.char_number += terminal_repr.size();
                    StringView repr = { m_token_begin,
                                        m_token_begin + terminal_repr.size() };
                    result_of_check = { { repr, 1, m_token_pos } };
                    return SCAN_FINISHED;
                }

                if ((m_current->can_end() && can_end()) ||
                    m_current == m_ranges.end()) {
                    StringView repr1 = { m_token_begin, m_token_begin + m_dot };
                    StringView repr2 = { repr1.end(),
                                         repr1.end() + terminal_repr.size() };
                    result_of_check  = {
                        { repr1, m_token_type, m_token_pos },
                        { repr2, 1, m_current_pos }// check for errors
                    };
                    m_current_pos.char_number += terminal_repr.size();
                    return SCAN_FINISHED;
                }
                return UNABLE_TO_MATCH;
            }

            ItemState state = m_current->check(get_char());
            ++m_current_pos.char_number;

            switch (state) {
            case NEED_TO_SCAN:
                ++m_dot;
                return NEED_TO_SCAN;

            case NEED_TO_SWITCH_RANGE_AND_CHAR:
                ++m_dot;
                [[fallthrough]];

            case NEED_TO_SWITCH_RANGE:
                ++m_current;
                return NEED_TO_SCAN;

            default:
                return UNABLE_TO_MATCH;
            }
        }

    public:
        constexpr DotItem(
            TokenType token_type, StringView input, size_t input_index = 0,
            const StringView single_line_comment     = "//"sv,
            const StringView multiline_comment_begin = "/*"sv,
            const StringView multiline_comment_end   = "*/"sv)
          : m_input_index(input_index), m_token_type(token_type),
            m_single_line_comment(single_line_comment),
            m_multiline_comment_begin(multiline_comment_begin),
            m_multiline_comment_end(multiline_comment_end)
        {
            bool is_range = false;// if we are looking for a range of chars
            bool are_brackets_opened = false;       // if range brackets are opened
            auto letter              = char_cast(0);// previous letter
            iterator current_range   = m_ranges.begin();// current range

            for (const auto elem : input) {
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

        consteval DotItem(
            bool constEval, TokenType token_type, StringView input,
            size_t input_index = 0, const StringView single_line_comment = "//"sv,
            const StringView multiline_comment_begin = "/*"sv,
            const StringView multiline_comment_end   = "*/"sv)
          : DotItem(
                token_type, input, input_index, single_line_comment,
                multiline_comment_begin, multiline_comment_end)
        {}
    };

    template<
        typename CharT, typename TokenType, bool MayThrow = true,
        bool AllowStringLiterals = true, bool AllowComments = true,
        DotItemInputWay InputWay = GLOBAL, size_t MaxSize4Terminals = 4,
        size_t MaxInputIndex = 8>
    class DotItemController
    {
        using Item = DotItem<
            CharT, TokenType, MayThrow, AllowStringLiterals, AllowComments, InputWay,
            MaxSize4Terminals, MaxInputIndex>;
        using StringView = typename Item::StringView;
        using StrChecker = typename Item::StrChecker;

        std::list<Item> m_items{};

    public:
        constexpr auto scan(const StringView &input) -> void
        {
            m_items.begin()->set_input(input);
            std::list<typename Item::result_t> results{};
            Item *item{ nullptr };

            while (!m_items.begin()->empty()) {
                ItemState state = UNABLE_TO_MATCH;
                for (auto &elem : m_items) {
                    elem.rebind();
                    do {
                        state = elem.check();
                    } while (state != UNABLE_TO_MATCH && state != SCAN_FINISHED);

                    if (state == SCAN_FINISHED &&
                        (item == nullptr || item->get_dot() > item->get_dot())) {
                        item = &elem;
                    }
                }

                if (item != nullptr) {
                    results.template emplace_back(item->result_of_check);
                    m_items.begin()->add2input_ans_set(
                        item->result_of_check.get_last()->repr.end());
                } else {
                    m_items.begin()->add2input_ans_set(static_cast<size_t>(0));
                }
                item = nullptr;
            }

            for (const auto &i : results) {
                for (auto &elem : i) {
                    std::cout << std::setw(8) << std::left << elem.repr << ' '
                              << elem.type << ' ' << elem.pos.char_number << ' '
                              << elem.pos.line_number << std::endl;
                }
            }
        }

    public:
        constexpr DotItemController(
            const std::initializer_list<Pair<TokenType, const StringView>> &rules,
            const StrChecker &checker, size_t input_index = 0,
            const StringView single_line_comment     = "//"sv,
            const StringView multiline_comment_begin = "/*"sv,
            const StringView multiline_comment_end   = "*/"sv)
        {
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : rules) {
                m_items.push_back({ elem.first, elem.second, input_index,
                                    single_line_comment, multiline_comment_begin,
                                    multiline_comment_end });
            }
            m_items.begin()->set_checker(checker);
        }
    };
}// namespace cerb::lex::experimental

#endif /* CERBERUS_DOT_ITEM_HPP */
