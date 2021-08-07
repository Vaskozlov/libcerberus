#ifndef CERBERUS_BITMAP_HPP
#define CERBERUS_BITMAP_HPP

#include <cerberus/string.hpp>
#include <cerberus/private/bitmapBase.hpp>

namespace cerb {

    template<size_t Size>
    struct constBitMap {
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

    public:
        storage_t m_data = {0};

    public:
        [[nodiscard]] constexpr
        auto size() const -> size_t {
            return Size;
        }

        [[nodiscard]] static constexpr
        auto sizeOfArray() -> size_t {
            return array_size;
        }

        [[nodiscard]] constexpr
        auto sizeOfData() const {
            return sizeOfArray() * sizeof(value_type);
        }

        [[nodiscard]] constexpr
        auto data() noexcept -> pointer {
            return m_data.data();
        }

    public:
        constexpr auto clear() noexcept -> void {
            cerb::memset(m_data, 0, sizeOfArray());
        }

        template<u8 value>
        constexpr auto set(size_t index) noexcept -> void {
            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            if constexpr (value) {
                m_data[elemIndex] |= (static_cast<value_type>(1) << bitIndex);
            } else {
                m_data[elemIndex] &= ~(static_cast<value_type>(1) << bitIndex);
            }
        }

        template<u8 Value>
        constexpr auto set(size_t index, size_t times) noexcept -> void {
            if (std::is_constant_evaluated()) {
                cerb::PRIVATE::bitmap_set<Value, ref_storage_t>(m_data, index, times);
            }
            else {
                cerb::PRIVATE::bitmap_set<Value, pointer>(m_data.data(), index, times);
            }
        }

        [[nodiscard]] constexpr
        auto isEmpty() const noexcept -> bool {
            if (std::is_constant_evaluated()) {
                return PRIVATE::isEmpty<const_ref_storage_t>(m_data, size());
            } else {
                return PRIVATE::isEmpty<const_pointer>(m_data.data(), size());
            }
        }

