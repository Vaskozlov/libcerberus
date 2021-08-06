#ifndef CERBERUS_DOUBLEBITMAP_HPP
#define CERBERUS_DOUBLEBITMAP_HPP

#include <cerberus/types.h>
#include <cerberus/private/doubleBitmapBase.hpp>

namespace cerb {
    template<typename T, size_t Size>
    class ConstDoubleBitmap {
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);

    public:
        [[nodiscard]] constexpr
        auto size() const -> size_t {
            return Size;
        }

        [[nodiscard]] static constexpr
        auto sizeOfArray() -> size_t {
            return Size / bitsizeof(T) + (Size % bitsizeof(T) > 0);
        }

        [[nodiscard]] constexpr
        auto sizeOfData() const {
            return sizeOfArray() * sizeof(T);
        }

        [[nodiscard]] constexpr
        auto data1() noexcept -> T * {
            return m_data1.data();
        }

        [[nodiscard]] constexpr
        auto data2() noexcept -> T * {
            return m_data1.data();
        }

    public:
        using size_type             = size_t;
        using value_type            = T;
        using const_value_type      = const T;
        using pointer               = T *;
        using const_pointer         = const T *;

        using storage_t             = std::array<T, sizeOfArray()>;
        using ref_storage_t         = std::array<T, sizeOfArray()>&;
        using const_storage_t       = const std::array<T, sizeOfArray()>;
        using const_ref_storage_t   = const std::array<T, sizeOfArray()>&;

    private:
        storage_t m_data1 = {0};
        storage_t m_data2 = {0};

    public:

        CERBLIB_DISABLE_WARNING(constant-evaluated,constant-evaluated,0)
        constexpr auto clear1() noexcept -> void {
            if (std::is_constant_evaluated() || !cerb::x86_64) {
                CERBLIB_UNROLL_N(4)
                for (auto &elem: m_data1) {
                    elem = 0;
                }
            } else {
                cerb::memset<value_type>(m_data1.data(), 0, sizeOfArray());
            }
        }

        constexpr auto clear2() noexcept -> void {
            if (std::is_constant_evaluated() || !cerb::x86_64) {
                CERBLIB_UNROLL_N(4)
                for (auto &elem: m_data2) {
                    elem = 0;
                }
            } else {
                cerb::memset<value_type>(m_data2.data(), 0, sizeOfArray());
            }
        }
        CERBLIB_ENABLE_WARNING(constant-evaluated,constant-evaluated,0)

        constexpr auto clear() noexcept -> void {
            this->clear1();
            this->clear2();
        }

        template<u8 value>
        constexpr auto set1(size_t index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            if constexpr (value) {
                m_data1[elemIndex] |= (value << static_cast<T>(bitIndex));
            } else {
                m_data1[elemIndex] &= ~(1 << static_cast<T>(bitIndex));
            }
        }

        template<u8 value>
        constexpr auto set2(size_t index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            if constexpr (value) {
                m_data2[elemIndex] |= (value << static_cast<T>(bitIndex));
            } else {
                m_data2[elemIndex] &= ~(1 << static_cast<T>(bitIndex));
            }
        }

        template<u8 value>
        constexpr auto set(size_t index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            if constexpr (value) {
                m_data1[elemIndex] |= (static_cast<T>(value) << bitIndex);
                m_data2[elemIndex] |= (static_cast<T>(value) << bitIndex);
            } else {
                m_data1[elemIndex] |= (static_cast<T>(value) << bitIndex);
                m_data2[elemIndex] &= ~(static_cast<T>(1) << bitIndex);
            }
        }

        [[nodiscard]] constexpr
        auto isEmpty1() const noexcept -> bool {
            return cerb::PRIVATE::isEmpty<const_ref_storage_t>(m_data1, size());
        }

        [[nodiscard]] constexpr
        auto isEmpty2() const noexcept -> bool {
            return cerb::PRIVATE::isEmpty<const_ref_storage_t>(m_data2, size());
        }

