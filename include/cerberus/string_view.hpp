#ifndef CERBERUS_STRING_VIEW_HPP
#define CERBERUS_STRING_VIEW_HPP

#include <cerberus/math.hpp>
#include <cerberus/string.hpp>
#include <cerberus/types.h>

namespace cerb {
    template<typename CharT>
    class CERBLIB_TRIVIAL basic_string_view
    {
        static_assert(!std::is_array_v<CharT>);
        static_assert(std::is_trivial_v<CharT> && std::is_standard_layout_v<CharT>);

        CERBLIB_DECL static auto length_of_string(const CharT *str) -> size_t
        {
#ifdef __x86_64__
            if constexpr (sizeof(CharT) == sizeof(char)) {
                return cerb::strlen(static_cast<const char *>(str));
            }
#endif /* __x86_64__ */

            size_t index = 0;

            CERBLIB_UNROLL_N(4)
            while (*str != static_cast<CharT>(0)) {
                str++;
                index++;
            }

            return index;
        }

    public:
        using value_type             = CharT;
        using pointer                = value_type *;
        using const_pointer          = const value_type *;
        using reference              = value_type &;
        using const_reference        = const value_type &;
        using const_iterator         = const value_type *;
        using iterator               = const_iterator;
        using const_reverce_iterator = std::reverse_iterator<const_iterator>;
        using reverse_iteraotr       = const_reverce_iterator;
        using size_type              = size_t;
        using difference_type        = ptrdiff_t;

        constexpr basic_string_view() noexcept                          = default;
        constexpr basic_string_view(basic_string_view &&) noexcept      = default;
        constexpr basic_string_view(const basic_string_view &) noexcept = default;

        constexpr basic_string_view(const CharT *str) noexcept
          : m_len(length_of_string(str)), m_str(str)
        {}

        constexpr basic_string_view(const CharT *str, size_t len) noexcept
          : m_len(len), m_str(str)
        {}

        template<unsigned long Size>
        constexpr basic_string_view(const CharT (&str)[Size]) noexcept
          : m_len(Size), m_str(str)
        {}

        constexpr basic_string_view(
            const_iterator first, const_iterator last) noexcept
          : m_len(static_cast<size_t>(static_cast<difference_type>(last - first))),
            m_str(first)
        {}

        constexpr ~basic_string_view() = default;

        constexpr auto operator    =(basic_string_view &&) noexcept
            -> basic_string_view & = default;
        constexpr auto operator    =(const basic_string_view &) noexcept
            -> basic_string_view & = default;

        CERBLIB_DECL auto size() const -> size_t
        {
            return m_len;
        }

        CERBLIB_DECL auto length() const -> size_t
        {
            return m_len;
        }

        CERBLIB_DECL auto begin() const noexcept -> const_iterator
        {
            return m_str;
        }

        CERBLIB_DECL auto end() const noexcept -> const_iterator
        {
            return m_str + m_len;
        }

        CERBLIB_DECL auto cbegin() const noexcept -> const_iterator
        {
            return m_str;
        }

        CERBLIB_DECL auto cend() const noexcept -> const_iterator
        {
            return m_str + m_len;
        }

        CERBLIB_DECL auto rbegin() const noexcept -> const_reverce_iterator
        {
            return const_reverce_iterator(begin());
        }

        CERBLIB_DECL auto rend() const noexcept -> const_reverce_iterator
        {
            return const_reverce_iterator(end());
        }

        CERBLIB_DECL auto crbegin() const noexcept -> const_reverce_iterator
        {
            return const_reverce_iterator(begin());
        }

        CERBLIB_DECL auto crend() const noexcept -> const_reverce_iterator
        {
            return const_reverce_iterator(end());
        }

        CERBLIB_DECL auto empty() const noexcept -> bool
        {
            return m_len == 0;
        }

        CERBLIB_DECL auto at(size_type index) const noexcept -> const_reference
        {
            return m_str[index];
        }

        constexpr auto operator[](size_type index) const noexcept -> const_reference
        {
            return m_str[index];
        }

        CERBLIB_DECL auto front() const noexcept -> const_reference
        {
            return *m_str;
        }

        CERBLIB_DECL auto back() const noexcept -> const_reference
        {
            return m_str[m_len - 1];
        }

        CERBLIB_DECL auto data() const noexcept -> const_pointer
        {
            return m_str;
        }

        CERBLIB_DECL auto to_string() const noexcept -> std::basic_string_view<CharT>
        {
            return std::basic_string_view<CharT>(begin(), size());
        }

        constexpr auto operator==(const basic_string_view &other) const noexcept
            -> bool
        {
            if (size() != other.size()) {
                return false;
            }

            CERBLIB_UNROLL_N(4)
            for (size_t i = 0; i < other.size(); ++i) {
                if (m_str[i] != other.m_str[i]) {
                    return false;
                }
            }

            return true;
        }

        constexpr auto operator<=>(const basic_string_view &other) const noexcept
            -> std::strong_ordering
        {
            CERBLIB_UNROLL_N(4)
            for (size_t i = 0; i < cerb::min(size(), other.size()); ++i) {
                if (m_str[i] != other.m_str[i]) {
                    return m_str[i] <=> other.m_str[i];
                }
            }
            return size() <=> other.size();
        }

        CERBLIB_DECL auto contains(CharT elem) const noexcept -> size_t
        {
            CERBLIB_UNROLL_N(4)
            for (size_t i = 0; i < size(); ++i) {
                if (m_str[i] == elem) {
                    return i;
                }
            }
            return std::numeric_limits<size_t>::max();
        }

    private:
        size_t m_len{};
        const CharT *m_str{ nullptr };
    };

    using string_view    = basic_string_view<char>;
    using wstring_view   = basic_string_view<wchar_t>;
    using u8string_view  = basic_string_view<char8_t>;
    using u16string_view = basic_string_view<char16_t>;
    using u32string_view = basic_string_view<char32_t>;

    namespace literals {
        consteval auto operator"" _sv(const char *str, size_t len) -> string_view
        {
            return { str, len };
        }

        consteval auto operator"" _sv(const char8_t *str, size_t len)
            -> u8string_view
        {
            return { str, len };
        }

        consteval auto operator"" _sv(const char16_t *str, size_t len)
            -> u16string_view
        {
            return { str, len };
        }

        consteval auto operator"" _sv(const char32_t *str, size_t len)
            -> u32string_view
        {
            return { str, len };
        }
    }// namespace literals

}// namespace cerb

template<typename T, typename CharT>
constexpr auto operator<<(T &os, const cerb::basic_string_view<CharT> &str) -> T &
{
    os << std::basic_string_view<CharT>(str.begin(), str.size());
    return os;
}

#endif /* CERBERUS_STRING_VIEW_HPP */
