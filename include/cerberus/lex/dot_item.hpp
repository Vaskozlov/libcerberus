#ifndef CERBERUS_DOT_ITEM_HPP
#define CERBERUS_DOT_ITEM_HPP

#include <cerberus/map.hpp>
#include <cerberus/set.hpp>
#include <cerberus/vector.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/lex/file.hpp>
#include <cerberus/lex/token.hpp>
#include <cerberus/lex/char.hpp>

/*
namespace cerb::lex {
    constexpr size_t MAX_RANGES      = 8;
    constexpr size_t MAX_OPTIONALS   = 10;
    constexpr size_t AVAILABLE_CHARS = 128;

    enum ItemRule : u16
    {
        BASIC              = 0b0,
        OPTIONAL           = 0b1,
        ONE_OR_MORE_TIMES  = 0b10,
        ZERO_OR_MORE_TIMES = 0b100
    };

    enum ItemState : u16
    {
        UNINITIALIZED        = 0b0,
        UNABLE_TO_MATCH      = 0b1,
        NEED_TO_SCAN         = 0b10,
        NEED_TO_SWITCH_RANGE = 0b100,
        SCAN_FINISHED        = 0b1000,
        OUT_OF_ELEMS         = 0b10000
    };

    constexpr auto cast(ItemRule value) -> u16
    {
        return static_cast<u16>(value);
    }

    constexpr auto cast(ItemState value) -> u16
    {
        return static_cast<u16>(value);
    }

    template<bool MayThrow = true>
    class DotItem
    {
        class Range
        {
            constexpr auto last_check() -> ItemState
            {
                switch (rule) {
                case BASIC:
                    return cmov(times == 1, SCAN_FINISHED, UNABLE_TO_MATCH);

                case OPTIONAL:
                    return cmov(times <= 1, SCAN_FINISHED, UNABLE_TO_MATCH);

                case ONE_OR_MORE_TIMES:
                    return cmov(times >= 1, SCAN_FINISHED, UNABLE_TO_MATCH);

                case ZERO_OR_MORE_TIMES:
                    return SCAN_FINISHED;
                }
            }

        public:
            constexpr auto check(size_t dot, const std::string_view &input) ->
ItemState
            {
                if (dot > input.size()) {
                    last_state = UNABLE_TO_MATCH;
                    return UNABLE_TO_MATCH;
                }

                switch (input[dot]) {
                case ' ' :
                case '\t':
                case '\n':
                case '\0':
                    return last_check();

                default:
                    {
                        u8 contains = bitmap.template at<0>(input[dot]);

                        if (contains == 1) {
                            ++times;

                            switch (rule) {
                            case BASIC:
                            case OPTIONAL:
                                return NEED_TO_SWITCH_RANGE;

                            case ONE_OR_MORE_TIMES:
                                return NEED_TO_SCAN;

                            case ZERO_OR_MORE_TIMES:
                                return NEED_TO_SCAN;
                            }
                        }

                        switch (rule) {
                        case BASIC:
                            return UNABLE_TO_MATCH;

                        case OPTIONAL:
                            return NEED_TO_SWITCH_RANGE;

                        case ONE_OR_MORE_TIMES:
                            return cmov(times > 0, NEED_TO_SWITCH_RANGE,
UNABLE_TO_MATCH);

                        case ZERO_OR_MORE_TIMES:
                            return NEED_TO_SWITCH_RANGE;
                        }
                    }
                    break;
                }
            }

        public:
            ItemState last_state{ UNINITIALIZED };
            u32 times{ 0 };
            ItemRule rule{};
            ConstBitmap<1, AVAILABLE_CHARS> bitmap{};
        };

        struct Optional
        {
            using storage_t      = gl::Set<Range, MAX_RANGES>;
            using iterator       = typename storage_t::iterator;
            using const_iterator = typename storage_t::const_iterator;


        private:
            constexpr auto check_ranges(ItemState state) -> ItemState
            {
                CERBLIB_UNROLL_N(1)
                while (current != ranges.end()) {
                    switch (current->rule) {
                    case BASIC:
                    case OPTIONAL:
                    case ONE_OR_MORE_TIMES:
                        return UNABLE_TO_MATCH;

                    default:
                        break;
                    }
                    current++;
                }
                return state;
            }

        public:
            constexpr auto rebind(bool keep_dot = false) -> void
            {
                dot = cmov<size_t>(keep_dot, dot, 0);
                current = ranges.begin();

                CERBLIB_UNROLL_N(2)
                for (auto &elem: ranges) {
                    elem.times = 0;
                    elem.last_state = UNINITIALIZED;
                }
            }

            constexpr auto check(const std::string_view &input) -> ItemState
            {
                ItemState state = current->check(dot, input);
                last_dot        = dot;

                switch (state) {
                case NEED_TO_SCAN:
                    ++dot;
                    return NEED_TO_SCAN;

                case NEED_TO_SWITCH_RANGE:

                    if (current->rule == BASIC || current->rule == OPTIONAL)
                    {
                        ++dot;
                        ++current;

                        switch (input[dot]) {
                        case ' ':
                        case '\t':
                        case '\n':
                        case '\0':
                            return SCAN_FINISHED;

                        default:
                            break;
                        }

                        return NEED_TO_SCAN;
                    }

                    ++current;
                    return cmov(current == ranges.end(), UNABLE_TO_MATCH,
NEED_TO_SCAN);

                case SCAN_FINISHED:
                    ++current;

                    switch (input[dot]) {
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\0':
                        dot += 2;
                        return check_ranges(SCAN_FINISHED);
                    }

                    dot += 2;
                    return cmov(current == ranges.end(), SCAN_FINISHED,
NEED_TO_SCAN);

                default:
                    ++dot;
                    return UNABLE_TO_MATCH;
                }
            }

        public:
            ItemState last_state{ UNINITIALIZED };
            size_t dot {0};
            size_t last_dot{0};
            iterator current{};
            storage_t ranges{};
        };

    private:
        using storage_t      = gl::Set<Optional, MAX_OPTIONALS>;
        using iterator       = typename storage_t::iterator;
        using const_iterator = typename storage_t::const_iterator;

    public:
        u32 class_of_token{0};
        inline static std::string_view m_input{};
        storage_t optionals{};

    private:
        constexpr auto best_option(ItemState lhs, ItemState rhs) -> ItemState
        {
            return static_cast<ItemState>(cmov(cast(lhs) > cast(rhs), lhs, rhs));
        }

    public:
        constexpr auto set(const std::string_view &input, bool keep_dot = false) ->
void
        {
            m_input = input;
            optionals.show();

            CERBLIB_UNROLL_N(2)
            for (auto &elem: optionals) {
                elem.rebind(keep_dot);
            }
        }

        constexpr auto rebind(bool keep_dot = false) -> void
        {
            optionals.show();

            CERBLIB_UNROLL_N(2)
            for (auto &elem: optionals) {
                elem.rebind(keep_dot);
            }
        }

        constexpr auto check(Vector<Pair<u32, std::string_view>> &results) ->
ItemState
        {
            ItemState final_option = UNINITIALIZED;

            for (iterator i = optionals.begin(); i != optionals.end(); ++i) {
                auto state = i->check(m_input);

                if (state == UNABLE_TO_MATCH) {
                    optionals.hide(i);
                    --i;
                } else if (state == SCAN_FINISHED) {
                    results.template emplace_back(class_of_token,
std::string_view(m_input.begin(), m_input.begin() + i->last_dot));
                }

                final_option = best_option(final_option, state);
            }

            if (final_option == SCAN_FINISHED && optionals.size() == 1) {
                auto new_iterator = m_input.begin() + optionals.begin()->dot - 1;

                if (new_iterator >= m_input.end()) {
                    return OUT_OF_ELEMS;
                }

                set({new_iterator, m_input.end()});
            }

            return cmov(optionals.size() == 0, OUT_OF_ELEMS, final_option);
        }

    public:
        constexpr auto operator=(const DotItem &) -> DotItem & = default;
        constexpr auto operator=(DotItem &&) noexcept -> DotItem & = default;

    public:
        constexpr DotItem() = default;
        constexpr DotItem(const DotItem &) = default;
        constexpr DotItem(DotItem &&) noexcept = default;

        consteval DotItem( u32 token_type, const std::string_view &input)
        : class_of_token(token_type)
        {
            char letter = '\0';
            bool range_started = false;
            bool is_range_of_elems = false;
            Range *current_range = nullptr;
            Optional *current_optional = nullptr;

            for (auto elem: input) {
                switch (elem) {
                case '[':
                    if constexpr (MayThrow) {
                        if (range_started) {
                            throw std::runtime_error("You are not allowed to open
more than one bracket ( '[' ) ");
                        }
                    }

                    if (current_optional == nullptr) {
                        current_optional = optionals.last();
                    }

                    letter = '\0';
                    range_started = true;
                    is_range_of_elems = false;
                    current_range = current_optional->ranges.last();
                    break;

                case ']':
                    range_started = false;

                    if (letter != '\0') {
                        current_range->bitmap.template set<1, 0>(letter);
                    }

                    break;

                case '-':
                    is_range_of_elems = true;
                    break;

                case '+':
                    current_range->rule = ONE_OR_MORE_TIMES;
                    break;

                case '*':
                    current_range->rule = ZERO_OR_MORE_TIMES;
                    break;

                case '?':
                    current_range->rule = OPTIONAL;
                    break;

                case '|':
                    current_optional = optionals.last();
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

                    letter = elem;
                    is_range_of_elems  = false;
                    break;;
                }
            }
            return;
        }
    };
}// namespace cerb::lex
 */

