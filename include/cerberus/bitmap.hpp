#ifndef CERBERUS_BITMAP_HPP
#define CERBERUS_BITMAP_HPP

#include <cerberus/math.hpp>
#include <cerberus/string.hpp>

namespace cerb {
    enum BitMapRule : u8
    {
        BIT_SET,
        BIT_FREE,
        ANY_BIT
    };
}

namespace cerb::PRIVATE {
    using value_type = uintmax_t;

    template<typename T>
    class CERBLIB_TRIVIAL BitmapElem
    {
        u16 m_bitIndex;
        T *m_elem;

    public:
        constexpr auto value() noexcept -> u8
        {
            return (*m_elem & (static_cast<T>(1) << m_bitIndex)) != 0;
        }

    public:
        auto operator=(BitmapElem &&) noexcept -> BitmapElem & = default;
        auto operator=(const BitmapElem &) noexcept -> BitmapElem & = default;

        constexpr auto operator=(u8 newValue) noexcept -> BitmapElem &
        {
            *m_elem &= ~(static_cast<T>(1) << m_bitIndex);
            *m_elem |= static_cast<T>(newValue) << m_bitIndex;
            return *this;
        }

    public:
        ~BitmapElem()                      = default;
        BitmapElem(BitmapElem &)           = default;
        BitmapElem(BitmapElem &&) noexcept = default;

        constexpr CERBLIB_INLINE BitmapElem(u16 bitIndex, T *elem) noexcept
          : m_bitIndex(bitIndex), m_elem(elem)
        {}
    };

    template<BitMapRule... Values>
    [[nodiscard]] constexpr auto joinValues() -> u64
    {
        u64 result = 0;

        forEach(
            [&](auto i) { result = (result << 1U) + static_cast<u8>(i); },
            Values...);

        return result;
    }

    template<BitMapRule Value, typename T>
    [[nodiscard]] constexpr auto reverse(size_t index, T value) -> value_type
    {
        if constexpr (Value == BIT_SET) {
            return (*value)[index];
        } else if constexpr (Value == ANY_BIT) {
            return std::numeric_limits<value_type>::max();
        } else {
            return ~(*value)[index];
        }
    }

    template<BitMapRule Value, BitMapRule... Values, typename T>
    [[nodiscard]] constexpr auto reverse(size_t index, T data) -> value_type
    {
        return reverse<Value, T>(index, data) &
               reverse<Values..., T>(index, data + 1);
    }

    template<u8 value, typename T>
    constexpr auto set(T data, size_t index)
    {
        auto arrayIndex = index / bitsizeof(value_type);
        auto bitIndex   = index % bitsizeof(value_type);

        if constexpr (value == 1) {
            data[arrayIndex] |= static_cast<value_type>(1) << bitIndex;
        } else {
            data[arrayIndex] &= ~(static_cast<value_type>(1) << bitIndex);
        }
    }

    template<u8 value, size_t AxisCount, typename T>
    constexpr auto set(T data, size_t index) -> void
    {
        auto arrayIndex = index / bitsizeof(value_type);
        auto bitIndex   = index % bitsizeof(value_type);

        constexprFor<0, AxisCount, 1>([&](auto i) {
            if constexpr (value == 1) {
                data[i.value][arrayIndex] |= static_cast<value_type>(1) << bitIndex;
            } else {
                data[i.value][arrayIndex] &=
                    ~(static_cast<value_type>(1) << bitIndex);
            }
        });
    }

    template<typename T>
    [[nodiscard]] constexpr auto isEmpty(T data, size_t limit) noexcept -> bool
    {
        size_t i = 0;

        CERBLIB_UNROLL_N(4)
        for (; i < limit / bitsizeof(value_type); ++i) {
            if (data[i] > 0) {
                return false;
            }
        }

        return (data[i] % pow2<value_type>(limit % bitsizeof(value_type))) == 0;
    }

    template<typename T, size_t AxisCount>
    [[nodiscard]] constexpr auto isEmpty(T data, size_t limit) noexcept -> bool
    {
        CERBLIB_UNROLL_N(2)
        for (size_t i = 0; i < AxisCount && isEmpty(data[i], limit); ++i) {}
        return true;
    }

