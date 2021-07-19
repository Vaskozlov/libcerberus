#ifndef CERBERUS_DOUBLEBITMAP_HPP
#define CERBERUS_DOUBLEBITMAP_HPP

#include <cerberus/types.h>
#include <cerberus/private/doubleBitmapAPI.hpp>

#if defined(__has_warning)
#  if __has_warning("-Wreorder-ctor")
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wreorder-ctor"
#  endif
#endif


namespace cerb {

    template<typename T, size_t _size>
    class constDoubleBitmap {
        static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
        T _data1[_size / bitsizeof(T) + (_size % bitsizeof(T) > 0)];
        T _data2[_size / bitsizeof(T) + (_size % bitsizeof(T) > 0)];

    public:
        [[nodiscard]]
        constexpr auto size() const -> size_t {
            return _size;
        }

        [[nodiscard]]
        constexpr auto data1() const -> const T * {
            return _data1;
        }

        [[nodiscard]]
        constexpr auto data2() const -> const T * {
            return _data2;
        }

        [[nodiscard]]
        constexpr auto sizeOfArray() const -> size_t {
            return size() / bitsizeof(T) + (size() % bitsizeof(T) != 0);
        }

        [[nodiscard]]
        constexpr auto sizeOfData() const -> size_t {
            return sizeOfArray() * sizeof(T);
        }

    public:
        always_inline void clear1() {
            PRIVATE::clear(_data1, size());
        }

        always_inline void clear2() {
            PRIVATE::clear(_data2, size());
        }

        always_inline void clear() {
            this->clear1();
            this->clear2();
        }

        [[nodiscard]]
        always_inline auto isEmpty1() const -> bool {
            return PRIVATE::isEmpty(data1(), size());
        }

        [[nodiscard]]
        always_inline auto isEmpty2() const -> bool {
            return PRIVATE::isEmpty(data2(), size());
        }

        [[nodiscard]]
        always_inline auto isEmpty() const -> bool {
            return PRIVATE::isEmpty(data1(), data2(), size());
        }

        template<u8 firstValue> [[nodiscard]]
        always_inline auto findWithRule1() const noexcept -> size_t {
            return PRIVATE::findWithRule<firstValue>(data1(), size());
        }

        template<u8 firstValue> [[nodiscard]]
        always_inline auto findWithRule2() const noexcept -> size_t {
            return PRIVATE::findWithRule<firstValue>(data2(), size());
        }

        template<u8 firstValue, u8 secondValue> [[nodiscard]]
        always_inline auto findWithRule() const noexcept -> size_t {
            return PRIVATE::findWithRule<firstValue, secondValue>(data1(), data2(), size());
        }

        template<u8 value>
        always_inline void set1(size_t index) noexcept {
            PRIVATE::set<value>(_data1, index);
        }

        template<u8 value>
        always_inline void set2(size_t index) noexcept {
            PRIVATE::set<value>(_data2, index);
        }

        template<u8 value>
        always_inline void set(size_t index) noexcept {
            PRIVATE::set<value>(_data1, _data2, index);
        }

        always_inline void set1(size_t index, u8 value) noexcept {
            PRIVATE::set(_data1, index, value);
        }

        always_inline void set2(size_t index, u8 value) noexcept {
            PRIVATE::set(_data2, index, value);
        }

        always_inline void set(size_t index, u8 value) noexcept {
            PRIVATE::set(_data1, _data2, index, value);
        }

    public:
        [[nodiscard]]
        always_inline auto at1(size_t index) const noexcept -> u8 {
            return PRIVATE::at(data1(), index);
        }

        [[nodiscard]]
        always_inline auto at2(size_t index) const noexcept -> u8 {
            return PRIVATE::at(data2(), index);
        }

        [[nodiscard]]
        always_inline auto at(size_t index) const noexcept -> PRIVATE::DoubleReturn {
            return PRIVATE::at2(data1(), data2(), index);
        }

        [[nodiscard]]
        always_inline auto operator[](size_t index) noexcept -> PRIVATE::DoubleBitmapElem<T> {
            return {
                static_cast<u16>(index % bitsizeof(T)),
                _data1 + (index / bitsizeof(T)),
                _data2 + (index / bitsizeof(T))
            };
        }

    public:
        auto operator=(const constDoubleBitmap &) -> constDoubleBitmap& = default;
        auto operator=(constDoubleBitmap&&) noexcept -> constDoubleBitmap& = default;

        auto operator=(const PRIVATE::doubleBitmapAPI<T> &other) noexcept {
            if (size() != other.size()) {
                return *this;
            }

            cerb::memcpy(_data1, other.data1(), sizeOfArray());
            cerb::memcpy(_data2, other.data1(), sizeOfArray());
        }

    public:
        constDoubleBitmap() = default;
        ~constDoubleBitmap() = default;

