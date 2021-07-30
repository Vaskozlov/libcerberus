#ifndef CERBERUS_DOUBLEBITMAPBASE_HPP
#define CERBERUS_DOUBLEBITMAPBASE_HPP

#include <cerberus/types.h>
#include <cerberus/pair.hpp>
#include <cerberus/math.hpp>
#include <cerberus/private/bitmapBase.hpp>

#pragma pack (push, 1)

namespace cerb::PRIVATE {
    struct CERBLIB_TRIVIAL TwoBitStorage {
        u8 first : 1;
        u8 second : 1;

    public:
        auto operator=(const TwoBitStorage&) -> TwoBitStorage& = default;
        auto operator=(TwoBitStorage&&) noexcept -> TwoBitStorage& = default;

    public:
        TwoBitStorage() = default;
        ~TwoBitStorage() = default;

        TwoBitStorage(TwoBitStorage&) = default;
        TwoBitStorage(TwoBitStorage&&) noexcept = default;
    };

    template<typename T>
    struct CERBLIB_TRIVIAL DoubleBitmapElem {
        BitmapElem<T> first;
        BitmapElem<T> second;

    public:
        auto operator=(DoubleBitmapElem<T>&&) noexcept -> DoubleBitmapElem<T>& = default;
        auto operator=(const DoubleBitmapElem<T>&) noexcept -> DoubleBitmapElem<T>& = default;

        constexpr auto operator=(cerb::pair<u8, u8> newValues) noexcept -> DoubleBitmapElem<T>& {
            first = newValues.first;
            second = newValues.second;

            return *this;
        }

    public:
        ~DoubleBitmapElem() = default;
        DoubleBitmapElem(DoubleBitmapElem&) = default;
        DoubleBitmapElem(DoubleBitmapElem&&) noexcept = default;

        constexpr DoubleBitmapElem(u16 bitIndex, T *elem1, T *elem2) noexcept
            : first(bitIndex, elem1), second(bitIndex, elem2)
        {}
    };

    template<u8 FirstValue, u8 SecondValue, size_t Size, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if(
            const T *data1,
            const T *data2,
            size_t limit
        ) noexcept -> size_t
    {
        size_t i = 0;
        auto reverse_function = [](T value1, T value2) -> T {
            if constexpr (FirstValue == 1) {
                value1 = ~value1;
            }
            if constexpr (SecondValue) {
                value2 = ~value2;
            }

            return value1 | value2;
        };

        #pragma unroll 4
        for (; i < limit / bitsizeof(T); ++i) {
            T value = reverse_function(data1[i], data2[i]);

            if (value < std::numeric_limits<T>::max()) {
                return i * bitsizeof(T) + cerb::findFreeBitForward(static_cast<u64>(value));
            }
        }

        T value = reverse_function(data1[i], data2[i]);

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

    template<u8 FirstValue, u8 SecondValue, size_t Size, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if(
            const std::array<T, Size> &data1,
            const std::array<T, Size> &data2,
            size_t limit
        ) noexcept -> size_t
    {
        size_t i = 0;
        auto reverse_function = [](T value1, T value2) -> T {
            if constexpr (FirstValue == 1) {
                value1 = ~value1;
            }
            if constexpr (SecondValue) {
                value2 = ~value2;
            }

            return value1 | value2;
        };

        #pragma unroll 4
        for (; i < limit / bitsizeof(T); ++i) {
            T value = reverse_function(data1[i], data2[i]);

            if (value < std::numeric_limits<T>::max()) {
                return i * bitsizeof(T) + cerb::findFreeBitForward(static_cast<u64>(value));
            }
        }

        T value = reverse_function(data1[i], data2[i]);

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
#endif //CERBERUS_DOUBLEBITMAPBASE_HPP