    template<BitMapRule... Values, typename T>
    [[nodiscard]] constexpr auto find_if(size_t limit, T iterator) noexcept -> size_t
    {
        size_t i = 0;

        CERBLIB_UNROLL_N(2)
        for (; i < limit / bitsizeof(value_type); ++i) {
            auto value = reverse<Values..., T>(i, iterator);

            if (value != 0) {
                return i * bitsizeof(value_type) + findSetBitForward(value);
            }
        }

        if (limit % bitsizeof(value_type) != 0) {
            auto value = reverse<Values..., T>(i, iterator);

            if (value != 0) {
                auto bitIndex = findSetBitForward(value);

                return cmov<size_t>(
                    bitIndex < limit % bitsizeof(value_type),
                    i * bitsizeof(value_type) + bitIndex,
                    std::numeric_limits<size_t>::max());
            }
        }

        return std::numeric_limits<size_t>::max();
    }

    template<BitMapRule Value, typename T>
    [[nodiscard]] constexpr auto axis_find_if(size_t limit, T data) noexcept
        -> size_t
    {
        size_t i = 0;

        CERBLIB_UNROLL_N(2)
        for (; i < limit / bitsizeof(value_type); ++i) {
            auto value = reverse<Value, T>(i, data);

            if (value != 0) {
                return i * bitsizeof(value_type) + findSetBitForward(value);
            }
        }

        if (limit % bitsizeof(value_type) != 0) {
            auto value = reverse<Value, T>(i, data);

            if (value != 0) {
                auto bitIndex = findSetBitForward(value);

                return cmov<size_t>(
                    bitIndex < limit % bitsizeof(value_type),
                    i * bitsizeof(value_type) + bitIndex,
                    std::numeric_limits<size_t>::max());
            }
        }

        return std::numeric_limits<size_t>::max();
    }

    template<BitMapRule... Values, typename T>
    [[nodiscard]] constexpr auto
        long_find_if(size_t limit, size_t times, T iterator) noexcept -> size_t
    {
        long last_match = 0;
        size_t i        = 0;
        size_t matches  = 0;

        for (; i < limit / bitsizeof(value_type); ++i) {
            auto value = reverse<Values..., T>(i, iterator);

            if (last_match == bitsizeof(value_type) - 1) {
                last_match = -1;
            }

            CERBLIB_UNROLL_N(1)
            while (value > 0) {
                long new_match = findSetBitForward(value);

                if (new_match - last_match == 1 || matches == 0) {
                    ++matches;
                } else {
                    matches = 0;
                }

                if (matches == times) [[unlikely]] {
                    return i * bitsizeof(value_type) +
                           bit_cast<value_type>(static_cast<intmax_t>(new_match)) -
                           times;
                }

                last_match = new_match;
                value &= ~(static_cast<value_type>(1U) << last_match);
            }
        }

        return std::numeric_limits<value_type>::max();
    }

    template<BitMapRule... Values, typename T>
    [[nodiscard]] constexpr auto
        find_if(size_t limit, u32 times, T iterator) noexcept -> size_t
    {
        if (times > bitsizeof(value_type)) {
            [[unlikely]];
            return long_find_if<Values...>(limit, times, iterator);
        }

        size_t i = 0;

        for (; i != limit / bitsizeof(value_type); ++i) {
            auto mask  = pow2<value_type>(times) - 1;
            auto value = reverse<Values..., T>(i, iterator);

            if (value >= mask) {
                size_t j = 0;

                CERBLIB_UNROLL_N(1)
                for (; j != bitsizeof(value_type); ++j) {
                    if ((value & mask) == mask) {
                        return i * bitsizeof(value_type) + j;
                    }
                    mask <<= 1U;
                }

                if ((value & mask) == value) {
                    // we need to know how many bits are suitable at the end of the
                    // number
                    auto remainder = findFreeBitReverse(value & mask) - 1;
                    mask           = pow2<value_type>(times - remainder) -
                           1;// mask of missing bits

                    if ((i + 1 < limit / bitsizeof(value_type) ||
                         limit < (i + 1) * bitsizeof(value_type) -
                                     (times - remainder)) &&
                        (reverse<Values..., T>(i + 1, iterator) & mask) == mask) {
                        return (i + 1) * bitsizeof(value_type) - (times - remainder);
                    }
                }
            }
        }

        if (limit % bitsizeof(value_type) != 0) {
            auto mask  = pow2<value_type>(times) - 1;
            auto value = reverse<Values..., T>(i, iterator);

            if (value >= mask) {
                size_t j = 0;

                CERBLIB_UNROLL_N(2)
                for (; j != bitsizeof(value_type) &&
                       j + times < limit % bitsizeof(value_type);
                     ++j) {
                    if ((value & mask) == mask) {
                        return i * bitsizeof(value_type) + j;
                    }
                    mask <<= 1U;
                }
            }
        }
        return std::numeric_limits<value_type>::max();
    }

