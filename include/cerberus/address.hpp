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
        CERBLIB_ARITHMETIC_BASE
        (
            address,
            always_inline,
            _lhs, _rhs, OP
            ,
            {
                return address(cerb::operators::count<OP>(_lhs, _rhs));
            }
        )

    public:
        CERBLIB_ARITHMETIC_BASE_EQUAL
        (
            address,
            always_inline,
            other, OP,
            {
                this->_address = reinterpret_cast<cerb::byte*>(
                        cerb::operators::count<OP>(value(), other.value())
                );
                return *this;
            }
        )

    public:
        CERBLIB_CREATE_COMPARISON_RULES(
            address,
            always_inline,
            _lhs, _rhs, OP,
            {
                return cerb::operators::compare<OP>(_lhs.raw(), _rhs.raw());
            }
        )
    
    public:
        CERBLIB_ARITHMETIC_INCREMENT
        (
            address,
            always_inline,
            { this->_address++; }
        )

        CERBLIB_ARITHMETIC_DECREMENT
        (
            address,
            always_inline,
            { this->_address++; }
        )

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
