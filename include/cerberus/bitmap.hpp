#ifndef bitmap_hpp
#define bitmap_hpp

#include <cerberus/string.hpp>
#include <cerberus/private/bitmapBase.hpp>

namespace cerb {

    template<typename T, size_t Size>
    class ConstBitMap {
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
        auto data() noexcept -> T * {
            return m_data.data();
        }

    private:
        std::array<T, sizeOfArray()> m_data = {0};

    public:
        constexpr auto clear() noexcept -> void {
            CERBLIB_UNROLL_N(4)
            for (auto &elem: m_data) {
                elem = 0;
            }
        }

        template<u8 value>
        constexpr auto set(size_t index) noexcept -> void {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            if constexpr (value) {
                m_data[elemIndex] |= (static_cast<T>(1) << bitIndex);
            } else {
                m_data[elemIndex] &= ~(static_cast<T>(1) << bitIndex);
            }
        }

        [[nodiscard]] constexpr
        auto isEmpty() const noexcept -> bool {
            if (std::is_constant_evaluated()) {
                return PRIVATE::isEmpty<const std::array<T, sizeOfArray()>&>(m_data, size());
            } else {
                return PRIVATE::isEmpty<const T*>(m_data.data(), size());
            }
        }

    public:
        [[nodiscard]] constexpr
        auto at(size_t index) const noexcept -> u8 {
            return PRIVATE::at(m_data, index);
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) noexcept -> cerb::PRIVATE::BitmapElem<T> {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            return cerb::PRIVATE::BitmapElem<T>(bitIndex, &m_data[elemIndex]);
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) const noexcept -> cerb::PRIVATE::BitmapElem<T> {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            return cerb::PRIVATE::BitmapElem<T>(bitIndex, &m_data[elemIndex]);
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if() const noexcept -> size_t {
            return PRIVATE::bitmap_find_if<firstValue, const std::array<T, sizeOfArray()>&>(m_data, size());
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if(size_t times) const noexcept -> size_t {
            return PRIVATE::bitmap_find_if<firstValue, const std::array<T, sizeOfArray()>&>(m_data, times, size());
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if2(size_t times) const noexcept -> size_t {
            return PRIVATE::bitmap_find_if2<firstValue, const std::array<T, sizeOfArray()>&>(m_data, times, size());
        }

        template<u8 Value> [[nodiscard]] constexpr
        auto is_value_set(size_t index, size_t times) -> bool {
            return PRIVATE::bitmap_is_set<Value, const std::array<T, sizeOfArray()>&, T>(m_data, index, times);
        }

    public:
        auto operator=(ConstBitMap&& other) noexcept -> ConstBitMap& {
            cerb::memcpy(m_data, other.m_data, m_data.size());
            return *this;
        }

        constexpr auto operator=(const ConstBitMap& other) noexcept -> ConstBitMap& {
            cerb::memcpy(m_data, other.m_data, m_data.size());
            return *this;
        }

    public:
        constexpr ConstBitMap() noexcept = default;
        CERBLIB20_CONSTEXPR ~ConstBitMap() noexcept = default;

        constexpr ConstBitMap(ConstBitMap& other) noexcept {
            copy(*this, other);
        }

        constexpr ConstBitMap(ConstBitMap&& other) noexcept {
            copy(*this, other);
        }
    };

    template<typename T, bool Freestanding = false>
    class Bitmap {
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);

    private:
        T *m_data;
        size_t m_size;

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
        auto data() noexcept -> T * {
            return m_data;
        }

    public:
        constexpr auto clear() noexcept -> void {
            CERBLIB_UNROLL_N(4)
            for (size_t i = 0; i < sizeOfArray(); ++i) {
                m_data[i] = 0;
            }
        }

        template<u8 value>
        constexpr auto set(size_t index) noexcept -> void {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            if constexpr (value) {
                m_data[elemIndex] |= (value << static_cast<T>(bitIndex));
            } else {
                m_data[elemIndex] &= ~(1 << static_cast<T>(bitIndex));
            }
        }

        [[nodiscard]] constexpr
        auto isEmpty() const noexcept -> bool {
            return PRIVATE::isEmpty<const T*>(m_data, size());
        }

        constexpr auto resize(size_t size) -> void {
        auto newArraySize = size / bitsizeof(T) + (size % bitsizeof(T)) != 0;

            if (sizeOfArray() < newArraySize) {
                T * newData;

                if constexpr (!Freestanding) {
                    newData = new T[newArraySize];
                    cerb::memcpy(newData, m_data, sizeOfArray());
                } else {
                    newData = m_data;
                }

                cerb::memset<T>(newData + sizeOfArray(), 0, newArraySize - sizeOfArray());
            }

            m_size = size;
        }

    public:
        [[nodiscard]] constexpr
        auto at(size_t index) const noexcept -> u8 {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);
            return (m_data[elemIndex] & (static_cast<T>(1) << bitIndex)) != 0;
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) noexcept -> cerb::PRIVATE::BitmapElem<T> {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            return cerb::PRIVATE::BitmapElem<T>(bitIndex, &m_data[elemIndex]);
        }

        [[nodiscard]] constexpr
        auto operator[](size_t index) const noexcept -> cerb::PRIVATE::BitmapElem<T> {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            return cerb::PRIVATE::BitmapElem<T>(bitIndex, &m_data[elemIndex]);
        }

        template<u8 firstValue> [[nodiscard]] constexpr
        auto find_if() const noexcept -> size_t {
            return PRIVATE::bitmap_find_if<firstValue, const T*>(m_data, size());
        }

    public:
        constexpr auto operator=(const Bitmap& other) noexcept -> Bitmap& {
            static_assert(!Freestanding);

            if (sizeOfArray() < other.sizeOfArray()) {
                delete[] m_data;
                m_data = new T[other.sizeOfArray()];
            }

            m_size = other.size();
            cerb::memcpy(m_data, other.m_data, sizeOfArray());
            return *this;
        }

        constexpr auto operator=(Bitmap&& other) noexcept -> Bitmap&
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

    public: // Bitmap do NOT work in constexpr context :/ but just in case everything is constexpr

        CERBLIB_DISABLE_WARNING(constant-evaluated, constant-evaluated, 0)
        constexpr Bitmap(Bitmap& other)
        : m_data(new T[other.sizeOfArray()]), m_size(other.size())
        {
            static_assert(!Freestanding);
            cerb::memcpy<T>(m_data, other.m_data, sizeOfArray());
        }
        CERBLIB_ENABLE_WARNING(constant-evaluated, constant-evaluated, 0)

        constexpr Bitmap(Bitmap&& other)
        : m_data(other.m_data), m_size(other.size())
        {
            other.m_size = 0;
            other.m_data = nullptr;
        }

        explicit constexpr Bitmap(size_t size)
        : m_data(new T[size / bitsizeof(T) + (size % bitsizeof(T)) != 0]), m_size(size)
        {
            static_assert(!Freestanding);
        }

        explicit constexpr Bitmap(T *data, size_t size)
        : m_data(data), m_size(size)
        {
            static_assert(Freestanding);
        }

        CERBLIB20_CONSTEXPR ~Bitmap() noexcept(Freestanding)
        {
            if constexpr (!Freestanding) {
                delete[] m_data;
            }
        }
    };
}

#endif /* bitmap_hpp */
