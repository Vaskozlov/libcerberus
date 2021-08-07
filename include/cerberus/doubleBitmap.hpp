#ifndef CERBERUS_DOUBLEBITMAP_HPP
#define CERBERUS_DOUBLEBITMAP_HPP

#include <cerberus/types.h>
#include <cerberus/private/doubleBitmapBase.hpp>

namespace cerb {
    template<size_t Size>
    struct constDoubleBitmap {
        using size_type             = size_t;
        using value_type            = PRIVATE::bitmap_value_type;
        using const_value_type      = const value_type;
        using pointer               = value_type *;
        using const_pointer         = const value_type *;

    private:
        constexpr static size_type array_size = Size / bitsizeof(value_type) + (Size % bitsizeof(value_type) > 0);

    public:
        using storage_t             = std::array<value_type, array_size>;
        using ref_storage_t         = std::array<value_type, array_size>&;
        using const_storage_t       = const std::array<value_type, array_size>;
        using const_ref_storage_t   = const std::array<value_type, array_size>&;

    private:
        storage_t m_data1 = {0};
        storage_t m_data2 = {0};

    public:
        [[nodiscard]] constexpr
        auto size() const -> size_type {
            return Size;
        }

        [[nodiscard]] static constexpr
        auto sizeOfArray() -> size_type {
            return array_size;
        }

        [[nodiscard]] constexpr
        auto sizeOfData() const {
            return sizeOfArray() * sizeof(value_type);
        }

        [[nodiscard]] constexpr
        auto data1() noexcept -> pointer {
            return m_data1.data();
        }

        [[nodiscard]] constexpr
        auto data2() noexcept -> pointer {
            return m_data1.data();
        }

    public:

        CERBLIB_DISABLE_WARNING(constant-evaluated,constant-evaluated,0)
        constexpr auto clear1() noexcept -> void {
            cerb::memset<value_type>(m_data1.data(), 0, sizeOfArray());
        }

        constexpr auto clear2() noexcept -> void {
            cerb::memset<value_type>(m_data2.data(), 0, sizeOfArray());
        }
        CERBLIB_ENABLE_WARNING(constant-evaluated,constant-evaluated,0)

        constexpr auto clear() noexcept -> void {
            this->clear1();
            this->clear2();
        }

        template<u8 value>
        constexpr auto set1(size_type index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            if constexpr (value) {
                m_data1[elemIndex] |= (value << static_cast<value_type>(bitIndex));
            } else {
                m_data1[elemIndex] &= ~(1 << static_cast<value_type>(bitIndex));
            }
        }

        template<u8 value>
        constexpr auto set2(size_type index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            if constexpr (value) {
                m_data2[elemIndex] |= (value << static_cast<value_type>(bitIndex));
            } else {
                m_data2[elemIndex] &= ~(1 << static_cast<value_type>(bitIndex));
            }
        }

        template<u8 value>
        constexpr auto set(size_type index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            if constexpr (value) {
                m_data1[elemIndex] |= (static_cast<value_type>(value) << bitIndex);
                m_data2[elemIndex] |= (static_cast<value_type>(value) << bitIndex);
            } else {
                m_data1[elemIndex] |= (static_cast<value_type>(value) << bitIndex);
                m_data2[elemIndex] &= ~(static_cast<value_type>(1) << bitIndex);
            }
        }

        [[nodiscard]] constexpr
        auto isEmpty1() const noexcept -> bool {
            if (std::is_constant_evaluated()) {
                return cerb::PRIVATE::isEmpty<const_ref_storage_t>(m_data1, size());
            } else {
                return PRIVATE::isEmpty<const_pointer>(m_data1.data(), size());
            }
        }

        [[nodiscard]] constexpr
        auto isEmpty2() const noexcept -> bool {
            if (std::is_constant_evaluated()) {
                return cerb::PRIVATE::isEmpty<const_ref_storage_t>(m_data2, size());
            } else {
                return PRIVATE::isEmpty<const_pointer>(m_data2.data(), size());
            }
        }

        [[nodiscard]] constexpr
        auto isEmpty() const noexcept -> bool {
            return isEmpty1() && isEmpty2();
        }