/*
namespace cerb::lex {
    constexpr size_t MAX_RANGES      = 8;
    constexpr size_t MAX_OPTIONALS   = 10;
    constexpr size_t AVAILABLE_CHARS = 128;

    enum ItemRule : u16
    {
        BASIC              = 0b0,
        OPTIONAL           = 0b1,
        ONE_OR_MORE_TIMES  = 0b10,
        ZERO_OR_MORE_TIMES = 0b100
    };

    enum ItemState : u16
    {
        UNINITIALIZED        = 0b0,
        OUT_OF_ELEMS         = 0b1,
        UNABLE_TO_MATCH      = 0b100,
        NEED_TO_SCAN         = 0b1000,
        NEED_TO_SWITCH_RANGE = 0b10000,
        SCAN_FINISHED        = 0b100000,
        LAYOUT               = 0b1000000,
    };

    constexpr auto cast(ItemRule value) -> u16
    {
        return static_cast<u16>(value);
    }

    constexpr auto cast(ItemState value) -> u16
    {
        return static_cast<u16>(value);
    }

    template<typename T>
    constexpr auto cast2state(T value) -> ItemState
    {
        return static_cast<ItemState>(value);
    }

    template<bool MayThrow = true>
    class DotItem
    {
        struct Range
        {
            constexpr auto rebind() -> void
            {
                times = 0;
            }

            constexpr auto last_check() -> ItemState
            {
                switch (rule) {
                case BASIC:
                    return cmov(times == 1, SCAN_FINISHED, UNABLE_TO_MATCH);

                case OPTIONAL:
                    return cmov(times <= 1, SCAN_FINISHED, UNABLE_TO_MATCH);

                case ONE_OR_MORE_TIMES:
                    return cmov(times > 0, SCAN_FINISHED, UNABLE_TO_MATCH);

                default:
                    return SCAN_FINISHED;
                }
            }

            constexpr auto check(char elem, char future) -> ItemState
            {
                switch (elem) {
                case ' ':
                case '\0':
                case '\t':
                case '\n':
                case '\r':
                    return last_check();

                default:
                {
                    u8 contains = bitmap.template at<0>(elem);

                    if (contains == 1) {
                        ++times;

                        switch (rule) {
                        case BASIC:
                        case OPTIONAL:
                            return NEED_TO_SWITCH_RANGE;

                        case ONE_OR_MORE_TIMES:
                        case ZERO_OR_MORE_TIMES:
                            return NEED_TO_SCAN;

                        default:
                            return UNABLE_TO_MATCH;
                        }
                    }

                    switch (rule) {
                    case BASIC:
                        return UNABLE_TO_MATCH;

                    case OPTIONAL:
                        return NEED_TO_SWITCH_RANGE;

                    case ONE_OR_MORE_TIMES:
                        return cmov(times > 0, NEED_TO_SWITCH_RANGE,
UNABLE_TO_MATCH);

                    case ZERO_OR_MORE_TIMES:
                        return NEED_TO_SWITCH_RANGE;
                    }
                } break;
                }
            }

        public:
            u32 times{0};
            ItemRule rule{BASIC};
            ConstBitmap<1, AVAILABLE_CHARS> bitmap{};
        };

    public:
        constexpr auto rebind() -> void
        {
            m_ranges.show();
            current = m_ranges.begin();

            CERBLIB_UNROLL_N(2)
            for (auto &elem: m_ranges) {
                elem.rebind();
            }
        }

        constexpr auto set(const std::string_view &input) -> void {
            m_dot = 0;
            m_input = input;
            rebind();
        }

    private:
        using storage_t = gl::Set<Range, MAX_RANGES>;
        using iterator  = typename storage_t::iterator;

    public: // make it private later
        u32 m_token_class{0};
        size_t m_dot{0};
        iterator current{};
        storage_t m_ranges{};
        static inline std::string_view m_input{};

    public:
        constexpr auto last_check() -> bool
        {
            CERBLIB_UNROLL_N(1)
            for (; current != m_ranges.end(); ++current) {
                switch (current->rule) {
                case BASIC:
                case ONE_OR_MORE_TIMES:
                    return false;
                default:
                    break;
                }
            }
            return true;
        }

        constexpr auto check(Vector<Pair<u32, std::string_view>> &results) ->
ItemState
        {
            if (current == m_ranges.end()) {
                return UNABLE_TO_MATCH;
            }

            if (m_dot == 0 && is_layout(m_input[0])) {
                m_input = std::string_view(m_input.begin() + 1, m_input.end());
                return LAYOUT;
            }

            auto state = current->check(m_input[m_dot], m_input[m_dot + 1]);

            switch (state) {
            case NEED_TO_SCAN:
                ++m_dot;
                return NEED_TO_SCAN;

            case NEED_TO_SWITCH_RANGE:
                if ((current->rule == BASIC || current->rule == OPTIONAL) &&
current->times == 1) {
                    ++m_dot;
                }

                ++current;
                return NEED_TO_SCAN;

            case SCAN_FINISHED: {
                auto point = m_dot;
                m_dot      = 0;
                results.template emplace_back(
                    m_token_class,
                    std::string_view(m_input.begin(), m_input.begin() + point));
                m_input = std::string_view(
                    min(m_input.begin() + point, m_input.end()), m_input.end());
                return cmov(
                    m_input.begin() == m_input.end(), OUT_OF_ELEMS, SCAN_FINISHED);
            }
            default:
                return UNABLE_TO_MATCH;
            }
        }

    public:
        constexpr DotItem() = default;
        constexpr ~DotItem() = default;

        consteval DotItem(u32 token_class, const std::string_view input)
        : m_token_class(token_class)
        {
            char letter = '\0';
            bool range_started = false;
            bool is_range_of_elems = false;
            Range *current_range = nullptr;

            for (const auto elem: input) {
                switch (elem) {
                case '[':
                    if constexpr (MayThrow) {
                        if (range_started) {
                            throw std::runtime_error("Two ranges were opened!");
                        }
                    }

                    letter = '\0';
                    range_started = true;
                    current_range = m_ranges.last();
                    break;

                case ']':
                    if constexpr (MayThrow) {
                        if (!range_started) {
                            throw std::runtime_error("There are not any opened
ranges!");
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

                    letter = elem;
                    is_range_of_elems  = false;

                    break;
                }
            }
        }
    };
}
 */

