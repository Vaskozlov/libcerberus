#ifndef cerberusAddress_hpp
#define cerberusAddress_hpp

#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include <cerberus/operators.hpp>

namespace cerb {

    /**
     * @brief universal class to store pointers
     *
     */
    class CERBLIB_TRIVIAL Address
    {
        cerb::byte *m_address{ nullptr };

    public:
        enum ALIGN2 : u32
        {
            Page4KB = 12U,
            Page2MB = 21U,
            Page1GB = 30U
        };

    public:
        [[nodiscard]] constexpr auto raw() const -> void *
        {
            return static_cast<void *>(m_address);
        }

        [[nodiscard]] CERBLIB_INLINE auto value() const -> size_t
        {
            return reinterpret_cast<size_t>(m_address);
        }

        template<typename T>
        [[nodiscard]] explicit constexpr operator T *() const noexcept
        {
            return static_cast<T *>(this->raw());
        }

    public:
        CERBLIB_INLINE friend auto operator+(const Address &lhs, const Address &rhs)
            -> Address
        {
            return lhs.m_address + rhs.value();
        }

        CERBLIB_INLINE friend auto operator-(const Address &lhs, const Address &rhs)
            -> Address
        {
            return lhs.m_address - rhs.value();
        }

        CERBLIB_INLINE friend auto operator*(const Address &lhs, const Address &rhs)
            -> Address
        {
            return lhs.value() * rhs.value();
        }

        CERBLIB_INLINE friend auto operator/(const Address &lhs, const Address &rhs)
            -> Address
        {
            return lhs.value() / rhs.value();
        }

        CERBLIB_INLINE friend auto operator%(const Address &lhs, const Address &rhs)
            -> Address
        {
            return lhs.value() % rhs.value();
        }

    public:
        CERBLIB_INLINE auto operator+=(const Address &other) -> Address &
        {
            m_address += other.value();
            return *this;
        }

        CERBLIB_INLINE auto operator-=(const Address &other) -> Address &
        {
            m_address -= other.value();
            return *this;
        }

        CERBLIB_INLINE auto operator*=(const Address &other) -> Address &
        {
            m_address = reinterpret_cast<byte *>(value() * other.value());
            return *this;
        }

        CERBLIB_INLINE auto operator/=(const Address &other) -> Address &
        {
            m_address = reinterpret_cast<byte *>(value() / other.value());
            return *this;
        }

        CERBLIB_INLINE auto operator%=(const Address &other) -> Address &
        {
            m_address = reinterpret_cast<byte *>(value() % other.value());
            return *this;
        }

    public:
        constexpr auto operator==(const Address &) const -> bool = default;
        constexpr auto operator<=>(const Address &) const        = default;

    public:
        template<u32 ALIGN_VALUE = ALIGN2::Page4KB, auto MODE = AlignMode::ALIGN>
        constexpr void align()
        {
            m_address = bit_cast<byte *>(align<ALIGN_VALUE, MODE>(value()));
        }

    public:
        constexpr auto operator=(const Address &) -> Address & = default;
        constexpr auto operator=(Address &&) noexcept -> Address & = default;

    public:
        constexpr Address() noexcept  = default;
        constexpr ~Address() noexcept = default;

        constexpr Address(Address &)           = default;
        constexpr Address(Address &&) noexcept = default;

        constexpr explicit Address(void *addr) noexcept
          : m_address(static_cast<byte *>(addr))
        {}

        CERBLIB_INLINE explicit Address(size_t addr) noexcept
          : m_address(reinterpret_cast<byte *>(addr))
        {}
    };

    namespace literals {
        CERBLIB_INLINE auto operator"" _addr(unsigned long long address)
        {
            return Address(address);
        }
    }// namespace literals
}// namespace cerb

#endif /* cerberusAddress_hpp */
