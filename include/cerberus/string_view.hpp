#ifndef CERBERUS_STRING_VIEW_HPP
#define CERBERUS_STRING_VIEW_HPP

#include <cerberus/types.h>

namespace cerb {
    template<typename CharT>
    class CERBLIB_TRIVIAL basic_string_view
    {
        static_assert(!std::is_array_v<CharT>);
        static_assert(std::is_trivial_v<CharT> && std::is_standard_layout_v<CharT>);

        static constexpr auto length_of_string(const char *str) -> size_t
        {
            size_t index = 0;

            CERBLIB_UNROLL_N(4)
            while (*str != '\0') {
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

        constexpr basic_string_view(const char *str) noexcept
          : m_len(length_of_string(str)), m_str(str)
        {}

        constexpr basic_string_view(const char *str, size_t len) noexcept
          : m_str(str), m_len(len)
        {}

        template<int Size>
        constexpr basic_string_view(const char (&str)[Size]) noexcept
          : m_str(str), m_len(Size)
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

        [[nodiscard]] constexpr auto size() const -> size_t
        {
            return m_len;
        }

        [[nodiscard]] constexpr auto length() const -> size_t
        {
            return m_len;
        }

        constexpr auto begin() const noexcept -> const_iterator
        {
            return m_str;
        }

        constexpr auto end() const noexcept -> const_iterator
        {
            return m_str + m_len;
        }

        constexpr auto cbegin() const noexcept -> const_iterator
        {
            return m_str;
        }

        constexpr auto cend() const noexcept -> const_iterator
        {
            return m_str + m_len;
        }

        constexpr auto rbegin() const noexcept -> const_reverce_iterator
        {
            return const_reverce_iterator(begin());
        }

        constexpr auto rend() const noexcept -> const_reverce_iterator
        {
            return const_reverce_iterator(end());
        }

        constexpr auto crbegin() const noexcept -> const_reverce_iterator
        {
            return const_reverce_iterator(begin());
        }

        constexpr auto crend() const noexcept -> const_reverce_iterator
        {
            return const_reverce_iterator(end());
        }

        [[nodiscard]] constexpr auto empty() const noexcept -> bool
        {
            return m_len == 0;
        }

        constexpr auto at(size_type index) const noexcept -> const_reference
        {
            return m_str[index];
        }

        constexpr auto operator[](size_type index) const noexcept -> const_reference
        {
            return m_str[index];
        }

        constexpr auto front() const noexcept -> const_reference
        {
            return *m_str;
        }

        constexpr auto back() const noexcept -> const_reference
        {
            return m_str[m_len - 1];
        }

        constexpr auto data() const noexcept -> const_pointer
        {
            return m_str;
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

        constexpr auto operator<=>(const basic_string_view &other)
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

    private:
        size_t m_len{};
        const CharT *m_str{ nullptr };
    };

    using string_view    = basic_string_view<char>;
    using wstring_view   = basic_string_view<wchar_t>;
    using u8string_view  = basic_string_view<char8_t>;
    using u16string_view = basic_string_view<char16_t>;
    using u32string_view = basic_string_view<char32_t>;
}// namespace cerb

template<typename T, typename CharT>
constexpr auto operator<<(T &os, const cerb::basic_string_view<CharT> &str) -> T &
{
    os << std::basic_string_view<CharT>(str.begin(), str.size());
    return os;
}

#endif /* CERBERUS_STRING_VIEW_HPP */