#include <iostream>

namespace cerb::lex {
    constexpr size_t MAX_RANGES                  = 8;
    constexpr size_t MAX_OPTIONALS               = 10;
    constexpr size_t AVAILABLE_CHARS             = 128;
    constexpr size_t MAX_LENGTH_FOR_STRONG_ELEMS = 4;

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
        Typename4Tokens m_class_of_token{};
        size_t m_dot{ 0 };
        size_t m_line{ 0 };
        size_t m_char{ 0 };
        storage_t m_ranges{};
        std::string_view m_input{};
        std::string_view m_filename{};
        iterator m_current{ m_ranges.begin() };
        StringChecker<StrictTokenSize, MayThrow> m_strong_tokens;

        inline static size_t m_shred_dot{ 0 };
        inline static size_t m_shred_line{ 0 };
        inline static size_t m_shred_char{ 0 };
        inline static std::string_view m_shared_input{};
        inline static std::string_view m_shared_filename{};

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

        constexpr auto get_filename() -> std::string_view &
        {
            if constexpr (ShredObject) {
                return m_shared_filename;
            }
            return m_filename;
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

            dot()          = 0;
            line()         = 0;
            char_number()  = 0;
            get_input()    = input;
            get_filename() = filename;
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

            dot()       = 0;
            change_input(end_of_repr + casted_layout);

            return { { begin_of_rep, end_of_repr },
                     m_class_of_token,
                     { line(), char_number(), get_filename() } };
        }

