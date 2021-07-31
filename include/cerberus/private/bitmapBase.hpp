#ifndef cerberusBitmapAPI_hpp
#define cerberusBitmapAPI_hpp

#include <utility>
#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include <cerberus/string.hpp>

#pragma pack (push, 1)

namespace cerb::PRIVATE {
    constexpr u64 value2bits[] = {
        0b0,
        0b1UL,
        0b11UL,
        0b111UL,
        0b1111UL,
        0b11111UL,
        0b111111UL,
        0b1111111UL,
        0b11111111UL,
        0b111111111UL,
        0b1111111111UL,
        0b11111111111UL,
        0b111111111111UL,
        0b1111111111111UL,
        0b11111111111111UL,
        0b111111111111111UL,
        0b1111111111111111UL,
        0b11111111111111111UL,
        0b111111111111111111UL,
        0b1111111111111111111UL,
        0b11111111111111111111UL,
        0b111111111111111111111UL,
        0b1111111111111111111111UL,
        0b11111111111111111111111UL,
        0b111111111111111111111111UL,
        0b1111111111111111111111111UL,
        0b11111111111111111111111111UL,
        0b111111111111111111111111111UL,
        0b1111111111111111111111111111UL,
        0b11111111111111111111111111111UL,
        0b111111111111111111111111111111UL,
        0b1111111111111111111111111111111UL,
        0b11111111111111111111111111111111UL,
        0b111111111111111111111111111111111UL,
        0b1111111111111111111111111111111111UL,
        0b11111111111111111111111111111111111UL,
        0b111111111111111111111111111111111111UL,
        0b1111111111111111111111111111111111111UL,
        0b11111111111111111111111111111111111111UL,
        0b111111111111111111111111111111111111111UL,
        0b1111111111111111111111111111111111111111UL,
        0b11111111111111111111111111111111111111111UL,
        0b111111111111111111111111111111111111111111UL,
        0b1111111111111111111111111111111111111111111UL,
        0b11111111111111111111111111111111111111111111UL,
        0b111111111111111111111111111111111111111111111UL,
        0b1111111111111111111111111111111111111111111111UL,
        0b11111111111111111111111111111111111111111111111UL,
        0b111111111111111111111111111111111111111111111111UL,
        0b1111111111111111111111111111111111111111111111111UL,
        0b11111111111111111111111111111111111111111111111111UL,
        0b111111111111111111111111111111111111111111111111111UL,
        0b1111111111111111111111111111111111111111111111111111UL,
        0b11111111111111111111111111111111111111111111111111111UL,
        0b111111111111111111111111111111111111111111111111111111UL,
        0b1111111111111111111111111111111111111111111111111111111UL,
        0b11111111111111111111111111111111111111111111111111111111UL,
        0b111111111111111111111111111111111111111111111111111111111UL,
        0b1111111111111111111111111111111111111111111111111111111111UL,
        0b11111111111111111111111111111111111111111111111111111111111UL,
        0b111111111111111111111111111111111111111111111111111111111111UL,
        0b1111111111111111111111111111111111111111111111111111111111111UL,
        0b11111111111111111111111111111111111111111111111111111111111111UL,
        0b111111111111111111111111111111111111111111111111111111111111111UL,
        0b1111111111111111111111111111111111111111111111111111111111111111UL
    };

    template<typename T>
    class CERBLIB_TRIVIAL BitmapElem {
        u16 m_bitIndex;
        T *m_elem;

    public:
        constexpr auto value() noexcept -> u8 {
            return (*m_elem & (static_cast<T>(1) << m_bitIndex)) != 0;
        }

    public:
        auto operator=(BitmapElem&&) noexcept -> BitmapElem& = default;
        auto operator=(const BitmapElem&) noexcept -> BitmapElem& = default;

        constexpr auto operator=(u8 newValue) noexcept -> BitmapElem& {
            *m_elem &= ~(static_cast<T>(1) << m_bitIndex);
            *m_elem |= static_cast<T>(newValue) << m_bitIndex;
            return *this;
        }

