#ifndef cerberusAddress_hpp
#define cerberusAddress_hpp

#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include <cerberus/operators.hpp>

namespace cerb {
    
    /**
     * @brief universal class to store m_pointer
     * 
     */
    class CERBLIB_TRIVIAL Address {
        cerb::byte *m_address;

    public:
        enum ALIGN2 : u32{
            Page4KB = 12U,
            Page2MB = 21U,
            Page1GB = 30U
        };

    public:
        [[nodiscard]] constexpr
        auto raw() const -> void * {
            return static_cast<void*>(m_address);
        }

        [[nodiscard]] CERBLIB_INLINE
        auto value() const -> size_t {
            return reinterpret_cast<size_t>(m_address);
        }

        template<typename T> [[nodiscard]]
        explicit constexpr
        operator T *() const noexcept {
            return static_cast<T*>(this->raw());
        }

    public:
        CERBLIB_CLASS_ARITHMETIC_ON_SELF
        (
                Address,
                constexpr,
                constexpr,
                other, OP,
                {
                this->m_address = reinterpret_cast<cerb::byte*>(
                        cerb::operators::count<OP>(value(), other.value())
                );
                return *this;
            }
        )
    
    public:
        CERBLIB_CLASS_ARITHMETIC_INCREMENT
        (
                Address,
                constexpr,
                { this->m_address++; }
        )

        CERBLIB_CLASS_ARITHMETIC_DECREMENT
        (
                Address,
                constexpr,
                { this->m_address--; }
        )

    public:
        template<u32 ALIGN_VALUE = ALIGN2::Page4KB, auto MODE = AlignMode::ALIGN>
        constexpr void align() {
            m_address = cerb::bit_cast<cerb::byte*>(
                    cerb::align<ALIGN_VALUE, MODE>(value())
            );
        }

    public:
        auto operator=(const Address &) -> Address& = default;
        auto operator=(Address &&) noexcept -> Address& = default;

    public:
        ~Address() = default;
        constexpr Address() noexcept : m_address(nullptr) {}
        
        Address(Address&) = default;
        Address(Address&&) noexcept = default;

        template<typename T>
        explicit constexpr CERBLIB_INLINE Address(T *addr) noexcept
            : m_address(static_cast<cerb::byte *>(addr))
        {}

        explicit constexpr CERBLIB_INLINE Address(size_t addr) noexcept
            : m_address(cerb::bit_cast<cerb::byte*>(addr))
        {}
    };

    CERBLIB_CLASS_ARITHMETIC
    (
        Address,
        constexpr,
        constexpr,
        _lhs, _rhs, OP,
        {
            return Address(cerb::operators::count<OP>(_lhs.value(), _rhs.value()));
        }
    )

    CERBLIB_CLASS_COMPARISON(
        Address,
        constexpr,
        constexpr,
        _lhs, _rhs, OP,
        {
            return cerb::operators::compare<OP>(_lhs.raw(), _rhs.raw());
        }
    )
} // namespace cerb

#endif /* cerberusAddress_hpp */
