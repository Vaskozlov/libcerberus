#ifndef CERBERUS_DOT_ITEM_HPP
#define CERBERUS_DOT_ITEM_HPP

#include <cerberus/map.hpp>
#include <cerberus/set.hpp>
#include <cerberus/vector.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/lex/file.hpp>
#include <cerberus/lex/token.hpp>
#include <cerberus/lex/char.hpp>
#include <iostream>

namespace cerb::lex {
    constexpr size_t MAX_RANGES                           = 8;
    constexpr size_t MAX_OPTIONALS                        = 10;
    constexpr size_t AVAILABLE_CHARS                      = 128;
    constexpr size_t MAX_LENGTH_FOR_STRONG_ELEMS          = 4;
    constexpr size_t DOT_ITEM_MAX_NUMBER_OF_INDEXED_INPUT = 8;

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
        SCAN_FINISHED,
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

    template<size_t Size = MAX_LENGTH_FOR_STRONG_ELEMS, bool MayThrow = true>
    struct StringChecker
    {
        using BitmapType = ConstBitmap<1, AVAILABLE_CHARS>;
        using storage_t  = std::array<BitmapType, Size>;

    private:
        storage_t bitmaps{};

    public:
        constexpr auto check(char input) -> bool
        {
            return static_cast<bool>(bitmaps[0].template at<0>(input));
        }

        constexpr auto check(size_t index, const std::string_view &input)
            -> std::string_view
        {
            size_t i = 0;

            CERBLIB_UNROLL_N(2)
            for (; i < bitmaps.size(); ++i) {
                if (bitmaps[i].template at<0>(input[index + i]) == 0) {
                    break;
                }
            }

            return { input.begin(), input.begin() + i };
        }

    public:
        consteval StringChecker(
            const std::initializer_list<const char> &chars,
            const std::initializer_list<const std::string_view> &strings)
        {
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : chars) {
                bitmaps[0].template set<1, 0>(elem);
            }