    public:
        [[nodiscard]] constexpr
        auto at1(size_type index) const noexcept -> u8 {
            if (std::is_constant_evaluated()) {
                return PRIVATE::at<const_ref_storage_t>(m_data1, index);
            } else {
                return PRIVATE::at<const_pointer>(m_data1.data(), index);
            }
        }

        [[nodiscard]] constexpr
        auto at2(size_type index) const noexcept -> u8 {
            if (std::is_constant_evaluated()) {
                return PRIVATE::at<const_ref_storage_t>(m_data1, index);
            } else {
                return PRIVATE::at<const_pointer>(m_data1.data(), index);
            }
        }

        [[nodiscard]] constexpr
        auto at(size_type index) const noexcept -> cerb::PRIVATE::TwoBitStorage {
            return {at1(index), at2(index)};
        }

        [[nodiscard]] constexpr
        auto operator[](size_type index) noexcept -> cerb::PRIVATE::DoubleBitmapElem<value_type> {
            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            return {bitIndex, &m_data1[elemIndex], &m_data2[elemIndex]};
        }

        [[nodiscard]] constexpr
        auto operator[](size_type index) const noexcept -> cerb::PRIVATE::DoubleBitmapElem<value_type> {
            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            return {bitIndex, &m_data1[elemIndex], &m_data2[elemIndex]};
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if1() const noexcept -> size_type {
            if (std::is_constant_evaluated()) {
                return cerb::PRIVATE::bitmap_find_if<firstValue, const_ref_storage_t>(m_data1, size());
            } else {
                return cerb::PRIVATE::bitmap_find_if<firstValue, const_pointer>(m_data1, size());
            }
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if2() const noexcept -> size_type {
            if (std::is_constant_evaluated()) {
                return cerb::PRIVATE::bitmap_find_if<firstValue, const_ref_storage_t>(m_data2, size());
            } else {
                return cerb::PRIVATE::bitmap_find_if<firstValue, const_pointer>(m_data2, size());
            }
        }

        template<u8 firstValue, u8 SecondValue> [[nodiscard]] constexpr
        auto find_if() const noexcept -> size_type {
            if (std::is_constant_evaluated()) {
                return cerb::PRIVATE::bitmap_find_if<firstValue, SecondValue, const_ref_storage_t>(m_data1, m_data2, size());
            } else {
                return cerb::PRIVATE::bitmap_find_if<firstValue, SecondValue, const_pointer>(m_data1, m_data2, size());
            }
        }

        template<u8 firstValue, u8 SecondValue> [[nodiscard]] constexpr
        auto find_if(size_type times) const noexcept -> size_type {
            if (std::is_constant_evaluated()) {
                return cerb::PRIVATE::bitmap_find_if<firstValue, SecondValue, const_ref_storage_t>(m_data1, m_data2, times, size());
            } else {
                return cerb::PRIVATE::bitmap_find_if<firstValue, SecondValue, const_pointer>(m_data1, m_data2, times, size());
            }
        }

    public:
        auto operator=(constDoubleBitmap&& other) noexcept -> constDoubleBitmap& {
            cerb::memcpy(m_data1, other.m_data1, m_data1.size());
            cerb::memcpy(m_data2, other.m_data2, m_data2.size());
        }

        auto operator=(const constDoubleBitmap& other) noexcept -> constDoubleBitmap& {
            cerb::memcpy(m_data1, other.m_data1, m_data1.size());
            cerb::memcpy(m_data2, other.m_data2, m_data2.size());
        }

    public:
        constDoubleBitmap() noexcept = default;
        ~constDoubleBitmap() noexcept = default;

        constexpr constDoubleBitmap(const constDoubleBitmap& other) noexcept
        {
            cerb::memcpy(m_data1, other.m_data1, m_data1.size());
            cerb::memcpy(m_data2, other.m_data2, m_data2.size());
        }

        constexpr constDoubleBitmap(constDoubleBitmap&& other) noexcept
        {
            cerb::memcpy(m_data1, other.m_data1, m_data1.size());
            cerb::memcpy(m_data2, other.m_data2, m_data2.size());
        }
    };

    template<bool Freestanding = false>
    struct DoubleBitmap {
        using size_type             = size_t;
        using value_type            = PRIVATE::bitmap_value_type;
        using const_value_type      = const value_type;
        using pointer               = value_type *;
        using const_pointer         = const value_type *;

    private:
        pointer m_data1;
        pointer m_data2;
        size_type m_size;

    public:
        [[nodiscard]] constexpr
        auto size() const noexcept -> size_type {
            return m_size;
        }

        [[nodiscard]] constexpr
        auto sizeOfArray() const noexcept -> size_type {
            return m_size / bitsizeof(value_type) + (m_size % bitsizeof(value_type) > 0);
        }

        [[nodiscard]] constexpr
        auto sizeOfData() const noexcept {
            return sizeOfArray() * sizeof(value_type);
        }

        [[nodiscard]] constexpr
        auto data1() noexcept -> pointer {
            return m_data1;
        }

        [[nodiscard]] constexpr
        auto data2() noexcept -> pointer {
            return m_data2;
        }
    public:

        CERBLIB_DISABLE_WARNING(constant-evaluated,constant-evaluated,0)
        constexpr auto clear1() noexcept -> void {
            cerb::memset<value_type>(m_data1, 0, sizeOfArray());
        }

        constexpr auto clear2() noexcept -> void {
            cerb::memset<value_type>(m_data2, 0, sizeOfArray());
        }
        CERBLIB_ENABLE_WARNING(constant-evaluated,constant-evaluated,0)

        constexpr auto clear() noexcept -> void {
            this->clear1();
            this->clear2();
        }

        template<u8 value>
        constexpr auto set1(size_type index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            if constexpr (value) {
                m_data1[elemIndex] |= (value << static_cast<value_type>(bitIndex));
            } else {
                m_data1[elemIndex] &= ~(1 << static_cast<value_type>(bitIndex));
            }
        }

        template<u8 value>
        constexpr auto set2(size_type index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            if constexpr (value) {
                m_data2[elemIndex] |= (value << static_cast<value_type>(bitIndex));
            } else {
                m_data2[elemIndex] &= ~(1 << static_cast<value_type>(bitIndex));
            }
        }

        template<u8 value>
        constexpr auto set(size_type index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            if constexpr (value) {
                m_data1[elemIndex] |= (static_cast<value_type>(value) << bitIndex);
                m_data2[elemIndex] |= (static_cast<value_type>(value) << bitIndex);
            } else {
                m_data1[elemIndex] |= (static_cast<value_type>(value) << bitIndex);
                m_data2[elemIndex] &= ~(static_cast<value_type>(1) << bitIndex);
            }
        }

        [[nodiscard]] constexpr
        auto isEmpty1() const noexcept -> bool {
            return cerb::PRIVATE::isEmpty<const_pointer>(m_data1, size());
        }

        [[nodiscard]] constexpr
        auto isEmpty2() const noexcept -> bool {
            return cerb::PRIVATE::isEmpty<const_pointer>(m_data2, size());
        }

        [[nodiscard]] constexpr
        auto isEmpty() const noexcept -> bool {
            return isEmpty1() && isEmpty2();
        }

    public:
        [[nodiscard]] constexpr
        auto at1(size_type index) const noexcept -> u8 {
            return cerb::PRIVATE::at(m_data1, index);
        }

        [[nodiscard]] constexpr
        auto at2(size_type index) const noexcept -> u8 {
            return cerb::PRIVATE::at<const_pointer>(m_data2, index);
        }

        [[nodiscard]] constexpr
        auto at(size_type index) const noexcept -> cerb::PRIVATE::TwoBitStorage {
            return cerb::PRIVATE::TwoBitStorage(at1(index), at2(index));
        }

        [[nodiscard]] constexpr
        auto operator[](size_type index) noexcept -> cerb::PRIVATE::DoubleBitmapElem<value_type> {
            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            return {static_cast<u16>(bitIndex), &m_data1[elemIndex], &m_data2[elemIndex]};
        }

        [[nodiscard]] constexpr
        auto operator[](size_type index) const noexcept -> cerb::PRIVATE::DoubleBitmapElem<value_type> {
            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            return {static_cast<u16>(bitIndex), &m_data1[elemIndex], &m_data2[elemIndex]};
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if1() const noexcept -> size_type {
            return cerb::PRIVATE::bitmap_find_if<firstValue, const_pointer>(m_data1, size());
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if2() const noexcept -> size_type {
            return cerb::PRIVATE::bitmap_find_if<firstValue, const_pointer>(m_data2, size());
        }

        template<u8 firstValue, u8 SecondValue> [[nodiscard]] constexpr
        auto find_if() const noexcept -> size_type {
            return cerb::PRIVATE::bitmap_find_if<firstValue, SecondValue, const_pointer>(m_data1, m_data2, size());
        }

    public:
        constexpr auto operator=(DoubleBitmap&& other) noexcept -> DoubleBitmap& {
            m_data1 = other.m_data1;
            m_data2 = other.m_data2;
            m_size = other.m_size;

            other.m_size = 0;
            other.m_data1 = nullptr;
            other.m_data2 = nullptr;

            return *this;
        }

        CERBLIB_DISABLE_WARNING(constant-evaluated,constant-evaluated,0)
        constexpr auto operator=(const DoubleBitmap& other) noexcept -> DoubleBitmap&{
            static_assert(!Freestanding);

            m_data1 = new value_type[other.sizeOfArray()];
            m_data2 = new value_type[other.sizeOfArray()];
            m_size = other.size();

            if (std::is_constant_evaluated() || !cerb::x86_64) {
                CERBLIB_UNROLL_N(4)
                for (size_type i = 0; i < sizeOfArray(); ++i) {
                    m_data1[i] = other.m_data1[i];
                    m_data2[i] = other.m_data2[i];
                }
            } else {
                cerb::memcpy<value_type>(m_data1, other.m_data1, sizeOfArray());
                cerb::memcpy<value_type>(m_data2, other.m_data1, sizeOfArray());
            }

            return *this;
        }

    public:
        constexpr DoubleBitmap(DoubleBitmap& other) noexcept
        : m_data1(new value_type[other.sizeOfArray()]),
          m_data2(new value_type[other.sizeOfArray()]),
          m_size(other.size())
        {
            static_assert(!Freestanding);

            if (std::is_constant_evaluated() || !cerb::x86_64) {
                CERBLIB_UNROLL_N(4)
                for (size_type i = 0; i < sizeOfArray(); ++i) {
                    m_data1[i] = other.m_data1[i];
                    m_data2[i] = other.m_data2[i];
                }
            } else {
                cerb::memcpy<value_type>(m_data1, other.m_data1, sizeOfArray());
                cerb::memcpy<value_type>(m_data2, other.m_data1, sizeOfArray());
            }
        }

        CERBLIB_ENABLE_WARNING(constant-evaluated,constant-evaluated,0)

        constexpr DoubleBitmap(DoubleBitmap &&other) noexcept
        : m_data1(other.m_data1),
          m_data2(other.m_data2),
          m_size(other.m_size)
        {
            other.m_size = 0;
            other.m_data1 = nullptr;
            other.m_data2 = nullptr;
        }

        explicit constexpr DoubleBitmap(size_type size) noexcept
        : m_data1(new value_type[size / bitsizeof(value_type) + (size % bitsizeof(value_type)) != 0]),
          m_data2(new value_type[size / bitsizeof(value_type) + (size % bitsizeof(value_type)) != 0]),
          m_size(size)
        {
            static_assert(!Freestanding);
        }

        explicit constexpr DoubleBitmap(value_type *data, size_type size) noexcept
        : m_data1(data),
          m_data2(data + (size / bitsizeof(value_type) + (size % bitsizeof(value_type)) != 0)),
          m_size(size)
        {
            static_assert(Freestanding);
        }

        explicit constexpr DoubleBitmap(value_type *data1, value_type *data2, size_type size) noexcept
        : m_data1(data1),
          m_data2(data2),
          m_size(size)
        {
            static_assert(Freestanding);
        }

        CERBLIB20_CONSTEXPR ~DoubleBitmap() noexcept (Freestanding)
        {
            if constexpr (!Freestanding) {
                delete[] m_data1;
                delete[] m_data2;
            }
        }
    };
}

#endif /* CERBERUS_DOUBLEBITMAP_HPP */
