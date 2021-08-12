#include <iostream>
#include <cerberus/bit.hpp>
#include <cerberus/map.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/doubleBitmap.hpp>

#include <deque>
#include <chrono>
#include <cerberus/map.hpp>

consteval auto TEST() -> int {
    cerb::constBitMap<128> a;
    //a.set(0, 1);
    a.clear();
    a[0] = 1;

    a.at(0);
    a.isEmpty();
    a.find_if<1>();

    cerb::constBitMap<128> b;
    b = a;
    return 0;
}

auto TEST2() -> size_t {
    u64 buf[2] = {0};
    cerb::bitmap<true> a(buf, 128);
    a.set<1>(0);
    a.clear();
    a[0] = 1;

    a.at(0);
    a.isEmpty();
    return a.isEmpty();
}

constexpr auto TEST3() -> bool{
    cerb::constDoubleBitmap<128> a;
    a.clear();
    a.set<1>(0);
    a.set<1>(1);

    return a.isEmpty1();
}

auto TEST4() -> int {
    cerb::DoubleBitmap a(128);

    a.clear();
    a.set<1>(0);
    a.set<1>(1);

    return a.isEmpty1();
}

consteval auto TEST7() -> bool {
    cerb::constBitMap<256> a;

    for (int i = 0; i < 60; i++) {
        a.set<1>(i);
    }
    //a.set<1>(60);
    a.set<1>(130);

    return a.is_value_set<0>(60, 70);
}

consteval auto TEST8() -> int {
    std::array<int, 10> a {} ;
    cerb::memset(a, 0, 10);
    return 0;
}

#include <set>
#include <cerberus/container.hpp>
using namespace cerb::literals;

namespace cerb {
    enum BitMapRule : u8 {
        BIT_SET,
        BIT_FREE,
        ANY_BIT
    };
}

namespace cerb::experimental::PRIVATE {
    using value_type = uintmax_t;

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

    template<BitMapRule... Values> [[nodiscard]] constexpr
    auto joinValues() -> u64 {
        u64 result = 0;

        forEach(
            [&](auto i){
                result = (result << 1) + static_cast<u8>(i);
            }, Values...
        );

        return result;
    }

    template<BitMapRule Value, typename T> [[nodiscard]] constexpr
    auto reverse(size_t index, T value) -> value_type{
        if constexpr (Value == BIT_SET) {
            return (*value)[index];
        } else if constexpr (Value == ANY_BIT) {
            return std::numeric_limits<value_type>::max();
        } else {
            return ~(*value)[index];
        }
    }

    template<BitMapRule Value, BitMapRule... Values, typename T> [[nodiscard]] constexpr
    auto reverse(size_t index, T data) -> value_type {
        return reverse<Value, T>(index, data) & reverse<Values..., T>(index, data + 1);
    }

    template<u8 value, typename T> constexpr
    auto set(T data, size_t index) {
        auto arrayIndex = index / bitsizeof(value_type);
        auto bitIndex = index % bitsizeof(value_type);

        if constexpr (value == 1) {
            data[arrayIndex] |= static_cast<value_type>(1) << bitIndex;
        } else {
            data[arrayIndex] &= ~(static_cast<value_type>(1) << bitIndex);
        }
    }

    template<u8 value, size_t AxisCount, typename T> constexpr
    auto set(T data, size_t index) -> void {
        auto arrayIndex = index / bitsizeof(value_type);
        auto bitIndex = index % bitsizeof(value_type);

        constexprFor<0, AxisCount, 1>(
            [&](auto i){
                if constexpr (value == 1) {
                    data[i.value][arrayIndex] |= static_cast<value_type>(1) << bitIndex;
                } else {
                    data[i.value][arrayIndex] &= ~(static_cast<value_type>(1) << bitIndex);
                }
            }
        );
    }

    template<typename T> [[nodiscard]] constexpr
    auto isEmpty(T data, size_t limit) noexcept -> bool {
        size_t i = 0;

        CERBLIB_UNROLL_N(4)
        for (; i < limit / bitsizeof(value_type); ++i) {
            if (data[i] > 0) {
                return false;
            }
        }

        return (data[i] % pow2<value_type>(limit % bitsizeof(value_type))) == 0;
    }