            for (const auto &elem : strings) {
                size_t counter = 0;

                if constexpr (MayThrow) {
                    if (Size < elem.size()) {
                        throw std::out_of_range(
                            "StringChecker can't contains such a long "
                            "string...");
                    }
                }
                CERBLIB_UNROLL_N(2)
                for (auto chr : elem) {
                    bitmaps[counter++].template set<1, 0>(chr);
                }
            }
        }
    };

    template<
        typename Typename4Tokens,
        bool ShredObject         = true,
        bool MayThrow            = true,
        bool AllowStringLiterals = true,
        size_t StrictTokenSize   = MAX_LENGTH_FOR_STRONG_ELEMS>
    struct DotItem
    {
        struct Range
        {
            constexpr auto rebind() -> void
            {
                times = 0;
            }

            constexpr auto canEnd() -> bool
            {
                switch (rule) {
                case BASIC:
                    return cmov(times == 1, true, false);

                case OPTIONAL:
                    return cmov(times <= 1, true, false);

                case ONE_OR_MORE_TIMES:
                    return cmov(times > 0, true, false);

                case ZERO_OR_MORE_TIMES:
                    return true;

                default:
                    return false;
                }
            }

            /*
             * check method should not get layout chars as an input
             *
             */
            constexpr auto check(char elem) -> ItemState
            {
                u8 contains =
                    bitmap.template at<0>(cerb::bit_cast<unsigned char>(elem));

                if (contains == 1) {
                    ++times;

                    switch (rule) {
                    case BASIC:
                    case OPTIONAL:
                        return NEED_TO_SWITCH_RANGE;

                    default:
                        return NEED_TO_SCAN;
                    }
                }

                switch (rule) {
                case BASIC:
                    return UNABLE_TO_MATCH;

                case ONE_OR_MORE_TIMES:
                    return cmov(times > 0, NEED_TO_SWITCH_RANGE, UNABLE_TO_MATCH);

                default:
                    return NEED_TO_SWITCH_RANGE;
                }
            }

        public:
            u32 times{ 0 };
            ItemRule rule{ BASIC };
            ConstBitmap<1, AVAILABLE_CHARS> bitmap{};
        };

    private:
        using storage_t = gl::Set<Range, MAX_RANGES>;
        using iterator  = typename storage_t::iterator;

    public:// make it private later
        bool isStringToken = false;
        Typename4Tokens m_class_of_token{};
        size_t m_dot{ 0 };
        size_t m_line{ 0 };
        size_t m_char{ 0 };
        storage_t m_ranges{};
        PositionInFile m_position{};
        std::string_view m_input{};
        iterator m_current{ m_ranges.begin() };
        StringChecker<StrictTokenSize, MayThrow> m_strong_tokens;

        inline static size_t m_shred_dot{ 0 };
        inline static size_t m_shred_line{ 0 };
        inline static size_t m_shred_char{ 0 };
        inline static PositionInFile m_shared_position{ 0, 0, "" };
        inline static std::string_view m_shared_input{};

    private:
        constexpr auto dot() -> size_t &
        {
            if constexpr (ShredObject) {
                return m_shred_dot;
            }
            return m_dot;
        }

        constexpr auto line() -> size_t &
        {
            if constexpr (ShredObject) {
                return m_shred_line;
            }
            return m_line;
        }

        constexpr auto char_number() -> size_t &
        {
            if constexpr (ShredObject) {
                return m_shred_char;
            }
            return m_char;
        }

        constexpr auto change_input(size_t offset) -> void
        {
            get_input() = { min(get_input().begin() + offset, get_input().end()),
                            get_input().end() };
        }

        constexpr auto change_input(std::string_view::iterator begin) -> void
        {
            get_input() = { min(begin, get_input().end()), get_input().end() };
        }

    public:
        constexpr auto get_input() -> std::string_view &
        {
            if constexpr (ShredObject) {
                return m_shared_input;
            }
            return m_input;
        }

        constexpr auto position() -> PositionInFile &
        {
            if constexpr (ShredObject) {
                return m_shared_position;
            }
            return m_position;
        }

    private:
        constexpr auto update_position() -> PositionInFile
        {
            PositionInFile copy = position();
            position()          = { line(), char_number(), copy.filename };
            return copy;
        }

        constexpr auto canEnd() -> bool
        {
            CERBLIB_UNROLL_N(2)
            for (iterator begin = m_ranges.end() - 1; begin != m_current; --begin) {
                switch (begin->rule) {
                case BASIC:
                case ONE_OR_MORE_TIMES:
                    return false;

                default:
                    break;
                }
            }

            return true;
        }

        constexpr auto end_scan(size_t offset, bool layout = false)
            -> Token<std::string_view, Typename4Tokens>
        {
            auto casted_layout = static_cast<unsigned>(layout);
            auto begin_of_rep  = get_input().begin();
            auto end_of_repr = min(get_input().begin() + offset, get_input().end());

            if (get_input()[offset] == '\n') {
                line()++;
                char_number() = 0;
            }

            dot() = 0;
            change_input(end_of_repr + casted_layout);

            return { { begin_of_rep, end_of_repr }, m_class_of_token, position() };
        }

        constexpr auto convert_result(
            bool layout,
            const std::string_view &strong = "")
            -> Pair<ItemState, gl::Set<Token<std::string_view, Typename4Tokens>, 2>>
        {
            if (m_current->canEnd() && canEnd()) {
                if (!strong.empty()) {
                    return { SCAN_FINISHED,
                             { end_scan(dot()), end_scan(strong.size()) } };
                }
                return { SCAN_FINISHED, { end_scan(dot()++, layout) } };
            }
            return { UNABLE_TO_MATCH, {} };
        }

        constexpr auto next_line_check() -> bool
        {
            if (!isStringToken) {
                if (get_input()[dot()] == '\\' && (get_input()[dot() + 1] == '\n' ||
                                                   get_input()[dot() + 1] == '\r')) {
                    dot() += 2;
                    CERBLIB_UNROLL_N(2)
                    while (is_layout(get_input()[dot()])) {
                        if (get_input()[dot()] == '\\') {
                            return next_line_check();
                        }

                        if (is_layout(get_input()[dot()]) == '\n' ||
                            is_layout(get_input()[dot()]) == '\r') {
                            return false;
                        }

                        ++dot();
                    }
                }
            }
            return true;
        }

    public:
        constexpr auto rebind() -> void
        {
            m_current = m_ranges.begin();

            CERBLIB_UNROLL_N(2)
            for (auto &elem : m_ranges) {
                elem.rebind();
            }
        }

        constexpr auto
            set(const std::string_view &input, const std::string_view &filename)
                -> void
        {
            rebind();

            dot()         = 0;
            line()        = 0;
            char_number() = 0;
            get_input()   = input;
            position()    = { 0, 0, filename };
        }

        constexpr auto check()
            -> Pair<ItemState, gl::Set<Token<std::string_view, Typename4Tokens>, 2>>
        {
            if (!next_line_check()) {
                return { UNABLE_TO_MATCH, {} };
            }
            if (get_input()[dot()] == '\n') {
                ++line();
                char_number() = 0;
            }

            bool layout = is_layout(get_input()[dot()]);
            auto strong = m_strong_tokens.check(dot(), get_input());

            if (((layout || get_input()[dot()] == '\0') && dot() != 0) ||
                strong.size() != 0) {
                return convert_result(layout, strong);
            }

            CERBLIB_UNROLL_N(2)
            while (is_layout(get_input()[dot()++])) {}

            if (get_input()[dot()] == '\0') {
                change_input(1);
                return { OUT_OF_ELEMS, {} };
            }
            if (dot() >= get_input().size()) {
                change_input(get_input().end());
                return { OUT_OF_ELEMS, {} };
            }

            auto state = m_current->check(get_input()[dot()]);

            switch (state) {
            case NEED_TO_SCAN:
                ++dot();
                return { NEED_TO_SCAN, {} };

            case NEED_TO_SWITCH_RANGE:
                ++dot();
                ++m_current;
                return { NEED_TO_SCAN, {} };

            case SCAN_FINISHED:
                return convert_result(false);

            case UNABLE_TO_MATCH:
                return { UNABLE_TO_MATCH, {} };
            }

            ++char_number();
        }

    public:
        using CheckResult =
            Pair<ItemState, gl::Set<Token<std::string_view, Typename4Tokens>, 2>>;

    public:
        constexpr DotItem(
            Typename4Tokens type,
            const std::string_view &input,
            StringChecker<StrictTokenSize, MayThrow>
                checker)
          : m_class_of_token(type), m_strong_tokens(std::move(checker))
        {
            char letter            = '\0';
            bool range_started     = false;
            bool is_range_of_elems = false;
            iterator current_range = m_ranges.begin();

            for (const auto elem : input) {
                switch (elem) {
                case '[':
                    if constexpr (MayThrow) {
                        if (range_started) {
                            throw std::runtime_error("Two ranges were opened!");
                        }
                    }

                    letter        = '\0';
                    range_started = true;
                    current_range = m_ranges.last();
                    break;

                case ']':
                    if constexpr (MayThrow) {
                        if (!range_started) {
                            throw std::runtime_error(
                                "There are not any opened ranges!");
                        }
                    }

                    if (letter != '\0') {
                        current_range->bitmap.template set<1, 0>(letter);
                    }

                    range_started = false;
                    break;

                case '-':
                    is_range_of_elems = true;
                    break;

                case '?':
                    current_range->rule = OPTIONAL;
                    break;

                case '*':
                    current_range->rule = ZERO_OR_MORE_TIMES;
                    break;

                case '+':
                    current_range->rule = ONE_OR_MORE_TIMES;
                    break;

                default:
                    if (letter != '\0' && is_range_of_elems) {
                        CERBLIB_UNROLL_N(2)
                        for (; letter <= elem; ++letter) {
                            current_range->bitmap.template set<1, 0>(letter);
                        }
                    } else if (letter != '\0') {
                        current_range->bitmap.template set<1, 0>(letter);
                    }

                    letter            = elem;
                    is_range_of_elems = false;

                    break;
                }
            }
            return;
        }
    };

    enum DotItemInputWay
    {
        LOCAL,
        GLOBAL,
        INDEXED
    };

    template<
        typename TokenType,
        DotItemInputWay InputWay = GLOBAL,
        bool MayThrow            = true,
        bool AllowStringLiterals = true,
        bool AllowComments       = true,
        size_t StrictTokenSize   = MAX_LENGTH_FOR_STRONG_ELEMS>
    class DotItem2
    {
        struct Range
        {
            constexpr auto rebind() -> void
            {
                times = 0;
            }

            constexpr auto mayEnd() -> bool
            {
                switch (rule) {
                case BASIC:
                    return times == 1;

                case OPTIONAL:
                    return times <= 1;

                case ONE_OR_MORE_TIMES:
                    return times >= 1;

                default:
                    return true;
                }
            }

            constexpr auto check(char elem) -> ItemState
            {
                u8 contains = bitmap.template at<0>(bit_cast<u8>(elem));

                if (contains == 1) {
                    ++times;

                    switch (rule) {
                    case BASIC:
                    case OPTIONAL:
                        return NEED_TO_SWITCH_RANGE;

                    default:
                        return NEED_TO_SCAN;
                    }
                }

                switch (rule) {
                case BASIC:
                    return UNABLE_TO_MATCH;

                case ONE_OR_MORE_TIMES:
                    return cmov(times >= 1, NEED_TO_SWITCH_RANGE, UNABLE_TO_MATCH);

                default:// OPTIONAL ZERO_OR_MORE_TIMES
                    return NEED_TO_SWITCH_RANGE;
                }
            }

        public:
            ItemRule rule{ BASIC };
            size_t times{};
            ConstBitmap<1, AVAILABLE_CHARS> bitmap{};
        };

    public:
        constexpr auto get_input() -> std::string_view &
        {
            if constexpr (InputWay == GLOBAL) {
                return m_shared_input;
            }
            if constexpr (InputWay == INDEXED) {
                return m_input[m_input_index];
            }
            return m_input[0];
        }

        constexpr auto change_input(size_t offset) -> void
        {
            get_input() = { min(get_input().begin() + offset, get_input().end()),
                            get_input().end() };
        }

        constexpr auto change_input(std::string_view::iterator begin) -> void
        {
            get_input() = { min(begin, get_input().end()), get_input().end() };
        }

        constexpr auto get_char(size_t offset) -> char
        {
            return get_input()[m_dot + offset];
        }

        template<size_t Offset = 0>
        constexpr auto get_char() -> char
        {
            return get_input()[m_dot + Offset];
        }

        constexpr auto rebind() -> void
        {
            m_current = m_ranges.begin();

            CERBLIB_UNROLL_N(2)
            for (auto &elem : m_ranges) {
                elem.rebind();
            }
        }

        constexpr auto
            set(const std::string_view &input, const std::string_view &filename)
                -> void
        {
            m_dot         = 0;
            m_token_pos   = { 0, 0, filename };
            m_current_pos = { 0, 0, filename };

            rebind();
            get_input() = input;
        }

    public:
        using storage_t = gl::Set<Range, MAX_RANGES>;
        using iterator  = typename storage_t::iterator;
        using ResultType =
            Pair<ItemState, gl::Set<Token<std::string_view, TokenType>, 2>>;

    public:// make it private later
        size_t m_dot{};
        size_t m_token_size{};
        size_t m_input_index{};
        TokenType m_token_class{};
        storage_t m_ranges{};
        iterator m_current{ m_ranges.begin() };
        PositionInFile m_token_pos{};
        PositionInFile m_current_pos{};
        StringChecker<StrictTokenSize, MayThrow> m_strong_tokens{};
        std::array<std::string_view, DOT_ITEM_MAX_NUMBER_OF_INDEXED_INPUT> m_input{};

        static inline std::string_view m_shared_input{};

    private:
        constexpr auto skip_comments() -> void
        {
            if (get_char() == '/' && get_char<1>() == '/') {
                CERBLIB_UNROLL_N(2)
                while (get_char() != '\n' && get_char() != '\0') {
                    ++m_dot;
                    ++m_current_pos.char_number;
                }
                if (get_char() == '\n') {
                    ++m_dot;
                    ++m_current_pos.line_number;
                    m_current_pos.char_number = 0;
                }
            } else if (get_char() == '/' && get_char<1>() == '*') {
                CERBLIB_UNROLL_N(2)
                while (get_char() != '\0' &&
                       (get_char() != '*' && get_char<1>() != '\\')) {
                    ++m_dot;
                    ++m_current_pos.char_number;

                    if (get_char() == '\n') {
                        ++m_current_pos.line_number;
                        m_current_pos.char_number = 0;
                    }
                }
            }
        }

        constexpr auto skip_layout() -> void
        {
            CERBLIB_UNROLL_N(2)
            while (is_layout(get_char())) {
                ++m_dot;// optionally we can change the input, so other dot items
                        // won't need to skip layout
                if (get_char() == '\n') {
                    ++m_current_pos.line_number;
                    m_current_pos.char_number = 0;
                } else {
                    ++m_current_pos.char_number;
                }
            }
        }

    public:
        constexpr auto check() -> ResultType
        {
            if (get_char() == '\n') {
                m_current_pos.char_number = 0;
                ++m_current_pos.line_number;
            }

            if (m_token_size == 0) {
                skip_layout();
                if constexpr (AllowComments) {
                    skip_comments();
                }
                m_token_pos = m_current_pos;
            } else {
                bool layout = is_layout(get_char());
                auto strong = m_strong_tokens.check(m_dot, get_input());

                if (layout || get_char() == '\0') {
                    if (m_token_size != 0) {
                        // return result
                    }
                    return { UNABLE_TO_MATCH, {} };
                }
                if (strong.size() != 0) {
                    if (m_token_size != 0) {
                        // return result
                    }
                    // complete it later
                    return { SCAN_FINISHED, {} };
                }
            }
            ++m_current_pos.char_number;

            if (get_char() == '\0' || m_dot >= get_input().size()) {
                change_input(1);
                return { OUT_OF_ELEMS, {} };
            }

            auto state = m_current->check(get_char());

            switch (state) {
            case NEED_TO_SCAN:
                ++m_dot;
                ++m_token_size;
                return { NEED_TO_SCAN, {} };

            case NEED_TO_SWITCH_RANGE:
                ++m_dot;
                ++m_current;
                return {
                    cmov(m_current == m_ranges.end(), UNABLE_TO_MATCH, NEED_TO_SCAN),
                    {}
                };

            case SCAN_FINISHED:
                return { SCAN_FINISHED, {} };

            default:
                return { UNABLE_TO_MATCH, {} };
            }
        }

    public:
        constexpr DotItem2(
            TokenType type,
            const std::string_view &input,
            StringChecker<StrictTokenSize, MayThrow>
                checker)
          : m_token_class(type), m_strong_tokens(std::move(checker))
        {
            char letter            = '\0';
            bool range_started     = false;
            bool is_range_of_elems = false;
            iterator current_range = m_ranges.begin();

            for (const auto elem : input) {
                switch (elem) {
                case '[':
                    if constexpr (MayThrow) {
                        if (range_started) {
                            throw std::runtime_error("Two ranges were opened!");
                        }
                    }

                    letter        = '\0';
                    range_started = true;
                    current_range = m_ranges.last();
                    break;

                case ']':
                    if constexpr (MayThrow) {
                        if (!range_started) {
                            throw std::runtime_error(
                                "There are not any opened ranges!");
                        }
                    }

                    if (letter != '\0') {
                        current_range->bitmap.template set<1, 0>(letter);
                    }

                    range_started = false;
                    break;

                case '-':
                    is_range_of_elems = true;
                    break;

                case '?':
                    current_range->rule = OPTIONAL;
                    break;

                case '*':
                    current_range->rule = ZERO_OR_MORE_TIMES;
                    break;

                case '+':
                    current_range->rule = ONE_OR_MORE_TIMES;
                    break;

                default:
                    if (letter != '\0' && is_range_of_elems) {
                        CERBLIB_UNROLL_N(2)
                        for (; letter <= elem; ++letter) {
                            current_range->bitmap.template set<1, 0>(letter);
                        }
                    } else if (letter != '\0') {
                        current_range->bitmap.template set<1, 0>(letter);
                    }

                    letter            = elem;
                    is_range_of_elems = false;

                    break;
                }
            }
            return;
        }
    };
}// namespace cerb::lex

