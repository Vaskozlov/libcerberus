#ifndef cerberusAddress_hpp
#define cerberusAddress_hpp

#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include <cerberus/operators.hpp>

namespace cerb {
    
    /**
     * @brief universal class to store pointer
     * 
     */
    class TRIVIAL address {
        cerb::byte *_address;

    public:
        enum ALIGN2 : u32{
            Page4KB = 12U,
            Page2MB = 21U,
            Page1GB = 30U
        };

    public:
        [[nodiscard]] CERBLIB_INLINE auto raw() const -> void * {
            return static_cast<void*>(_address);
        }

        [[nodiscard]] CERBLIB_INLINE auto value() const -> size_t {
            return reinterpret_cast<size_t>(_address);
        }

        template<typename T> [[nodiscard]]
        explicit CERBLIB_INLINE operator T *() const noexcept {
            return static_cast<T*>(this->raw());
        }

    public:
        CERBLIB_CLASS_ARITHMETIC
        (
            address,
            constexpr,
            constexpr,
            _lhs, _rhs, OP,
            {
                return address(cerb::operators::count<OP>(_lhs.value(), _rhs.value()));
            }
        )

    public:
        CERBLIB_CLASS_ARITHMETIC_ON_SELF
        (
            address,
            constexpr,
            constexpr,
            other, OP,
            {
                this->_address = reinterpret_cast<cerb::byte*>(
                        cerb::operators::count<OP>(value(), other.value())
                );
                return *this;
            }
        )

    public:
        CERBLIB_CLASS_COMPARISON(
            address,
            constexpr,
            constexpr,
            _lhs, _rhs, OP,
            {
                return cerb::operators::compare<OP>(_lhs.raw(), _rhs.raw());
            }
        )
    
    public:
        CERBLIB_CLASS_ARITHMETIC_INCREMENT
        (
            address,
            constexpr,
            { this->_address++; }
        )

        CERBLIB_CLASS_ARITHMETIC_DECREMENT
        (
            address,
            constexpr,
            { this->_address--; }
        )

    public:
        template<u32 ALIGN_VALUE = ALIGN2::Page4KB, auto MODE = AlignMode::ALIGN>
        constexpr void align() {
            _address = cerb::bit_cast<cerb::byte*>(
                    cerb::align<ALIGN_VALUE, MODE>(value())
            );
        }

    public:
        auto operator=(const address &) -> address& = default;
        auto operator=(address &&) noexcept -> address& = default;

    public:
        ~address() = default;
        address() noexcept : _address(nullptr) {}
        
        address(address&) = default;
        address(address&&) noexcept = default;

        template<typename T>
        explicit constexpr CERBLIB_INLINE address(T *addr) noexcept
            : _address(static_cast<cerb::byte *>(addr))
        {}

        explicit constexpr CERBLIB_INLINE address(size_t addr) noexcept
            : _address(cerb::bit_cast<cerb::byte*>(addr))
        {}
    };
} // namespace cerb

#endif /* cerberusAddress_hpp */
