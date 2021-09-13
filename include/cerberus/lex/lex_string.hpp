#ifndef CERBERUS_LEX_STRING_HPP
#define CERBERUS_LEX_STRING_HPP

#include <cerberus/types.h>

namespace cerb::lex {

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
}// namespace cerb::lex

#endif /* CERBERUS_LEX_STRING_HPP */
