#ifndef cerberusBitmapAPI_hpp
#define cerberusBitmapAPI_hpp

#include <utility>
#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include <cerberus/string.hpp>

#pragma pack (push, 1)

namespace cerb::PRIVATE {
    using bitmap_value_type = uintmax_t;
    constexpr auto bitmap_elem_size = bitsizeof(bitmap_value_type);

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
    auto reverse_function(T value) -> T{
        if constexpr (FirstValue == 1) {
            return value;
        } else {
            return ~value;
        }
    };

    template<typename T> [[nodiscard]] constexpr
    auto at(T data, size_t index) noexcept -> u8 {
        auto elemIndex = index / bitmap_elem_size;
        auto bitIndex  = index % bitmap_elem_size;
        return (data[elemIndex] & (static_cast<bitmap_value_type>(1) << bitIndex)) != 0;
    }

    template<typename T> [[nodiscard]] constexpr
    auto isEmpty(T data, size_t limit) noexcept -> bool {
        size_t i = 0;

        CERBLIB_UNROLL_N(4)
        for (; i < limit / bitmap_elem_size; i++) {
            if (data[i] > 0) {
                return false;
            }
        }

        return (data[i] % cerb::pow2<size_t>(limit % bitmap_elem_size)) == 0;
    }

    template<u8 FirstValue, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if(T data, size_t limit) noexcept -> size_t {
        size_t i = 0;

        CERBLIB_UNROLL_N(2)
        for (; i < limit / bitmap_elem_size; ++i) {
            auto value = reverse_function<FirstValue>(data[i]);

            if (value != 0) {
                return i * bitmap_elem_size + cerb::findSetBitForward(static_cast<u64>(value));
            }
        }

        auto value = reverse_function<FirstValue>(data[i]);

        if (value != 0) {
            auto bitIndex = cerb::findSetBitForward(static_cast<u64>(value));
            return cerb::cmov<size_t>(
                    bitIndex < limit % bitmap_elem_size,
                    i * bitmap_elem_size + bitIndex,
                    std::numeric_limits<size_t>::max()
            );
        }

        return std::numeric_limits<size_t>::max();
    }

    template<u8 Value, typename T> constexpr
    auto bitmap_set(T data, size_t position, size_t times) -> void {
        size_t index = position / bitmap_elem_size;
        size_t before_alignment = cerb::min(times, bitmap_elem_size - position % bitmap_elem_size) % bitmap_elem_size;

        if (before_alignment % bitmap_elem_size != 0) {
            times -= before_alignment;
            before_alignment = cerb::pow2<size_t>(before_alignment) - 1;

            if constexpr (Value == 1) {
                data[index++] |= (static_cast<bitmap_value_type>(Value) * before_alignment) << position % bitmap_elem_size;
            }
            else {
                data[index++] &= ~((static_cast<bitmap_value_type>(Value) * before_alignment) << position % bitmap_elem_size);
            }
        }

        auto i = static_cast<intmax_t>(times);

        for (; i - static_cast<intmax_t>(bitmap_elem_size) >= 0; i -= bitmap_elem_size) {
            if constexpr (Value == 1) {
                data[index++] |= ~static_cast<bitmap_value_type>(0);
            }
            else {
                data[index++] = 0;
            }
        }

        if (i == 0) {
            return;
        }

        size_t after_alignment = cerb::pow2<size_t>(i) - 1;

        if constexpr (Value == 1) {
            data[index] |= static_cast<bitmap_value_type>(Value) * after_alignment;
        }
        else {
            data[index] &= ~(static_cast<bitmap_value_type>(Value) * after_alignment);
        }
    }

    template<u8 Value, typename T> [[nodiscard]] constexpr
    auto bitmap_is_set(T data, size_t position, size_t times) -> bool {
        size_t index = position / bitmap_elem_size;
        size_t before_alignment = cerb::min(times, bitmap_elem_size - position % bitmap_elem_size);
        times -= before_alignment;

        before_alignment = cerb::pow2<size_t>(before_alignment) - 1;
        before_alignment = before_alignment << position % bitmap_elem_size;

        if ((reverse_function<Value>(data[index]) & before_alignment) != before_alignment) {
            return false;
        }

        ++index;

        auto i = static_cast<intmax_t>(times);
        constexpr auto comparator = std::numeric_limits<bitmap_value_type>::max() * Value;

        for (; i - static_cast<intmax_t>(bitmap_elem_size) >= 0; i -= bitmap_elem_size) {
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
        long last_match = 0;
        size_t i = 0, matches = 0;

        for (; i < limit / bitmap_elem_size; ++i) {
            auto value = reverse_function<FirstValue>(data[i]);

            if (last_match == bitmap_elem_size - 1) {
                last_match = -1;
            }

            while (value > 0) {
                long new_match = cerb::findSetBitForward(value);

                if (new_match - last_match == 1 || matches == 0) {
                    ++matches;
                } else {
                    matches = 0;
                }

                if (matches == times) UNLIKELY {
                    return i * bitmap_elem_size + (cerb::bit_cast<unsigned long>(new_match) - times);
                }

                last_match = new_match;
                value &= ~(static_cast<decltype(value)>(1) << new_match);
            }
        }

        return std::numeric_limits<size_t>::max();
    }

    template<u8 FirstValue, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if(T data, size_t times, size_t limit) -> size_t {
        if (times >= bitmap_elem_size) UNLIKELY {
            return bitmap_long_find_if<FirstValue>(data, times, limit);
        }

        size_t i = 0;

        for (; i < limit / bitmap_elem_size; ++i) {
            auto mask = cerb::pow2<size_t>(times) - 1;
            auto value = reverse_function<FirstValue>(data[i]);

            if (value != 0) UNLIKELY {
                size_t j = 0;

                for (; j < bitmap_elem_size - times; ++j) {
                    if ((value & mask) == mask) {
                        return i * bitmap_elem_size + j;
                    }
                    mask <<= 1;
                }

                if ((value & mask) == value) {
                    // we get amount of remaining bits in the number
                    auto remainder = cerb::findFreeBitReverse(value & mask) - 1;
                    mask = cerb::pow2<size_t>(times - remainder) - 1;

                    if (
                            (i + 1 < limit / bitmap_elem_size ||
                            limit < (i + 1) * bitmap_elem_size - (times - remainder)) &&
                            (reverse_function<FirstValue>(data[i + 1]) & mask) == mask
                    ) {
                        return (i + 1) * bitmap_elem_size - (times - remainder);
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
        if (times >= bitmap_elem_size) UNLIKELY {
            bitmap_find_if<FirstValue>(data, times, limit);
        }

        size_t i = 0;
        size_t matches = 0;
        auto m_mask = cerb::pow2<size_t>(times) - 1;

        for (; i < limit / bitmap_elem_size; ++i) {
            auto value = reverse_function<FirstValue>(data[i]);

            if (value != 0) {

                size_t j = 0;
                for (; j < bitmap_elem_size - times; ++j) {
                    if ((value & m_mask) == m_mask) {
                        return i * bitmap_elem_size + j;
                    }
                    m_mask *= 2ULL;
                }

                m_mask = cerb::pow2<size_t>(times - cerb::findFreeBitReverse(value) - 1) - 1;
                if ((reverse_function<FirstValue>(data[i + 1]) & m_mask) == m_mask) {
                    return (i) * bitmap_elem_size + (bitmap_elem_size - cerb::findFreeBitReverse(value));
                }
            }
        }
        return std::numeric_limits<size_t>::max();
    }
 */