    public:
        [[nodiscard]] constexpr
        auto at(size_t index) const noexcept -> u8 {
            if (std::is_constant_evaluated()) {
                return PRIVATE::at<const_ref_storage_t>(m_data, index);
            }
            else {
                return PRIVATE::at<const_pointer>(m_data.data(), index);
            }
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) noexcept -> cerb::PRIVATE::BitmapElem<value_type> {
            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            return cerb::PRIVATE::BitmapElem<value_type>(bitIndex, &m_data[elemIndex]);
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) const noexcept -> cerb::PRIVATE::BitmapElem<value_type> {
            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            return {bitIndex, &m_data[elemIndex]};
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if() const noexcept -> size_t {
            if (std::is_constant_evaluated()) {
                return PRIVATE::bitmap_find_if<firstValue, const_ref_storage_t>(m_data, size());
            } else {
                return PRIVATE::bitmap_find_if<firstValue, const_pointer>(m_data.data(), size());
            }
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if(size_t times) const noexcept -> size_t {
            if (std::is_constant_evaluated()) {
                return PRIVATE::bitmap_find_if<firstValue, const_ref_storage_t>(m_data, times, size());
            } else {
                return PRIVATE::bitmap_find_if<firstValue, const_pointer>(m_data.data(), times, size());
            }
        }

        template<u8 Value> [[nodiscard]] constexpr
        auto is_value_set(size_t index, size_t times) -> bool {
            if (std::is_constant_evaluated()) {
                return PRIVATE::bitmap_is_set<Value, const_ref_storage_t>(m_data, index, times);
            } else {
                return PRIVATE::bitmap_is_set<Value, const_pointer>(m_data.data(), index, times);
            }
        }

    public:
        auto operator=(constBitMap&& other) noexcept -> constBitMap& {
            cerb::memcpy(m_data, other.m_data, m_data.size());
            return *this;
        }

        constexpr auto operator=(const constBitMap& other) noexcept -> constBitMap& {
            cerb::memcpy(m_data, other.m_data, m_data.size());
            return *this;
        }

    public:
        constexpr constBitMap() noexcept = default;
        constexpr ~constBitMap() noexcept = default;

        constexpr constBitMap(const constBitMap& other) noexcept {
            cerb::memcpy(m_data, other.m_data, sizeOfArray());
        }

        constexpr constBitMap(constBitMap&& other) noexcept {
            cerb::memcpy(m_data, other.m_data, sizeOfArray());
        }
    };

    template<bool Freestanding = false>
    struct bitmap {
        using size_type             = size_t;
        using value_type            = PRIVATE::bitmap_value_type;
        using const_value_type      = const value_type;
        using pointer               = value_type *;
        using const_pointer         = const value_type *;

    private:
        pointer m_data;
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
        auto data() noexcept -> pointer {
            return m_data;
        }

    public:
        constexpr auto clear() noexcept -> void {
            cerb::memset(m_data, 0, sizeOfArray());
        }

        template<u8 value>
        constexpr auto set(size_t index) noexcept -> void {
            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            if constexpr (value) {
                m_data[elemIndex] |= (value << static_cast<value_type>(bitIndex));
            } else {
                m_data[elemIndex] &= ~(1 << static_cast<value_type>(bitIndex));
            }
        }

        [[nodiscard]] constexpr
        auto isEmpty() const noexcept -> bool {
            return PRIVATE::isEmpty<const_pointer>(m_data, size());
        }

        constexpr auto resize(size_t size) -> void {
            auto newArraySize = size / bitsizeof(value_type) + (size % bitsizeof(value_type)) != 0;

            if (sizeOfArray() < newArraySize) {
                pointer newData;

                if constexpr (!Freestanding) {
                    newData = new value_type[newArraySize];
                    cerb::memcpy(newData, m_data, sizeOfArray());
                } else {
                    newData = m_data;
                }

                cerb::memset<value_type>(newData + sizeOfArray(), 0, newArraySize - sizeOfArray());
            }

            m_size = size;
        }

    public:
        [[nodiscard]] constexpr
        auto at(size_t index) const noexcept -> u8 {
            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);
            return (m_data[elemIndex] & (static_cast<value_type>(1) << bitIndex)) != 0;
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) noexcept -> cerb::PRIVATE::BitmapElem<value_type> {
            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            return {static_cast<u16>(bitIndex), &m_data[elemIndex]};
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) const noexcept -> cerb::PRIVATE::BitmapElem<value_type> {
            auto elemIndex = index / bitsizeof(value_type);
            auto bitIndex  = index % bitsizeof(value_type);

            return {static_cast<u16>(bitIndex), &m_data[elemIndex]};
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if() const noexcept -> size_t {
            return PRIVATE::bitmap_find_if<firstValue, const_pointer>(m_data, size());
        }

    public:
        constexpr auto operator=(const bitmap& other) noexcept -> bitmap& {
            static_assert(!Freestanding);

            if (sizeOfArray() < other.sizeOfArray()) {
                delete[] m_data;
                m_data = new value_type[other.sizeOfArray()];
            }

            m_size = other.size();
            cerb::memcpy(m_data, other.m_data, sizeOfArray());
            return *this;
        }

        constexpr auto operator=(bitmap&& other) noexcept -> bitmap&
        {
            if constexpr (!Freestanding) {
                delete[] m_data;
            }

            m_data = other.m_data;
            m_size = other.m_size;

            other.m_size = 0;
            other.m_data = nullptr;

            return *this;
        }

    public: // bitmap do NOT work in constexpr context :/ but just in case everything is constexpr

        CERBLIB_DISABLE_WARNING(constant-evaluated, constant-evaluated, 0)
        constexpr bitmap(bitmap& other)
        : m_data(new value_type[other.sizeOfArray()]), m_size(other.size())
        {
            static_assert(!Freestanding);
            cerb::memcpy<value_type>(m_data, other.m_data, sizeOfArray());
        }
        CERBLIB_ENABLE_WARNING(constant-evaluated, constant-evaluated, 0)

        constexpr bitmap(bitmap&& other) noexcept
        : m_data(other.m_data), m_size(other.size())
        {
            other.m_size = 0;
            other.m_data = nullptr;
        }

        explicit constexpr bitmap(size_t size)
        : m_data(new value_type[size / bitsizeof(value_type) + (size % bitsizeof(value_type)) != 0]), m_size(size)
        {
            static_assert(!Freestanding);
        }

        explicit constexpr bitmap(value_type *data, size_t size)
        : m_data(data), m_size(size)
        {
            static_assert(Freestanding);
        }

        constexpr ~bitmap() noexcept(Freestanding)
        {
            if constexpr (!Freestanding) {
                delete[] m_data;
            }
        }
    };
}

#endif /* CERBERUS_BITMAP_HPP */