namespace cerb::lex::experimental {
    constexpr size_t MAX_RANGES = 10;
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
        constexpr auto check(CharT elem) -> bool
        {
            return static_cast<bool>(m_bitmaps[0].template at<0>(elem));
        }

        constexpr auto check(size_t index, const StringView &str) -> StringView
        {
            size_t i = 0;

            CERBLIB_UNROLL_N(2)
            for (; i < m_bitmaps.size(); ++i) {
                if (m_bitmaps[i].template at<0>(str[index + i]) == 0) {
                    break;
                }
            }

            if (i == str.size()) {
                return { str.begin(), str.begin() + i };
            }

            return { str.begin(), str.begin() };
        }

    public:
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
        typename CharT,
        typename TokenType,
        bool MayThrow                = true,
        bool AllowStringLiterals     = true,
        bool AllowComments           = true,
        DotItemInputWay InputWay     = GLOBAL,
        size_t MaxSize4StringChecker = 4,
        size_t MaxInputIndex         = 8>
    class DotItem
    {
        static_assert(MaxInputIndex > 0);
        using StrChecker = StringChecker<CharT, MaxSize4StringChecker, MayThrow>;

        struct Range
        {
            using StorageType = ConstBitmap<1, StrChecker::MaxChars>;

        public:
            constexpr auto at(CharT index) const -> u8
            {
                return bitmap.template at<0>(to_unsigned(index));
            }

