#ifndef cerberusBitmapAPI_hpp
#define cerberusBitmapAPI_hpp

#include <utility>
#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include <cerberus/string.hpp>

#pragma pack (push, 1)

namespace cerb::PRIVATE {
    constexpr inline u64 value2bits[] = {
            0x0, 0x1, 0x3,
            0x7, 0xf, 0x1f,
            0x3f, 0x7f, 0xff,
            0x1ff, 0x3ff, 0x7ff,
            0xfff, 0x1fff, 0x3fff,
            0x7fff, 0xffff, 0x1ffff,
            0x3ffff, 0x7ffff, 0xfffff,
            0x1fffff, 0x3fffff, 0x7fffff,
            0xffffff, 0x1ffffff, 0x3ffffff,
            0x7ffffff, 0xfffffff, 0x1fffffff,
            0x3fffffff, 0x7fffffff, 0xffffffff,
            0x1ffffffff, 0x3ffffffff, 0x7ffffffff,
            0xfffffffff, 0x1fffffffff, 0x3fffffffff,
            0x7fffffffff, 0xffffffffff, 0x1ffffffffff,
            0x3ffffffffff, 0x7ffffffffff, 0xfffffffffff,
            0x1fffffffffff, 0x3fffffffffff, 0x7fffffffffff,
            0xffffffffffff, 0x1ffffffffffff, 0x3ffffffffffff,
            0x7ffffffffffff, 0xfffffffffffff, 0x1fffffffffffff,
            0x3fffffffffffff, 0x7fffffffffffff, 0xffffffffffffff,
            0x1ffffffffffffff, 0x3ffffffffffffff, 0x7ffffffffffffff,
            0xfffffffffffffff, 0x1fffffffffffffff, 0x3fffffffffffffff,
            0x7fffffffffffffff, 0xffffffffffffffff
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

    template<u8 FirstValue, typename T> constexpr
    auto reverse_function(T value) {
        if constexpr (FirstValue == 1) {
            return value;
        } else {
            return ~value;
        }
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

    template<typename T> [[nodiscard]] constexpr
    auto isEmpty(T data, size_t limit) noexcept -> bool {
        size_t i = 0;

        CERBLIB_UNROLL_N(4)
        for (; i < limit / bitsizeof(data[0]); i++) {
            if (data[i] > 0) {
                return false;
            }
        }

        return (data[i] % cerb::pow2<size_t>(limit % bitsizeof(data[0]))) == 0;
    }

    template<u8 FirstValue, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if(T data, size_t limit) noexcept -> size_t {
        size_t i = 0;

        CERBLIB_UNROLL_N(2)
        for (; i < limit / bitsizeof(data[0]); ++i) {
            auto value = reverse_function<FirstValue>(data[i]);

            if (value != 0) {
                return i * bitsizeof(data[0]) + cerb::findSetBitForward(static_cast<u64>(value));
            }
        }

        auto value = reverse_function<FirstValue>(data[i]);

        if (value != 0) {
            auto bitIndex = cerb::findSetBitForward(static_cast<u64>(value));
            return cerb::cmov<size_t>(
                    bitIndex < limit % bitsizeof(data[0]),
                    i * bitsizeof(data[0]) + bitIndex,
                    std::numeric_limits<size_t>::max()
            );
        }

        return std::numeric_limits<size_t>::max();
    }

    // very slow function... but it can find any amount of set bits
    template<u8 FirstValue, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if(T data, size_t times, size_t limit) {
        size_t i = 0;
        size_t matches = 1;
        size_t start_index = 0;

        for (; i < limit / bitsizeof(data[0]); ++i) {
            auto value = reverse_function<FirstValue>(data[i]);

            while (true) {
                if (value != 0) {
                    size_t index = cerb::findSetBitForward(value);
                    auto bit_test = cerb::pow2<size_t>(index);

                    if (matches == 1 || index != 0) {
                        start_index = i * bitsizeof(data[0]) + index;
                        matches = 1;
                    }

                    for (; index < bitsizeof(data[0]); ++index) {
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

        auto value = reverse_function<FirstValue>(data[i]);

        if (value != 0) {
            size_t index = cerb::findSetBitForward(value);
            auto bit_test = cerb::pow2<size_t>(index);

            if (matches == 1 || index != 0) {
                start_index = i * bitsizeof(data[0]) + index;
                matches = 1;
            }

            for (; index < bitsizeof(data[0]) && index < limit % bitsizeof(data[0]); ++index) {
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

        return std::numeric_limits<size_t>::max();
    }

#include <iostream>

    template<u8 Value, typename T, typename VT> [[nodiscard]] constexpr
    auto bitmap_is_set(T data, size_t position, size_t times) -> bool {
        size_t index = position / bitsizeof(data[0]);
        size_t before_alignment = cerb::min(times, bitsizeof(data[0]) - position % bitsizeof(data[0]));
        times -= before_alignment;

        before_alignment = cerb::pow2<size_t>(before_alignment) - 1;
        before_alignment = before_alignment << position % bitsizeof(data[0]);

        if ((reverse_function<Value>(data[index]) & before_alignment) != before_alignment) {
            return false;
        }

        ++index;

        auto i = static_cast<intmax_t>(times);
        constexpr auto comparator = std::numeric_limits<VT>::max() * Value;
        for (; i - static_cast<intmax_t>(bitsizeof(data[0])) >= 0; i -= bitsizeof(data[0])) {
            if (data[index++] != comparator) {
                return false;
            }
        }

        if (i == 0) {
            return true;
        }

        size_t after_alignment = cerb::pow2<size_t>(i) - 1;

        if ((reverse_function<Value>(data[index]) & after_alignment) != after_alignment) {
            return false;
        } else {
            return true;
        }
    }

    template<u8 FirstValue, typename T> [[nodiscard]] constexpr
    auto bitmap_long_find_if(T data, size_t times, size_t limit) -> size_t {
        size_t i = 0;

        for (; i < limit / bitsizeof(data[0]); ++i) {
            auto mask = cerb::pow2<size_t>(times) - 1;
            auto value = reverse_function<FirstValue>(data[i]);

            if (value != 0) UNLIKELY {
                size_t j = 0;

                for (; j < bitsizeof(data[0]) - times; ++j) {
                    if ((value & mask) == mask) {
                        return i * bitsizeof(data[0]) + j;
                    }
                    mask <<= 1;
                }

                if ((value & mask) != 0) LIKELY {
                    // we get amount of remaining bits in the number
                    auto remainder = cerb::findFreeBitReverse(value & mask) - 1;
                    mask = cerb::pow2<size_t>(times - remainder) - 1;

                    if (
                            (i + 1 < limit / bitsizeof(data[0]) ||
                            limit < (i + 1) * bitsizeof(data[0] + times - remainder)) &&
                            (reverse_function<FirstValue>(data[i + 1]) & mask) == mask
                            ) {
                        return (i + 1) * bitsizeof(data[0]) - remainder - 1;
                    }
                }
            }
        }
        return std::numeric_limits<size_t>::max();
    }

    template<u8 FirstValue, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if2(T data, size_t times, size_t limit) -> size_t {
        if (times >= bitsizeof(data[0])) UNLIKELY {
            return bitmap_find_if<FirstValue>(data, times, limit);
        }

        size_t i = 0;

        for (; i < limit / bitsizeof(data[0]); ++i) {
            auto mask = cerb::pow2<size_t>(times) - 1;
            auto value = reverse_function<FirstValue>(data[i]);

            if (value != 0) UNLIKELY {
                size_t j = 0;

                for (; j < bitsizeof(data[0]) - times; ++j) {
                    if ((value & mask) == mask) {
                        return i * bitsizeof(data[0]) + j;
                    }
                    mask <<= 1;
                }

                if ((value & mask) != 0) LIKELY {
                    // we get amount of remaining bits in the number
                    auto remainder = cerb::findFreeBitReverse(value & mask) - 1;
                    mask = cerb::pow2<size_t>(times - remainder) - 1;

                    if (
                            (i + 1 < limit / bitsizeof(data[0]) ||
                            limit < (i + 1) * bitsizeof(data[0] + times - remainder)) &&
                            (reverse_function<FirstValue>(data[i + 1]) & mask) == mask
                    ) {
                        return (i + 1) * bitsizeof(data[0]) - remainder - 1;
                    }
                }
            }
        }
        return std::numeric_limits<size_t>::max();
    }

} // namespace cerb::PRIVATE
#pragma pack (pop)
#endif /* cerberusBitmapAPI_hpp */

/*
    template<u8 FirstValue, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if2(T data, size_t times, size_t limit) -> size_t {
        if (times >= bitsizeof(data[0])) UNLIKELY {
            bitmap_find_if<FirstValue>(data, times, limit);
        }

        size_t i = 0;
        size_t matches = 0;
        auto mask = cerb::pow2<size_t>(times) - 1;

        for (; i < limit / bitsizeof(data[0]); ++i) {
            auto value = reverse_function<FirstValue>(data[i]);

            if (value != 0) {

                size_t j = 0;
                for (; j < bitsizeof(data[0]) - times; ++j) {
                    if ((value & mask) == mask) {
                        return i * bitsizeof(data[0]) + j;
                    }
                    mask *= 2ULL;
                }

                mask = cerb::pow2<size_t>(times - cerb::findFreeBitReverse(value) - 1) - 1;
                if ((reverse_function<FirstValue>(data[i + 1]) & mask) == mask) {
                    return (i) * bitsizeof(data[0]) + (bitsizeof(data[0]) - cerb::findFreeBitReverse(value));
                }
            }
        }
        return std::numeric_limits<size_t>::max();
    }
 */