    template<BitMapRule... Values, typename T>
    [[nodiscard]] constexpr auto
        is_set(size_t location, size_t times, T iterator) noexcept -> bool
    {
        const auto bits_to_align = location % bitsizeof(value_type);

        size_t index = location / bitsizeof(value_type);

        if (bits_to_align != 0) {
            size_t before_alignment =
                cerb::min(bitsizeof(value_type) - bits_to_align, times);

            times -= before_alignment;

            before_alignment = pow2<size_t>(static_cast<u32>(before_alignment)) - 1U;
            before_alignment = before_alignment << bits_to_align;

            if ((reverse<Values..., T>(index++, iterator) & before_alignment) !=
                before_alignment) {
                return false;
            }
        }

        CERBLIB_UNROLL_N(2)
        for (; times >= bitsizeof(value_type); times -= bitsizeof(value_type)) {
            if (reverse<Values..., T>(index++, iterator) !=
                std::numeric_limits<value_type>::max()) {
                return false;
            }
        }

        if (times == 0) {
            return true;
        }

        size_t after_alignment =
            pow2<size_t>(static_cast<u32>(after_alignment)) - 1U;
        return (reverse<Values..., T>(index, iterator) & after_alignment) ==
               after_alignment;
    }

    template<typename T>
    [[nodiscard]] constexpr auto atAxis(size_t index, T data) -> u8
    {
        auto arrayIndex = index / bitsizeof(value_type);
        auto bitIndex   = index % bitsizeof(value_type);

        return (data[arrayIndex] & (static_cast<value_type>(1) << bitIndex)) != 0;
    }
}// namespace cerb::PRIVATE

namespace cerb {

    template<size_t Axis, size_t Size>
    class ConstBitmap
    {
        static_assert(Axis != 0);

    public:
        using size_type        = size_t;
        using value_type       = PRIVATE::value_type;
        using const_value_type = const value_type;
        using pointer          = value_type *;
        using const_pointer    = const pointer;

    private:
        constexpr static size_type array_size =
            Size / bitsizeof(value_type) +
            static_cast<unsigned long>((Size % bitsizeof(value_type)) != 0);

    public:
        using storage_elem_t           = std::array<value_type, array_size>;
        using ref_storage_elem_t       = std::array<value_type, array_size> &;
        using const_storage_elem_t     = const std::array<value_type, array_size>;
        using const_ref_storage_elem_t = const std::array<value_type, array_size> &;

        using storage_t           = std::array<storage_elem_t, Axis>;
        using ref_storage_t       = std::array<storage_elem_t, Axis> &;
        using const_storage_t     = const std::array<storage_elem_t, Axis>;
        using const_ref_storage_t = const std::array<storage_elem_t, Axis> &;

    private:
        storage_t m_data{};

    private:
        constexpr auto copyFrom(const_ref_storage_t src) noexcept
        {
            CERBLIB_UNROLL_N(2)
            for (size_t i = 0; i < axis(); ++i) {
                memcpy<value_type>(m_data[i], src[i], lengthOfAxisArray());
            }
        }

    public:
        [[nodiscard]] constexpr auto size() const noexcept -> size_type
        {
            return Size;
        }

        [[nodiscard]] constexpr auto axis() const noexcept -> size_type
        {
            return Axis;
        }

        [[nodiscard]] constexpr auto lengthOfAxisArray() const noexcept -> size_type
        {
            return array_size;
        }

        [[nodiscard]] constexpr auto sizeOfAxisArray() const noexcept -> size_type
        {
            return lengthOfAxisArray() * sizeof(value_type);
        }