            constexpr auto operator[](CharT index) const -> u8
            {
                return bitmap.template at<0>(to_unsigned(index));
            }

            template<u8 value>
            constexpr auto set(CharT index) -> void
            {
                return bitmap.template set<value, 0>(to_unsigned(index));
            }

            constexpr auto rebind() -> void
            {
                times = 0;
            }

        public:
            ItemRule rule{ BASIC };
            size_t times{};
            StorageType bitmap{};
        };

        using StorageType        = gl::Set<Range, experimental::MAX_RANGES>;
        using StringView         = typename StrChecker::StringView;
        using iterator           = typename StorageType::iterator;
        using StringViewIterator = typename StringView::iterator;
        using ResultType =
            Pair<ItemState, gl::Set<Token<std::string_view, TokenType>, 2>>;

    private:
        bool m_is_string_token{ false };
        size_t m_dot{};
        size_t m_input_index{};
        StrChecker m_checker;
        TokenType m_token_type{};
        StringViewIterator m_token_begin{};
        StorageType m_ranges{};
        iterator m_current{ m_ranges.begin() };
        PositionInFile m_token_position{};
        PositionInFile m_current_position{};
        std::array<StringView, MaxInputIndex> m_input{};

        StringView m_sing_line_comment_begin{};
        StringView m_multiline_comment_begin{};
        StringView m_multiline_comment_end{};

