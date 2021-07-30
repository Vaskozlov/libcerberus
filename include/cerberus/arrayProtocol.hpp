#ifndef CERBERUS_ARRAYPROTOCOL_HPP
#define CERBERUS_ARRAYPROTOCOL_HPP

#include <cerberus/types.h>

namespace cerb {

    template<typename T, int POINTABLE, size_t SIZE = 0>
    union CERBLIB_TRIVIAL ArrayProtocol {
        static_assert(POINTABLE < 2);
    private:
        T *m_pointer[POINTABLE];
        T  m_data[SIZE * (!POINTABLE)];

    public:
        constexpr auto get() noexcept
        {
            if constexpr (POINTABLE != 0) {
                return m_pointer[0];
            } else {
                return m_data;
            }
        }

        constexpr auto get() const noexcept -> const T *
        {
            if constexpr (POINTABLE != 0) {
                return m_pointer[0];
            } else {
                return static_cast<const T*>(m_data);
            }
        }

    public:
        constexpr void set(T *ptr) noexcept
        {
            static_assert(POINTABLE != 0);
            this->m_pointer[0] = ptr;
        }

    public:
        constexpr auto operator[](size_t index) noexcept -> T&
        {
            if constexpr (POINTABLE != 0) {
                return m_pointer[0][index];
            } else {
                return m_data[index];
            }
        }

        constexpr auto operator[](size_t index) const noexcept -> T&
        {
            if constexpr (POINTABLE != 0) {
                return m_pointer[0][index];
            } else {
                return m_data[index];
            }
        }

    public:
        [[nodiscard]] explicit constexpr
        operator T *() noexcept
        {
            return get();
        }

        [[nodiscard]] explicit constexpr
        operator T *() const noexcept
        {
            return get();
        }

    public:
        constexpr auto operator=(ArrayProtocol&&) noexcept -> ArrayProtocol& = default;
        constexpr auto operator=(const ArrayProtocol&) noexcept -> ArrayProtocol& = default;

        constexpr auto operator=(T *pointer) noexcept -> ArrayProtocol&
        {
            static_assert(POINTABLE);
            this->m_pointer[0] = pointer;
            return *this;
        }

    public:
        constexpr ArrayProtocol() noexcept = default;
        CERBLIB20_CONSTEXPR ~ArrayProtocol() noexcept = default;

        constexpr ArrayProtocol(ArrayProtocol&) = default;
        constexpr ArrayProtocol(ArrayProtocol&&) noexcept = default;

        explicit constexpr ArrayProtocol(T *pointer) noexcept
        {
            static_assert(POINTABLE);
            this->m_pointer[0] = pointer;
        }
    };
}

#endif /* CERBERUS_ARRAYPROTOCOL_HPP */
