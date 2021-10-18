#ifndef CERBERUS_LEX_STRING_HPP
#define CERBERUS_LEX_STRING_HPP

#include <cerberus/types.h>
#include <cerberus/string_view.hpp>

namespace cerb::lex {
    template<typename CharT>
    constexpr auto to_unsigned(CharT value)
    {
        static_assert(std::is_integral_v<CharT>);

        if constexpr (std::is_unsigned_v<CharT>) {
            return value;
        } else if constexpr (sizeof(CharT) == sizeof(u8)) {
            return bit_cast<u8>(value);
        } else if constexpr (sizeof(CharT) == sizeof(u16)) {
            return bit_cast<u16>(value);
        } else if constexpr (sizeof(CharT) == sizeof(u32)) {
            return bit_cast<u32>(value);
        } else if constexpr (sizeof(CharT) == sizeof(u64)) {
            return bit_cast<u64>(value);
        }
    }

    template<typename CharT>
    constexpr auto check_substring(
        size_t index, const cerb::basic_string_view<CharT> &src,
        const cerb::basic_string_view<CharT> &substr) -> bool
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

        return i == substr_size && !substr.empty();
    }

    template<
        typename CharT, typename TokenType, size_t MaxTerminals = 128,
        size_t MaxLength4Terminal = 8, bool MayThrow = true>
    struct TerminalContainer
    {
        constexpr static size_t MaxChars = (1ULL << bitsizeof(CharT)) - 1;
        using string_view_t              = basic_string_view<CharT>;
        using bitmap_t                   = ConstBitmap<1, MaxChars>;
        using storage_t                  = std::array<bitmap_t, MaxLength4Terminal>;
        using map_t = gl::Map<uintmax_t, TokenType, MaxTerminals>;

    public:
        CERBLIB_DECL auto check(CharT elem) const -> Pair<bool, TokenType>
        {
            auto hash = to_unsigned(elem);
            return { static_cast<bool>(m_bitmaps[0].template at<0>(hash)),
                     m_map.search(hash)->second };
        }

        CERBLIB_DECL auto
            check(size_t offset, const string_view_t &str) const
            -> Pair<string_view_t, TokenType>
        {
            size_t i    = 0;
            size_t hash = 0;

            CERBLIB_UNROLL_N(2)
            for (; i < str.size(); ++i) {
                auto local_hash = to_unsigned(str[offset + i]);

                if (m_bitmaps[i].template at<0>(local_hash) == 0) {
                    break;
                }

                hash = hash * 31U + local_hash;
            }
            auto result = m_map.search(hash);
            i           = cmov<size_t>(result == m_map.end(), 0, i);

            return { { str.begin() + offset, str.begin() + i + offset },
                     result->second };
        }

        consteval TerminalContainer() = default;
        consteval TerminalContainer(
            const std::initializer_list<Pair<TokenType, CharT>> &chars,
            const std::initializer_list<Pair<TokenType, const string_view_t>>
                &strings)
        {
            if constexpr (MayThrow) {
                if (chars.size() + strings.size() > MaxTerminals) {
                    throw std::runtime_error(
                        "Terminal container can't hold so much terminals!");
                }
            }

            CERBLIB_UNROLL_N(4)
            for (const auto &elem : chars) {
                auto hash = to_unsigned(elem.second);
                m_map.emplace(hash, elem.first);
                m_bitmaps[0].template set<1, 0>(hash);
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &elem : strings) {
                size_t i    = 0;
                size_t hash = 0;

                if constexpr (MayThrow) {
                    if (MaxLength4Terminal <= elem.second.size()) {
                        throw std::out_of_range(
                            "String checker can't hold such a long string!");
                    }
                }

                CERBLIB_UNROLL_N(4)
                for (const auto &chr : elem.second) {
                    auto local_hash = to_unsigned(chr);
                    hash            = hash * 31U + local_hash;
                    m_bitmaps[i++].template set<1, 0>(local_hash);
                }

                m_map.emplace(hash, elem.first);
            }
        }

        constexpr ~TerminalContainer() = default;

    private:
        map_t m_map{};
        storage_t m_bitmaps{};
    };
}// namespace cerb::lex

#endif /* CERBERUS_LEX_STRING_HPP */