        constDoubleBitmap(constDoubleBitmap&) = default;
        constDoubleBitmap(constDoubleBitmap&&) = default;
    };

    template<typename T>
    class TRIVIAL freeDoubleBitmap final : public PRIVATE::doubleBitmapAPI<T> {
        using PRIVATE::doubleBitmapAPI<T>::_size;
        using PRIVATE::doubleBitmapAPI<T>::_data1;
        using PRIVATE::doubleBitmapAPI<T>::_data2;

    public:
        using PRIVATE::doubleBitmapAPI<T>::size;
        using PRIVATE::doubleBitmapAPI<T>::data1;
        using PRIVATE::doubleBitmapAPI<T>::data2;
        using PRIVATE::doubleBitmapAPI<T>::toAPI;
        using PRIVATE::doubleBitmapAPI<T>::sizeOfData;
        using PRIVATE::doubleBitmapAPI<T>::sizeOfArray;

    public:
        always_inline void forceResize(size_t newSize) {
            _size = newSize;
        }

    public:
        template<size_t bitmapSize>
        auto operator=(const constDoubleBitmap<T, bitmapSize> &other) -> freeDoubleBitmap<T>& {
            if (sizeOfArray() < other.sizeOfArray()) UNLIKELY {
                _size = 0;
                _data1 = _data2 = nullptr;
                return *this;
            }

            _size = other.size();
            cerb::memcpy(_data1, other.data1(), sizeOfArray());
            cerb::memcpy(_data2, other.data2(), sizeOfArray());

            return *this;
        }

        auto operator=(const PRIVATE::doubleBitmapAPI<T> &other) -> freeDoubleBitmap<T>& {
            if (sizeOfArray() < other.sizeOfArray()) UNLIKELY {
                _size = 0;
                _data1 = _data2 = nullptr;
                return *this;
            }

            _size = other.size();
            cerb::memcpy(_data1, other.data1(), sizeOfArray());
            cerb::memcpy(_data2, other.data2(), sizeOfArray());
            return *this;
        }

        inline auto operator=(PRIVATE::doubleBitmapAPI<T> &&other) noexcept -> freeDoubleBitmap& {
            _size = std::exchange(other._size, 0);
            _data1 = std::exchange(other._data1, nullptr);
            _data2 = std::exchange(other._data2, nullptr);

            return *this;
        }

    public:
        freeDoubleBitmap() = default;
        ~freeDoubleBitmap() = default;

        freeDoubleBitmap(freeDoubleBitmap &) = default;
        freeDoubleBitmap(freeDoubleBitmap &&) noexcept = default;

        always_inline freeDoubleBitmap(T *buffer, size_t size)
                : PRIVATE::doubleBitmapAPI<T>(buffer, size)
        {}

        always_inline freeDoubleBitmap(T *buffer1, T *buffer2, size_t size)
            : PRIVATE::doubleBitmapAPI<T>(buffer1, buffer2, size)
        {}

        inline freeDoubleBitmap(PRIVATE::doubleBitmapAPI<T> &other, T *buffer)
                : PRIVATE::doubleBitmapAPI<T>(buffer, other.size())
        {
            cerb::memcpy(_data1, other.data1(), other.sizeOfArray());
            cerb::memcpy(_data2, other.data2(), other.sizeOfArray());
        }

        inline freeDoubleBitmap(PRIVATE::doubleBitmapAPI<T> &other, T *buffer1, T *buffer2)
            : PRIVATE::doubleBitmapAPI<T>(buffer1, buffer2, other.size())
        {
            cerb::memcpy(buffer1, other.data1(), other.sizeOfArray());
            cerb::memcpy(buffer2, other.data2(), other.sizeOfArray());
        }

        template<size_t bitmapSize>
        inline freeDoubleBitmap(constDoubleBitmap<T, bitmapSize> &other, T *buffer)
                : PRIVATE::doubleBitmapAPI<T>(buffer, other.size())
        {
            cerb::memcpy(_data1, other.data1(), other.sizeOfArray());
            cerb::memcpy(_data2, other.data2(), other.sizeOfArray());
        }

        template<size_t bitmapSize>
        inline freeDoubleBitmap(constDoubleBitmap<T, bitmapSize> &other, T *buffer1, T *buffer2)
            : PRIVATE::doubleBitmapAPI<T>(buffer1, buffer2, other.size())
        {
            cerb::memcpy(buffer1, other.data1(), other.sizeOfArray());
            cerb::memcpy(buffer2, other.data2(), other.sizeOfArray());
        }
    };
}

#if defined(__has_warning)
#  if __has_warning("-Wreorder-ctor")
#     pragma GCC diagnostic pop
#  endif
#endif

#endif /* CERBERUS_DOUBLEBITMAP_HPP */