    template<typename T, size_t AxisCount> [[nodiscard]] constexpr
    auto isEmpty(T data, size_t limit) noexcept -> bool {
        for (size_t i = 0; i < AxisCount && isEmpty(data[i], limit); ++i);
        return true;
    }

    template<BitMapRule... Values, typename T> [[nodiscard]] constexpr
    auto find_if(size_t limit, T iterator) noexcept -> size_t {
        size_t i = 0;

        CERBLIB_UNROLL_N(2)
        for (; i < limit / bitsizeof(value_type); ++i) {
            auto value = reverse<Values..., T>(i, iterator);

            if (value != 0) {
                return i * bitsizeof(value_type) + findSetBitForward(value);
            }
        }

        auto value = reverse<Values..., T>(i, iterator);
        if (value != 0) {
            auto bitIndex = findSetBitForward(value);

            return cmov<size_t>(
                bitIndex < limit % bitsizeof(value_type),
                i * bitsizeof(value_type) + bitIndex,
                std::numeric_limits<size_t>::max()
            );
        }

        return std::numeric_limits<size_t>::max();
    }

    template<BitMapRule Value, typename T> [[nodiscard]] constexpr
    auto axis_find_if(size_t limit, T data) noexcept -> size_t {
        size_t i = 0;

        CERBLIB_UNROLL_N(2)
        for (; i < limit / bitsizeof(value_type); ++i) {
            auto value = reverse<Value, T>(i, data);

            if (value != 0) {
                return i * bitsizeof(value_type) + findSetBitForward(value);
            }
        }

        auto value = reverse<Value, T>(i, data);
        if (value != 0) {
            auto bitIndex = findSetBitForward(value);

            return cmov<size_t>(
                    bitIndex < limit % bitsizeof(value_type),
                    i * bitsizeof(value_type) + bitIndex,
                    std::numeric_limits<size_t>::max()
            );
        }

        return std::numeric_limits<size_t>::max();
    }

    template<BitMapRule... Values, typename T> [[nodiscard]] constexpr
    auto long_find_if(size_t limit, size_t times, T iterator) noexcept -> size_t {
        long last_match = 0;
        size_t i = 0, matches = 0;

        for (; i < limit / bitsizeof(value_type); ++i) {
            auto value = reverse<Values..., T>(i, iterator);

            if (last_match == bitsizeof(value_type) - 1) {
                last_match = -1;
            }

            while (value > 0) {
                long new_match = findSetBitForward(value);

                if (new_match - last_match == 1 || matches == 0) {
                    ++matches;
                } else {
                    matches = 0;
                }

                if (matches == times) UNLIKELY {
                    return i * bitsizeof(value_type) + bit_cast<value_type>(new_match) - times;
                }

                last_match = new_match;
                value &= ~(static_cast<value_type>(1) << last_match);
            }
        }

        return std::numeric_limits<value_type>::max();
    }

    template<BitMapRule... Values, typename T> [[nodiscard]] constexpr
    auto find_if(size_t limit, size_t times, T iterator) noexcept -> size_t {
        if (times > bitsizeof(value_type)) UNLIKELY {
            return long_find_if<Values...>(limit, times, iterator);
        }

        size_t i = 0;

        for (; i != limit / bitsizeof(value_type); ++i) {
            auto mask = pow2<value_type>(times) - 1;
            auto value = reverse<Values..., T>(i, iterator);

            if (value >= mask) {
                size_t j = 0;

                for (; j != bitsizeof(value_type); ++j) {
                    if ((value & mask) == mask) {
                        return i * bitsizeof(value_type) + j;
                    }
                    mask <<= 1; // mask *= 2;
                }

                if ((value & mask) == value) {
                    // we need to know how many bits are suitable at the end of the number
                    auto remainder = findFreeBitReverse(value & mask) - 1;
                    mask = pow2<value_type>(times - remainder) - 1; // mask of missing bits

                    if (
                            (i + 1 < limit / bitsizeof(value_type) ||
                            limit < (i + 1) * bitsizeof(value_type) - (times - remainder)) &&
                                    (reverse<Values..., T>(i + 1, iterator) & mask) == mask
                    ) {
                        return (i + 1) * bitsizeof(value_type) - (times - remainder);
                    }
                }
            }
        }

        auto mask = cerb::pow2<value_type>(times) - 1;
        auto value = reverse<Values..., T>(i, iterator);

        if (value >= mask) {
            size_t j = 0;

            for (; j != bitsizeof(value_type) && j + times < limit % bitsizeof(value_type); ++j) {
                if ((value & mask) == mask) {
                    return i * bitsizeof(value_type) + j;
                }
                mask <<= 1; // mask *= 2;
            }
        }

        return std::numeric_limits<value_type>::max();
    }

