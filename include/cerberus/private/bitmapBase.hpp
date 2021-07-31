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

    constexpr inline u64 notBit[] = {
            0xffffffffffffffff,
            0xfffffffffffffffe, 0xfffffffffffffffd,
            0xfffffffffffffffb, 0xfffffffffffffff7,
            0xffffffffffffffef, 0xffffffffffffffdf,
            0xffffffffffffffbf, 0xffffffffffffff7f,
            0xfffffffffffffeff, 0xfffffffffffffdff,
            0xfffffffffffffbff, 0xfffffffffffff7ff,
            0xffffffffffffefff, 0xffffffffffffdfff,
            0xffffffffffffbfff, 0xffffffffffff7fff,
            0xfffffffffffeffff, 0xfffffffffffdffff,
            0xfffffffffffbffff, 0xfffffffffff7ffff,
            0xffffffffffefffff, 0xffffffffffdfffff,
            0xffffffffffbfffff, 0xffffffffff7fffff,
            0xfffffffffeffffff, 0xfffffffffdffffff,
            0xfffffffffbffffff, 0xfffffffff7ffffff,
            0xffffffffefffffff, 0xffffffffdfffffff,
            0xffffffffbfffffff, 0xffffffff7fffffff,
            0xfffffffeffffffff, 0xfffffffdffffffff,
            0xfffffffbffffffff, 0xfffffff7ffffffff,
            0xffffffefffffffff, 0xffffffdfffffffff,
            0xffffffbfffffffff, 0xffffff7fffffffff,
            0xfffffeffffffffff, 0xfffffdffffffffff,
            0xfffffbffffffffff, 0xfffff7ffffffffff,
            0xffffefffffffffff, 0xffffdfffffffffff,
            0xffffbfffffffffff, 0xffff7fffffffffff,
            0xfffeffffffffffff, 0xfffdffffffffffff,
            0xfffbffffffffffff, 0xfff7ffffffffffff,
            0xffefffffffffffff, 0xffdfffffffffffff,
            0xffbfffffffffffff, 0xff7fffffffffffff,
            0xfeffffffffffffff, 0xfdffffffffffffff,
            0xfbffffffffffffff, 0xf7ffffffffffffff,
            0xefffffffffffffff, 0xdfffffffffffffff,
            0xbfffffffffffffff, 0x7fffffffffffffff
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

        CERBLIB_UNROLL_N(2)
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

    template<u8 FirstValue, size_t Size, typename T> [[nodiscard]] constexpr
    auto bitmap_find_if(const std::array<T, Size> &data, size_t times, size_t limit) {
        size_t i = 0;
        size_t matches = 1;
        size_t start_index = 0;

        auto reverse_function = [](T value) {
            if constexpr (FirstValue == 1) {
                return value;
            } else {
                return ~value;
            }
        };

        for (; i < limit / bitsizeof(T); ++i) {
            T value = reverse_function(data[i]);

            while (true) {
                if (value != 0) {
                    size_t index = cerb::findSetBitForward(value);
                    size_t bit_test = cerb::pow2<T>(index);

                    if (matches == 1 || index != 0) {
                        start_index = i * bitsizeof(T) + index;
                        matches = 1;
                    }

                    for (; index < bitsizeof(T); ++index) {
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

        T value = reverse_function(data[i]);

        if (value != 0) {
            size_t index = cerb::findSetBitForward(value);
            size_t bit_test = cerb::pow2<T>(index);

            if (matches == 1 || index != 0) {
                start_index = i * bitsizeof(T) + index;
                matches = 1;
            }

            for (; index < bitsizeof(T) && index < limit % bitsizeof(T); ++index) {
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

        CERBLIB_UNROLL_N(2)
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