    public:
        ~BitmapElem() = default;
        BitmapElem(BitmapElem&) = default;
        BitmapElem(BitmapElem&&) noexcept = default;

        constexpr CERBLIB_INLINE BitmapElem(u16 bitIndex, T *elem) noexcept
            : m_bitIndex(bitIndex), m_elem(elem)
        {}
    };

    template<size_t Size, typename T> [[nodiscard]] constexpr
    auto at(const std::array<T, Size> &data, size_t index) noexcept -> u8 {
        auto elemIndex = index / bitsizeof(T);
        auto bitIndex  = index % bitsizeof(T);
        return (data[elemIndex] & (static_cast<T>(1) << bitIndex)) != 0;
    }

    template<typename T> [[nodiscard]] constexpr
    auto at(const T *data, size_t index) noexcept -> u8 {
        auto elemIndex = index / bitsizeof(T);
        auto bitIndex  = index % bitsizeof(T);
        return (data[elemIndex] & (static_cast<T>(1) << bitIndex)) != 0;
    }

    template<size_t Size, typename T> [[nodiscard]] constexpr
    auto isEmpty(const std::array<T, Size> &data, size_t limit) noexcept -> bool {
        size_t i = 0;

        CERBLIB_UNROLL_N(4)
        for (; i < limit / bitsizeof(T); i++) {
            if (data[i] > 0) {
                return false;
            }
        }

        return (data[i] & cerb::PRIVATE::value2bits[limit % bitsizeof(T)]) == 0;
    }

    template<typename T> [[nodiscard]] constexpr
    auto isEmpty(const T *data, size_t limit) noexcept -> bool {
        size_t i = 0;

        CERBLIB_UNROLL_N(4)
        for (; i < limit / bitsizeof(T); i++) {
            if (data[i] > 0) {
                return false;
            }
        }

        return (data[i] & cerb::PRIVATE::value2bits[limit % bitsizeof(T)]) == 0;
    }

    template<u8 FirstValue, size_t Size, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if(const std::array<T, Size> &data, size_t limit) noexcept -> size_t {
        size_t i = 0;
        auto reverse_function = [](T value) {
            if constexpr (FirstValue == 1) {
                return ~value;
            } else {
                return value;
            }
        };

        CERBLIB_UNROLL_N(4)
        for (; i < limit / bitsizeof(T); ++i) {
            T value = reverse_function(data[i]);

            if (value < std::numeric_limits<T>::max()) {
                return i * bitsizeof(T) + cerb::findFreeBitForward(static_cast<u64>(value));
            }
        }

        T value = reverse_function(data[i]);

        if (value < std::numeric_limits<T>::max()) {
            auto bitIndex = cerb::findFreeBitForward(static_cast<u64>(value));
            return cerb::cmov<size_t>(
                    bitIndex < limit % bitsizeof(T),
                    i * bitsizeof(T) + bitIndex,
                    std::numeric_limits<size_t>::max()
            );
        }

        return std::numeric_limits<size_t>::max();
    }

    template<u8 FirstValue, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if(const T *data, size_t limit) noexcept -> size_t {
        size_t i = 0;
        auto reverse_function = [](T value) {
            if constexpr (FirstValue == 1) {
                return ~value;
            } else {
                return value;
            }
        };

        CERBLIB_UNROLL_N(4)
        for (; i < limit / bitsizeof(T); ++i) {
            T value = reverse_function(data[i]);

            if (value < std::numeric_limits<T>::max()) {
                return i * bitsizeof(T) + cerb::findFreeBitForward(static_cast<u64>(value));
            }
        }

        T value = reverse_function(data[i]);

        if (value < std::numeric_limits<T>::max()) {
            auto bitIndex = cerb::findFreeBitForward(static_cast<u64>(value));
            return cerb::cmov<size_t>(
                    bitIndex < limit % bitsizeof(T),
                    i * bitsizeof(T) + bitIndex,
                    std::numeric_limits<size_t>::max()
                    );
        }

        return std::numeric_limits<size_t>::max();
    }
} // namespace cerb::PRIVATE
#pragma pack (pop)
#endif /* cerberusBitmapAPI_hpp */