    template<BitMapRule... Values, typename T> [[nodiscard]] constexpr
    auto is_set(size_t location, size_t times, T iterator) noexcept -> bool {
        const auto bits_to_align = location % bitsizeof(value_type);

        size_t index = location / bitsizeof(value_type);

        if (bits_to_align != 0) {
            size_t before_alignment = min(bitsizeof(value_type) - bits_to_align, times);

            times -= before_alignment;

            before_alignment = pow2<size_t>(before_alignment) - 1;
            before_alignment = before_alignment << bits_to_align;

            if ((reverse<Values..., T>(index++, iterator) & before_alignment) != before_alignment) {
                return false;
            }
        }

        for (; times >= bitsizeof(value_type); times -= bitsizeof(value_type)) {
            if (reverse<Values..., T>(index++, iterator) != std::numeric_limits<value_type>::max()) {
                return false;
            }
        }

        if (times == 0) {
            return true;
        } else {
            size_t after_alignment = pow2<size_t>(times) - 1;
            return (reverse<Values..., T>(index, iterator) & after_alignment) == after_alignment;
        }
    }

    template<typename T> [[nodiscard]] constexpr
    auto atAxis(size_t index, T data) -> u8 {
        auto arrayIndex = index / bitsizeof(value_type);
        auto bitIndex = index % bitsizeof(value_type);

        return (data[arrayIndex] & (1<<bitIndex)) != 0;
    }
}

namespace cerb::experimental {

    template<size_t Axis, size_t Size>
    class ConstBitmap {
        static_assert(Axis != 0);

    public:
        using size_type = size_t;
        using value_type = PRIVATE::value_type;
        using const_value_type = const value_type;
        using pointer = value_type*;
        using const_pointer = const pointer;

    private:
        constexpr static size_type array_size = Size / bitsizeof(value_type) +
                                                ((Size % bitsizeof(value_type)) != 0);

    public:
        using storage_elem_t            = std::array<value_type, Size>;
        using ref_storage_elem_t        = std::array<value_type, Size>&;
        using const_storage_elem_t      = const std::array<value_type, Size>;
        using const_ref_storage_elem_t  = const std::array<value_type, Size> &;

        using storage_t                 = std::array<storage_elem_t, Axis>;
        using ref_storage_t             = std::array<storage_elem_t, Axis>&;
        using const_storage_t           = const std::array<storage_elem_t, Axis>;
        using const_ref_storage_t       = const std::array<storage_elem_t, Axis>&;

    private:
        storage_t m_data {};

    public:
        [[nodiscard]] constexpr
        auto size() const noexcept -> size_type {
            return Size;
        }

        [[nodiscard]] constexpr
        auto axis() const noexcept -> size_type {
            return Axis;
        }

        [[nodiscard]] constexpr
        auto lengthOfAxisArray() const noexcept -> size_type {
            return array_size;
        }

        [[nodiscard]] constexpr
        auto sizeOfAxisArray() const noexcept -> size_type {
            return lengthOfAxisArray() * sizeof(value_type);
        }

        [[nodiscard]] constexpr
        auto sizeOfStorage() const noexcept -> size_type {
            return sizeOfAxisArray() * axis();
        }

        template<size_type AxisN> [[nodiscard]] constexpr
        auto data() noexcept -> pointer {
            static_assert(AxisN < Axis);
            return m_data[AxisN].data();
        }

        template<size_type AxisN> [[nodiscard]] constexpr
        auto data() const noexcept -> pointer {
            static_assert(AxisN < Axis);
            return m_data[AxisN].data();
        }

        template<size_type AxisN> [[nodiscard]] constexpr
        auto array() noexcept -> ref_storage_elem_t {
            return m_data[AxisN];
        }

