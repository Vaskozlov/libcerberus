#ifndef CERBERUS_ARRAYPROTOCOL_HPP
#define CERBERUS_ARRAYPROTOCOL_HPP

#include <cerberus/types.h>

namespace cerb {

    template<typename T, int POINTABLE, size_t SIZE = 0>
    class TRIVIAL ArrayProtocol {
        static_assert(POINTABLE < 2);

        union ArrayData {
            T *pointer[POINTABLE];
            T data[SIZE * (!POINTABLE)];
        };

        ArrayData m_p;

    public:
        always_inline constexpr auto get() -> T * {
            if constexpr (POINTABLE != 0) {
                return m_p.pointer[0];
            } else {
                return m_p.data;
            }
        }

        always_inline constexpr auto get() const -> T * {
            if constexpr (POINTABLE != 0) {
                return m_p.pointer[0];
            } else {
                return m_p.data;
            }
        }

        always_inline auto operator[](size_t index) -> T& {
            if constexpr (POINTABLE) {
                return m_p.pointer[0][index];
            } else {
                return m_p.data[index];
            }
        }

        always_inline auto operator[](size_t index) const -> T& {
            if constexpr (POINTABLE) {
                return m_p.pointer[index];
            } else {
                return m_p.data[index];
            }
        }

    public:
        auto operator=(const ArrayProtocol&) -> ArrayProtocol& = default;
        auto operator=(ArrayProtocol&&) noexcept -> ArrayProtocol& = default;

        auto operator=(T *pointer) noexcept -> ArrayProtocol& {
            static_assert(POINTABLE);
            m_p.pointer[0] = pointer;
            return *this;
        }

    public:
        ArrayProtocol() = default;
        ~ArrayProtocol() = default;

        ArrayProtocol(ArrayProtocol&) = default;
        ArrayProtocol(ArrayProtocol&&) noexcept = default;

        always_inline ArrayProtocol(T *pointer) {
            static_assert(POINTABLE);
            m_p.pointer[0] = pointer;
        }
    };
}

#endif /* CERBERUS_ARRAYPROTOCOL_HPP */