        static inline StringView m_shared_input{};

    public:
        template<typename T>
        static constexpr auto cast(T value) -> CharT
        {
            return static_cast<CharT>(value);
        }

        constexpr auto get_input() -> StringView &
        {
            if constexpr (InputWay == GLOBAL) {
                return m_shared_input;
            }
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

        constexpr auto change_input(size_t offset) -> void
        {
            auto &input = get_input();
            get_input() = { min(input.begin() + offset, input.end()), input.end() };
        }

        constexpr auto change_input(StringViewIterator offset) -> void
        {
            auto &input = get_input();
            get_input() = { min(input.end(), offset), input.end() };
        }

    private:
        constexpr auto check_rule(iterator current_range) -> void
        {
            if constexpr (MayThrow) {
                if (current_range->rule != BASIC) {
                    throw std::runtime_error("You must not change rule of range!");
                }
            }
        }

    public:
        constexpr auto check() -> ResultType
        {
            return { UNABLE_TO_MATCH, {} };
        }

        constexpr auto rebind() -> void
        {
            m_current        = m_ranges.begin();
            m_token_begin    = get_input().begin();
            m_token_position = m_current_position;

            CERBLIB_UNROLL_N(2)
            for (auto &elem : m_ranges) {
                elem.rebind();
            }
        }

        constexpr auto set(const StringView &input) -> void
        {
            get_input() = input;
        }

    public:
        constexpr DotItem(
            TokenType token_type,
            const StringView &input,
            StrChecker checker,
            size_t input_index                        = 0,
            const StringView &sing_line_comment_begin = "//"sv,
            const StringView &multiline_comment_begin = "/*"sv,
            const StringView &multiline_comment_end   = "*\\"sv)
          : m_input_index(input_index), m_checker(checker), m_token_type(token_type),
            m_sing_line_comment_begin(sing_line_comment_begin),
            m_multiline_comment_begin(multiline_comment_begin),
            m_multiline_comment_end(multiline_comment_end)
        {
            bool is_range          = false;
            bool opened_brackets   = false;
            auto letter            = cast(0);
            iterator current_range = m_ranges.begin();

            for (const auto elem : input) {
                switch (elem) {
                case '[':
                    if constexpr (MayThrow) {
                        if (opened_brackets) {
                            throw std::runtime_error(
                                "You can't open more than range!");
                        }
                    }

                    letter          = cast(0);
                    opened_brackets = true;
                    current_range   = m_ranges.last();
                    break;

                case ']':
                    if constexpr (MayThrow) {
                        if (!opened_brackets) {
                            throw std::runtime_error("You must open bracket first!");
                        }
                    }

                    if (letter == cast(0)) {
                        current_range->template set<1>(letter);
                    }

                    opened_brackets = false;
                    break;

                case '-':
                    if constexpr (MayThrow) {
                        if (is_range) {
                            throw std::runtime_error(
                                "Close range before opening another one");
                        }
                    }

                    is_range = true;
                    break;

                case '+':
                    check_rule(current_range);
                    current_range->rule = ONE_OR_MORE_TIMES;
                    break;

                case '*':
                    check_rule(current_range);
                    current_range->rule = ZERO_OR_MORE_TIMES;
                    break;

                case '?':
                    check_rule(current_range);
                    current_range->rule = OPTIONAL;
                    break;

                default:
                    if (letter != cast(0) && is_range) {
                        CERBLIB_UNROLL_N(2)
                        for (; letter <= elem; ++letter) {
                            current_range->template set<1>(letter);
                        }
                    } else if (letter != cast(0)) {
                        current_range->template set<1>(letter);
                    }

                    letter   = elem;
                    is_range = true;
                    break;
                }
            }
        }

        consteval DotItem(
            int /* unused */,
            TokenType token_type,
            const StringView &input,
            StrChecker checker,
            size_t input_index                        = 0,
            const StringView &sing_line_comment_begin = "//"sv,
            const StringView &multiline_comment_begin = "/*"sv,
            const StringView &multiline_comment_end   = "*\\"sv)
          : DotItem(
                token_type,
                input,
                checker,
                input_index,
                sing_line_comment_begin,
                multiline_comment_begin,
                multiline_comment_end)
        {}
    };
}// namespace cerb::lex::experimental

#endif /* CERBERUS_DOT_ITEM_HPP */