        template<size_type AxisN> [[nodiscard]] constexpr
        auto array() const noexcept -> ref_storage_elem_t {
            return m_data[AxisN];
        }

        [[nodiscard]] constexpr
        auto storage() noexcept -> ref_storage_t {
            return m_data;
        }

        [[nodiscard]] constexpr
        auto storage() const noexcept -> ref_storage_t {
            return m_data;
        }

    public:
        template<size_type AxisN> constexpr
        auto clear() noexcept -> void {
            static_assert(AxisN < Axis);
            cerb::memset<value_type>(m_data[AxisN], 0, lengthOfAxisArray());
        }

        constexpr auto clear() noexcept -> void {
            constexprFor<0, Axis, 1>(
                [&](auto i){
                    clear<i.value>();
                }
            );
        }

    public:
        template<u8 value> constexpr
        auto set(size_type index) noexcept -> void {
            PRIVATE::set<value, Axis, ref_storage_t>(m_data, index);
        }

        template<u8 value, size_type AxisN> constexpr
        auto set(size_type index) noexcept -> void {
            static_assert(AxisN < Axis);
            PRIVATE::set<value, ref_storage_elem_t>(m_data[AxisN], index);
        }

        template<size_type AxisN> [[nodiscard]] constexpr
        auto isEmpty() const noexcept -> bool {
            static_assert(AxisN < Axis);
            return PRIVATE::isEmpty<const_ref_storage_t>(m_data[AxisN], size());
        }

        [[nodiscard]] constexpr
        auto isEmpty() const noexcept -> bool {
            return PRIVATE::isEmpty<const_ref_storage_t, Axis>(m_data, size());
        }

        template<BitMapRule... Values> [[nodiscard]] constexpr
        auto find() const noexcept -> size_type {
            return PRIVATE::find_if<Values...>(size(), m_data.begin());
        }

        template<int AxisN, BitMapRule Value> [[nodiscard]] constexpr
        auto axis_find() const noexcept -> size_type {
            static_assert(AxisN < Axis);
            return PRIVATE::find_if<Value>(size(), m_data.begin() + AxisN);
        }

        template<BitMapRule... Values> [[nodiscard]] constexpr
        auto find(size_type times) const noexcept -> size_type {
            return PRIVATE::find_if<Values...>(size(), times, m_data.begin());
        }

        template<size_type AxisN, BitMapRule Value> [[nodiscard]] constexpr
        auto axis_find(size_type times) const noexcept-> size_type {
            static_assert(AxisN < Axis);
            return PRIVATE::find_if<Value>(size(), times, m_data.begin() + AxisN);
        }

        template<BitMapRule... Values> [[nodiscard]] constexpr
        auto is_set(size_type index, size_type times) const noexcept -> bool {
            return PRIVATE::is_set<Values...>(index, times, m_data.begin());
        }

        template<size_type AxisN, BitMapRule Values> [[nodiscard]] constexpr
        auto axis_is_set(size_type index, size_type times) const noexcept -> bool {
            static_assert(AxisN < Axis);
            return PRIVATE::is_set<Values>(index, times, m_data.begin() + AxisN);
        }

        [[nodiscard]] constexpr
        auto at(size_t axisN, size_t index) noexcept -> u8 {
            return PRIVATE::atAxis<const_ref_storage_elem_t>(axisN, index, m_data[axisN]);
        }

        template<size_type AxisN> [[nodiscard]] constexpr
        auto at(size_t index) noexcept -> u8 {
            static_assert(AxisN < Axis);
            return PRIVATE::atAxis<const_ref_storage_elem_t>(index, m_data[AxisN]);
        }
    };
}

constexpr auto TEST10() {
    cerb::experimental::ConstBitmap<2, 128> a;

    a.set<1, 1>(10);
    a.set<1>(11);
    a.set<1>(12);
    a.set<1>(13);

    a.clear<0>();
    return a.at<1>(12);
}

#include <functional>
#include <cerberus/set.hpp>

auto main(int argc, char *argv[]) -> int {

    cerb::Set<int, cerb::Throwable {}> a {};
    a.insert(10);
    a.insert(20);
    a.insert(30);
    a.insert(40);
    a.insert(50);
    a.insert(60);
    a.insert(70);
    auto b = a.begin();
    ++b;
    std::cout << *b << std::endl;

    return 0;
}