        constexpr auto convert_result(
            bool layout,
            const std::string_view &strong = "")
            -> Pair<ItemState, gl::Set<Token<std::string_view, Typename4Tokens>, 2>>
        {
            if (m_current->canEnd() && canEnd()) {
                if (strong.size() != 0) {
                    return { SCAN_FINISHED,
                             { end_scan(dot()), end_scan(strong.size()) } };
                }
                return { SCAN_FINISHED, { end_scan(dot()++, layout) } };
            }
            return { UNABLE_TO_MATCH, {} };
        }

        constexpr auto check()
            -> Pair<ItemState, gl::Set<Token<std::string_view, Typename4Tokens>, 2>>
        {
            bool layout = is_layout(get_input()[dot()]);
            auto strong = m_strong_tokens.check(dot(), get_input());

            if (((layout || get_input()[dot()] == '\0') && dot() != 0) ||
                strong.size() != 0) {
                return convert_result(layout, strong);
            }

            CERBLIB_UNROLL_N(2)
            while (is_layout(get_input()[dot()++])) {}

            if (dot() >= get_input().size() || get_input()[dot()] == '\0') {
                change_input(1);
                return { UNABLE_TO_MATCH, {} };
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
}// namespace cerb::lex

#endif /* CERBERUS_DOT_ITEM_HPP */
