#ifndef CERBERUS_ARRAYPROTOCOL_HPP
#define CERBERUS_ARRAYPROTOCOL_HPP

#include <cerberus/types.h>

namespace cerb {

    template<typename T, int POINTABLE, size_t SIZE = 0>
    union TRIVIAL ArrayProtocol {
        static_assert(POINTABLE < 2);

        T *pointer[POINTABLE];
        T data[SIZE * (!POINTABLE)];

    public:
        CERBLIB_INLINE auto get() -> T * {
            if constexpr (POINTABLE != 0) {
                return pointer[0];
            } else {
                return static_cast<T*>(data);
            }
        }

        CERBLIB_INLINE constexpr auto get() const -> const T * {
            if constexpr (POINTABLE != 0) {
                return pointer[0];
            } else {
                return static_cast<const T*>(data);
            }
        }

    public:
        CERBLIB_INLINE void set(T *ptr) {
            static_assert(POINTABLE != 0);
            this->pointer[0] = ptr;
        }

    public:
        CERBLIB_INLINE auto operator[](size_t index) -> T& {
            if constexpr (POINTABLE != 0) {
                return pointer[0][index];
            } else {
                return data[index];
            }
        }

        CERBLIB_INLINE auto operator[](size_t index) const -> T& {
            if constexpr (POINTABLE != 0) {
                return pointer[index];
            } else {
                return data[index];
            }
        }

    public:
        [[nodiscard]]
        explicit CERBLIB_INLINE operator T *() {
            return get();
        }

        [[nodiscard]]
        explicit CERBLIB_INLINE operator T *() const {
            return get();
        }

    public:
        auto operator=(const ArrayProtocol&) -> ArrayProtocol& = default;
        auto operator=(ArrayProtocol&&) noexcept -> ArrayProtocol& = default;

        auto operator=(T *pointer) noexcept -> ArrayProtocol& {
            static_assert(POINTABLE);
            this->pointer[0] = pointer;
            return *this;
        }

    public:
        ArrayProtocol() = default;
        ~ArrayProtocol() = default;

        ArrayProtocol(ArrayProtocol&) = default;
        ArrayProtocol(ArrayProtocol&&) noexcept = default;

        explicit CERBLIB_INLINE ArrayProtocol(T *pointer) noexcept {
            static_assert(POINTABLE);
            this->pointer[0] = pointer;
        }
    };
}

#endif /* CERBERUS_ARRAYPROTOCOL_HPP */