        [[nodiscard]] constexpr auto sizeOfStorage() const noexcept -> size_type
        {
            return sizeOfAxisArray() * axis();
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto data() noexcept -> pointer
        {
            static_assert(AxisN < Axis);
            return m_data[AxisN].data();
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto data() const noexcept -> pointer
        {
            static_assert(AxisN < Axis);
            return m_data[AxisN].data();
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto array() noexcept -> ref_storage_elem_t
        {
            return m_data[AxisN];
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto array() const noexcept -> ref_storage_elem_t
        {
            return m_data[AxisN];
        }

        [[nodiscard]] constexpr auto storage() noexcept -> ref_storage_t
        {
            return m_data;
        }

        [[nodiscard]] constexpr auto storage() const noexcept -> ref_storage_t
        {
            return m_data;
        }

    public:
        template<size_type AxisN>
        constexpr auto clear() noexcept -> void
        {
            static_assert(AxisN < Axis);
            memset<value_type>(m_data[AxisN], 0, lengthOfAxisArray());
        }

        constexpr auto clear() noexcept -> void
        {
            constexprFor<0, Axis, 1>([&](auto i) { clear<i.value>(); });
        }

    public:
        template<u8 value>
        constexpr auto set(size_type index) noexcept -> void
        {
            PRIVATE::set<value, Axis, ref_storage_t>(m_data, index);
        }

        template<u8 value, size_type AxisN>
        constexpr auto set(size_type index) noexcept -> void
        {
            static_assert(AxisN < Axis);
            PRIVATE::set<value, ref_storage_elem_t>(m_data[AxisN], index);
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool
        {
            static_assert(AxisN < Axis);
            return PRIVATE::isEmpty<const_ref_storage_t>(m_data[AxisN], size());
        }

        [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool
        {
            return PRIVATE::isEmpty<const_ref_storage_t, Axis>(m_data, size());
        }

        template<BitMapRule... Values>
        [[nodiscard]] constexpr auto find() const noexcept -> size_type
        {
            return PRIVATE::find_if<Values...>(size(), m_data.begin());
        }

        template<int AxisN, BitMapRule Value>
        [[nodiscard]] constexpr auto axis_find() const noexcept -> size_type
        {
            static_assert(AxisN < Axis);
            return PRIVATE::find_if<Value>(size(), m_data.begin() + AxisN);
        }

        template<BitMapRule... Values>
        [[nodiscard]] constexpr auto find(size_type times) const noexcept
            -> size_type
        {
            return PRIVATE::find_if<Values...>(size(), times, m_data.begin());
        }

        template<size_type AxisN, BitMapRule Value>
        [[nodiscard]] constexpr auto find_on_axis(size_type times) const noexcept
            -> size_type
        {
            static_assert(AxisN < Axis);
            return PRIVATE::find_if<Value>(size(), times, m_data.begin() + AxisN);
        }

        template<BitMapRule... Values>
        [[nodiscard]] constexpr auto
            is_set(size_type index, size_type times) const noexcept -> bool
        {
            return PRIVATE::is_set<Values...>(index, times, m_data.begin());
        }

        template<size_type AxisN, BitMapRule Values>
        [[nodiscard]] constexpr auto
            is_set_on_axis(size_type index, size_type times) const noexcept -> bool
        {
            static_assert(AxisN < Axis);
            return PRIVATE::is_set<Values>(index, times, m_data.begin() + AxisN);
        }

        [[nodiscard]] constexpr auto at(size_t axisN, size_t index) noexcept -> u8
        {
            return PRIVATE::atAxis<const_ref_storage_elem_t>(
                axisN, index, m_data[axisN]);
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto at(size_t index) const noexcept -> u8
        {
            static_assert(AxisN < Axis);
            return PRIVATE::atAxis<const_ref_storage_elem_t>(index, m_data[AxisN]);
        }

    public:
        constexpr auto operator=(const ConstBitmap &other) noexcept -> ConstBitmap &
        {
            if (&other == this) {
                [[unlikely]];
                return *this;
            }
            copyFrom(other.m_data);
            return *this;
        }

        constexpr auto operator=(ConstBitmap &&other) noexcept -> ConstBitmap &
        {
            copyFrom(other.m_data);
            return *this;
        }

    public:
        constexpr ConstBitmap() noexcept = default;

        constexpr ConstBitmap(const ConstBitmap &other) noexcept
        {
            copyFrom(other.m_data);
        }

        constexpr ConstBitmap(ConstBitmap &&other) noexcept
        {
            copyFrom(other.m_data);
        }

        constexpr ~ConstBitmap() noexcept = default;
    };

    template<size_t Axis, bool Freestanding = false>
    class Bitmap
    {
    public:
        using size_type        = size_t;
        using value_type       = PRIVATE::value_type;
        using const_value_type = const value_type;
        using pointer          = value_type *;
        using const_pointer    = const pointer;

    public:
        using storage_t           = std::array<pointer, Axis>;
        using ref_storage_t       = std::array<pointer, Axis> &;
        using const_storage_t     = const std::array<pointer, Axis>;
        using const_ref_storage_t = const std::array<pointer, Axis> &;

    private:
        size_type m_size{ 0 };
        storage_t m_data{};

    private:
        constexpr auto copyFrom(ref_storage_t src) noexcept
        {
            if (std::is_constant_evaluated()) {
                for (size_t i = 0; i < axis(); ++i) {
                    CERBLIB_UNROLL_N(2)
                    for (size_t j = 0; j < lengthOfAxisArray(); ++j) {
                        m_data[i][j] = src[i][j];
                    }
                }
            } else {
                memcpy<value_type>(&m_data, &src, lengthOfAxisArray() * Axis);
            }
        }

    public:
        [[nodiscard]] constexpr auto size() const noexcept -> size_type
        {
            return m_size;
        }

        [[nodiscard]] constexpr auto axis() const noexcept -> size_type
        {
            return Axis;
        }

        [[nodiscard]] constexpr auto lengthOfAxisArray() const noexcept -> size_type
        {
            return m_size / bitsizeof(value_type) +
                   static_cast<unsigned long>((m_size % bitsizeof(value_type)) != 0);
        }

        [[nodiscard]] constexpr auto sizeOfAxisArray() const noexcept -> size_type
        {
            return lengthOfAxisArray() * sizeof(value_type);
        }

        [[nodiscard]] constexpr auto sizeOfStorage() const noexcept -> size_type
        {
            return sizeOfAxisArray() * axis();
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto data() noexcept -> pointer
        {
            static_assert(AxisN < Axis);
            return m_data[AxisN].data();
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto data() const noexcept -> pointer
        {
            static_assert(AxisN < Axis);
            return m_data[AxisN].data();
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto array() noexcept -> pointer
        {
            return m_data[AxisN];
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto array() const noexcept -> pointer
        {
            return m_data[AxisN];
        }

        [[nodiscard]] constexpr auto storage() noexcept -> ref_storage_t
        {
            return m_data;
        }

        [[nodiscard]] constexpr auto storage() const noexcept -> ref_storage_t
        {
            return m_data;
        }

    public:
        template<size_type AxisN>
        constexpr auto clear() noexcept -> void
        {
            static_assert(AxisN < Axis);
            memset<value_type>(m_data[AxisN], 0, lengthOfAxisArray());
        }

        constexpr auto clear() noexcept -> void
        {
            constexprFor<0, Axis, 1>([&](auto i) { clear<i.value>(); });
        }

    public:
        template<u8 value>
        constexpr auto set(size_type index) noexcept -> void
        {
            PRIVATE::set<value, Axis, ref_storage_t>(m_data, index);
        }

        template<u8 value, size_type AxisN>
        constexpr auto set(size_type index) noexcept -> void
        {
            static_assert(AxisN < Axis);
            PRIVATE::set<value, pointer>(m_data[AxisN], index);
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool
        {
            static_assert(AxisN < Axis);
            return PRIVATE::isEmpty<const_ref_storage_t>(m_data[AxisN], size());
        }

        [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool
        {
            return PRIVATE::isEmpty<const_ref_storage_t, Axis>(m_data, size());
        }

        template<BitMapRule... Values>
        [[nodiscard]] constexpr auto find() const noexcept -> size_type
        {
            return PRIVATE::find_if<Values...>(size(), m_data.begin());
        }

        template<int AxisN, BitMapRule Value>
        [[nodiscard]] constexpr auto axis_find() const noexcept -> size_type
        {
            static_assert(AxisN < Axis);
            return PRIVATE::find_if<Value>(size(), m_data.begin() + AxisN);
        }

        template<BitMapRule... Values>
        [[nodiscard]] constexpr auto find(size_type times) const noexcept
            -> size_type
        {
            return PRIVATE::find_if<Values...>(size(), times, m_data.begin());
        }

        template<size_type AxisN, BitMapRule Value>
        [[nodiscard]] constexpr auto find_on_axis(size_type times) const noexcept
            -> size_type
        {
            static_assert(AxisN < Axis);
            return PRIVATE::find_if<Value>(size(), times, m_data.begin() + AxisN);
        }

        template<BitMapRule... Values>
        [[nodiscard]] constexpr auto
            is_set(size_type index, size_type times) const noexcept -> bool
        {
            return PRIVATE::is_set<Values...>(index, times, m_data.begin());
        }

        template<size_type AxisN, BitMapRule Values>
        [[nodiscard]] constexpr auto
            is_set_on_axis(size_type index, size_type times) const noexcept -> bool
        {
            static_assert(AxisN < Axis);
            return PRIVATE::is_set<Values>(index, times, m_data.begin() + AxisN);
        }

        [[nodiscard]] constexpr auto at(size_t axisN, size_t index) noexcept -> u8
        {
            return PRIVATE::atAxis<pointer>(axisN, index, m_data[axisN]);
        }

        template<size_type AxisN>
        [[nodiscard]] constexpr auto at(size_t index) noexcept -> u8
        {
            static_assert(AxisN < Axis);
            return PRIVATE::atAxis<pointer>(index, m_data[AxisN]);
        }

    public:
        template<size_type OAxis, bool OFreestanding>
        constexpr auto operator=(const Bitmap<OAxis, OFreestanding> &other) noexcept(
            Freestanding) -> Bitmap &
        {
            static_assert(!Freestanding);

            if (other.lengthOfAxisArray() > lengthOfAxisArray()) {
                [[unlikely]];

                m_size = other.m_size;
                auto *newBuffer =
                    new value_type[other.lengthOfAxisArray() * axis()]();

                delete[] m_data[0];

                CERBLIB_UNROLL_N(1)
                for (size_t i = 0; i < axis(); ++i) {
                    m_data[i] = newBuffer;
                    memcpy<value_type>(
                        m_data[i], other.m_data[i], lengthOfAxisArray());
                    newBuffer += lengthOfAxisArray();
                }
            } else {
                [[likely]];

                m_size = other.m_size;
                memcpy<value_type>(
                    m_data[0], other.m_data[0], lengthOfAxisArray() * axis());
            }

            return *this;
        }

        constexpr auto operator=(Bitmap &&other) noexcept -> Bitmap &
        {
            memcpy(m_data, other.m_data, axis());
            memset<pointer>(other.m_data, nullptr, axis());
            m_size       = other.m_size;
            other.m_size = 0;
        }

    public:
        constexpr Bitmap(Bitmap &&other) noexcept : m_size(other.m_size)
        {
            memcpy(m_data, other.m_data, axis());
            memset<pointer>(other.m_data, nullptr, axis());
            other.m_size = 0;
        }

        constexpr Bitmap(const Bitmap &other) : m_size(other.m_size)
        {
            static_assert(!Freestanding);
            auto *newBuffer = new value_type[other.lengthOfAxisArray() * axis()]();

            CERBLIB_UNROLL_N(1)
            for (size_t i = 0; i < axis(); ++i) {
                m_data[i] = newBuffer;
                memcpy<value_type>(m_data[i], other.m_data[i], lengthOfAxisArray());
                newBuffer += lengthOfAxisArray();
            }
        }

        constexpr explicit Bitmap(size_t size) noexcept : m_size(size)
        {
            static_assert(!Freestanding);
            auto *memory = new value_type[lengthOfAxisArray() * axis()]();

            CERBLIB_UNROLL_N(1)
            for (size_t i = 0; i < axis(); ++i) {
                m_data[i] = memory;
                memory += lengthOfAxisArray();
            }
        }

        constexpr explicit Bitmap(const_pointer ptr, size_t size) noexcept
          : m_size(size)
        {
            static_assert(Freestanding);

            CERBLIB_UNROLL_N(1)
            for (size_t i = 0; i < axis(); ++i) {
                m_data[i] = ptr;
                ptr += lengthOfAxisArray();
            }

            memset<value_type>(m_data[0], 0, lengthOfAxisArray() * axis());
        }

        template<typename... Ts>
        constexpr explicit Bitmap(size_t size, Ts &&...args) noexcept : m_size(size)
        {
            static_assert(Freestanding && sizeof...(Ts) == Axis);
            size_t i = 0;

            forEach(
                [&](auto elem) {
                    m_data[i] = elem;
                    memset<value_type>(m_data[i], 0, lengthOfAxisArray());
                    ++i;
                },
                args...);
        }

        constexpr ~Bitmap() noexcept(Freestanding)
        {
            if constexpr (!Freestanding) {
                delete[] m_data[0];
            }
        }
    };
}// namespace cerb

#endif /* CERBERUS_BITMAP_HPP */