        [[nodiscard]] constexpr
        auto isEmpty() const noexcept -> bool {
            return isEmpty1() && isEmpty2();
        }

    public:
        [[nodiscard]] constexpr
        auto at1(size_t index) const noexcept -> u8 {
            return cerb::PRIVATE::at(m_data1, index);
        }

        [[nodiscard]] constexpr
        auto at2(size_t index) const noexcept -> u8 {
            return cerb::PRIVATE::at(m_data2, index);
        }

        [[nodiscard]] constexpr
        auto at(size_t index) const noexcept -> cerb::PRIVATE::TwoBitStorage {
            return cerb::PRIVATE::TwoBitStorage(at1(index), at2(index));
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) noexcept -> cerb::PRIVATE::DoubleBitmapElem<T> {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            return cerb::PRIVATE::DoubleBitmapElem<T>(bitIndex, &m_data1[elemIndex], &m_data2[elemIndex]);
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) const noexcept -> cerb::PRIVATE::BitmapElem<T> {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            return cerb::PRIVATE::DoubleBitmapElem<T>(bitIndex, &m_data1[elemIndex], &m_data2[elemIndex]);
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if1() const noexcept -> size_t {
            return cerb::PRIVATE::bitmap_find_if<firstValue, const_ref_storage_t>(m_data1, size());
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if2() const noexcept -> size_t {
            return cerb::PRIVATE::bitmap_find_if<firstValue, const_ref_storage_t>(m_data2, size());
        }

        template<u8 firstValue, u8 SecondValue> [[nodiscard]] constexpr
        auto find_if() const noexcept -> size_t {
            return cerb::PRIVATE::bitmap_find_if<firstValue, SecondValue, const_ref_storage_t>(m_data1, m_data2, size());
        }

        template<u8 firstValue, u8 SecondValue> [[nodiscard]] constexpr
        auto find_if(size_t times) const noexcept -> size_t {
            return cerb::PRIVATE::bitmap_find_if<firstValue, SecondValue, const_ref_storage_t>(m_data1, m_data2, times, size());
        }

    public:
        auto operator=(ConstDoubleBitmap&& other) noexcept -> ConstDoubleBitmap& {
            cerb::memcpy(m_data1, other.m_data1, m_data1.size());
            cerb::memcpy(m_data2, other.m_data2, m_data2.size());
        }

        auto operator=(const ConstDoubleBitmap& other) noexcept -> ConstDoubleBitmap& {
            cerb::memcpy(m_data1, other.m_data1, m_data1.size());
            cerb::memcpy(m_data2, other.m_data2, m_data2.size());
        }

    public:
        ConstDoubleBitmap() noexcept = default;
        ~ConstDoubleBitmap() noexcept = default;

        constexpr ConstDoubleBitmap(const ConstDoubleBitmap& other) noexcept
        {
            cerb::memcpy(m_data1, other.m_data1, m_data1.size());
            cerb::memcpy(m_data2, other.m_data2, m_data2.size());
        }

        constexpr ConstDoubleBitmap(ConstDoubleBitmap&& other) noexcept
        {
            cerb::memcpy(m_data1, other.m_data1, m_data1.size());
            cerb::memcpy(m_data2, other.m_data2, m_data2.size());
        }
    };

    template<typename T, bool Freestanding = false>
    class DoubleBitmap {
    public:
        using size_type             = size_t;
        using value_type            = T;
        using const_value_type      = const T;
        using pointer               = T *;
        using const_pointer         = const T *;

    private:
        pointer m_data1;
        pointer m_data2;
        size_type m_size;

    public:
        [[nodiscard]] constexpr
        auto size() const noexcept -> size_t {
            return m_size;
        }

        [[nodiscard]] constexpr
        auto sizeOfArray() const noexcept -> size_t {
            return m_size / bitsizeof(T) + (m_size % bitsizeof(T) > 0);
        }

        [[nodiscard]] constexpr
        auto sizeOfData() const noexcept {
            return sizeOfArray() * sizeof(T);
        }

        [[nodiscard]] constexpr
        auto data1() noexcept -> T * {
            return m_data1;
        }

        [[nodiscard]] constexpr
        auto data2() noexcept -> T * {
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
        constexpr auto set1(size_t index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            if constexpr (value) {
                m_data1[elemIndex] |= (value << static_cast<T>(bitIndex));
            } else {
                m_data1[elemIndex] &= ~(1 << static_cast<T>(bitIndex));
            }
        }

        template<u8 value>
        constexpr auto set2(size_t index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            if constexpr (value) {
                m_data2[elemIndex] |= (value << static_cast<T>(bitIndex));
            } else {
                m_data2[elemIndex] &= ~(1 << static_cast<T>(bitIndex));
            }
        }

        template<u8 value>
        constexpr auto set(size_t index) noexcept -> void {
            static_assert(value < 2);

            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            if constexpr (value) {
                m_data1[elemIndex] |= (static_cast<T>(value) << bitIndex);
                m_data2[elemIndex] |= (static_cast<T>(value) << bitIndex);
            } else {
                m_data1[elemIndex] |= (static_cast<T>(value) << bitIndex);
                m_data2[elemIndex] &= ~(static_cast<T>(1) << bitIndex);
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
        auto at1(size_t index) const noexcept -> u8 {
            return cerb::PRIVATE::at(m_data1, index);
        }

        [[nodiscard]] constexpr
        auto at2(size_t index) const noexcept -> u8 {
            return cerb::PRIVATE::at(m_data2, index);
        }

        [[nodiscard]] constexpr
        auto at(size_t index) const noexcept -> cerb::PRIVATE::TwoBitStorage {
            return cerb::PRIVATE::TwoBitStorage(at1(index), at2(index));
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) noexcept -> cerb::PRIVATE::DoubleBitmapElem<T> {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            return cerb::PRIVATE::DoubleBitmapElem<value_type>(bitIndex, &m_data1[elemIndex], &m_data2[elemIndex]);
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) const noexcept -> cerb::PRIVATE::BitmapElem<T> {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            return cerb::PRIVATE::DoubleBitmapElem<value_type>(bitIndex, &m_data1[elemIndex], &m_data2[elemIndex]);
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if1() const noexcept -> size_t {
            return cerb::PRIVATE::bitmap_find_if<firstValue, const_pointer>(m_data1, size());
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if2() const noexcept -> size_t {
            return cerb::PRIVATE::bitmap_find_if<firstValue, const_pointer>(m_data2, size());
        }

        template<u8 firstValue, u8 SecondValue> [[nodiscard]] constexpr
        auto find_if() const noexcept -> size_t {
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

            m_data1 = new T[other.sizeOfArray()];
            m_data2 = new T[other.sizeOfArray()];
            m_size = other.size();

            if (std::is_constant_evaluated() || !cerb::x86_64) {
                CERBLIB_UNROLL_N(4)
                for (size_t i = 0; i < sizeOfArray(); ++i) {
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
        : m_data1(new T[other.sizeOfArray()]),
          m_data2(new T[other.sizeOfArray()]),
          m_size(other.size())
        {
            static_assert(!Freestanding);

            if (std::is_constant_evaluated() || !cerb::x86_64) {
                CERBLIB_UNROLL_N(4)
                for (size_t i = 0; i < sizeOfArray(); ++i) {
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

        explicit constexpr DoubleBitmap(size_t size) noexcept
        : m_data1(new T[size / bitsizeof(T) + (size % bitsizeof(T)) != 0]),
          m_data2(new T[size / bitsizeof(T) + (size % bitsizeof(T)) != 0]),
          m_size(size)
        {
            static_assert(!Freestanding);
        }

        explicit constexpr DoubleBitmap(T *data, size_t size) noexcept
        : m_data1(data),
          m_data2(data + (size / bitsizeof(T) + (size % bitsizeof(T)) != 0)),
          m_size(size)
        {
            static_assert(Freestanding);
        }

        explicit constexpr DoubleBitmap(T *data1, T *data2, size_t size) noexcept
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
