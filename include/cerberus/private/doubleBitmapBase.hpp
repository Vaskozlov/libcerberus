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

    template<u8 FirstValue, u8 SecondValue, typename T> CERBLIB_INLINE
    auto reverse_function(T value1, T value2) {
        if constexpr (FirstValue == 0) {
            value1 = ~value1;
        }
        if constexpr (SecondValue == 0) {
            value2 = ~value2;
        }

        return value1 & value2;
    };

    template<u8 FirstValue, u8 SecondValue, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if(
            T data1,
            T data2,
            size_t limit
        ) noexcept -> size_t
    {
        size_t i = 0;

        CERBLIB_UNROLL_N(4)
        for (; i < limit / bitsizeof(data1[0]); ++i) {
            auto value = reverse_function<FirstValue, SecondValue>(data1[i], data2[i]);

            if (value != 0) {
                return i * bitsizeof(data1[0]) + cerb::findSetBitForward(static_cast<u64>(value));
            }
        }

        auto value = reverse_function<FirstValue, SecondValue>(data1[i], data2[i]);

        if (value != 0) {
            auto bitIndex = cerb::findSetBitForward(static_cast<u64>(value));
            return cerb::cmov<size_t>(
                    bitIndex < limit % bitsizeof(data1[0]),
                    i * bitsizeof(data1[0]) + bitIndex,
                    std::numeric_limits<size_t>::max()
            );
        }
        return std::numeric_limits<size_t>::max();
    }

    template<u8 FirstValue, u8 SecondValue, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if(T data1, T data2, size_t times, size_t limit) {
        size_t i = 0;
        size_t matches = 1;
        size_t start_index = 0;

        for (; i < limit / bitsizeof(data1[0]); ++i) {
            auto value = reverse_function<FirstValue, SecondValue>(data1[i], data2[i]);

            while (true) {
                if (value != 0) {
                    size_t index = cerb::findSetBitForward(value);

                    if (matches == 1 || index != 0) {
                        start_index = i * bitsizeof(data1[0]) + index;
                        matches = 1;
                    }

                    auto bit_test = cerb::pow2<size_t>(index);
                    value &= ~bit_test;
                    bit_test <<= 1;
                    ++index;

                    for (; index < bitsizeof(data1[0]); ++index) {
                        if (matches == times) {
                            return start_index;
                        } else if ((value & bit_test) == bit_test) {
                            ++matches;
                            value &= ~bit_test;
                            bit_test <<= 1;
                        } else {
                            matches = 1;
                            start_index = 0;
                            break;
                        }
                    }
                } else {
                    break;
                }
            }
        }

        auto value = reverse_function<FirstValue, SecondValue>(data1[i], data2[i]);

        if (value != 0) {
            size_t index = cerb::findSetBitForward(value);
            size_t bit_test = cerb::pow2<size_t>(index);

            if (matches == 1 || index != 0) {
                start_index = i * bitsizeof(data1[0]) + index;
                matches = 1;
            }

            for (; index < bitsizeof(data1[0]) && index < limit % bitsizeof(data1[0]); ++index) {
                if (matches == times) {
                    return start_index;
                } else if ((value & bit_test) == bit_test) {
                    ++matches;
                    value &= ~bit_test;
                    bit_test <<= 1;
                } else {
                    matches = 1;
                    start_index = 0;
                    break;
                }
            }
        }

        return UINTMAX_MAX;
    }
} // namespace cerb::PRIVATE

#pragma pack (pop)
#endif //CERBERUS_DOUBLEBITMAPBASE_HPP
