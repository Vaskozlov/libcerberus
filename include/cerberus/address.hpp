#ifndef cerberusAddress_hpp
#define cerberusAddress_hpp

#include <cerberus/types.h>
#include <cerberus/math.hpp>

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
        [[nodiscard]] always_inline auto raw()   const -> void * {
            return static_cast<void*>(_address);
        }

        [[nodiscard]] always_inline auto value() const -> size_t {
            return reinterpret_cast<size_t>(_address);
        }

        template<typename T> [[nodiscard]]
        explicit always_inline operator T *() const noexcept {
            return static_cast<T*>(this->raw());
        }

    public:
        always_inline friend auto operator+(const address &_lhs, const address &_rhs) -> address {
            return address(_lhs._address + _rhs.value());
        }

        always_inline friend auto operator-(const address &_lhs, const address &_rhs) -> address {
            return address(_lhs._address - _rhs.value());
        }

        always_inline friend auto operator*(const address &_lhs, const address &_rhs) -> address {
            return address(_lhs.value() * _rhs.value());
        }

        always_inline friend auto operator/(const address &_lhs, const address &_rhs) -> address {
            return address(_lhs.value() / _rhs.value());
        }
    
    public:
        #if CERBLIB_THREE_WAY_COMPARISON
            always_inline auto operator<=>(const address&) const = default;
        #else
            GEN_COMPARISON_RULES(address, _address);
        #endif /* CERBLIB_THREE_WAY_COMPARISON */
    
    public:
        always_inline auto operator++() -> address& {
            _address++;
            return *this;
        }

        always_inline auto operator++(int) -> address {
            address old = *this;
            operator++();
            return old;
        }

        always_inline auto operator--() -> address& {
            _address++;
            return *this;
        }

        always_inline auto operator--(int) -> address {
            address old = *this;
            operator--();
            return old;
        }
    
    public:
        always_inline auto operator+=(const address &_rhs) -> address& {
            _address += _rhs.value();
            return *this;
        }

        always_inline auto operator-=(const address &_rhs) -> address& {
            _address -= _rhs.value();
            return *this;
        }

        always_inline auto operator*=(const address &_rhs) -> address& {
            _address = reinterpret_cast<cerb::byte*>(this->value() * _rhs.value());
            return *this;
        }

        always_inline auto operator/=(const address &_rhs) -> address& {
            _address = reinterpret_cast<cerb::byte*>(this->value() / _rhs.value());
            return *this;
        }
    
    public:
        template<u32 ALIGN_VALUE = ALIGN2::Page4KB, auto MODE = AlignMode::ALIGN>
        always_inline void align() {
            _address = reinterpret_cast<cerb::byte*>(
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
        explicit always_inline address(T *addr) noexcept
            : _address(static_cast<cerb::byte *>(addr))
        {}

        explicit always_inline address(size_t addr) noexcept
            : _address(reinterpret_cast<cerb::byte *>(addr))
        {}
    };
} // namespace cerb

#endif /* cerberusAddress_hpp */
