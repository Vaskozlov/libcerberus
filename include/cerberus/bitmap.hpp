#ifndef bitmap_hpp
#define bitmap_hpp

#include <cerberus/private/bitmapAPI.hpp>

#if defined(__has_warning)
#  if __has_warning("-Wreorder-ctor")
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wreorder-ctor"
#  endif
#endif

namespace cerb {
    template<typename T, size_t _size>
    class constBitmap {
        static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
        T _data[_size / bitsizeof(T) + (_size % bitsizeof(T) > 0)];

    public:
        [[nodiscard]]
        constexpr auto size() const -> size_t {
            return _size;
        }

        [[nodiscard]]
        constexpr auto data() const -> const T * {
            return _data;
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
        always_inline void clear() {
            PRIVATE::clear(_data, size());
        }

        template<auto value>
        always_inline void set(size_t index) {
            PRIVATE::set<value>(_data, index);
        }

        [[nodiscard]]
        always_inline auto isEmpty() const -> bool {
            return PRIVATE::isEmpty(data(), size());
        }

        template<u8 firstValue> [[nodiscard]]
        always_inline auto findWithRule() const -> size_t {
            return PRIVATE::findWithRule<firstValue>(data(), size());
        }

    public:
        [[nodiscard]]
        always_inline auto at(size_t index) const -> u8 {
            return PRIVATE::at(data(), index);
        }

        [[nodiscard]]
        always_inline auto operator[](size_t index) const -> u8 {
            return this->at(index);
        }

        [[nodiscard]]
        always_inline auto operator[](size_t index) noexcept -> PRIVATE::BitmapElem<T> {
            return {static_cast<u16>(index % bitsizeof(T)), _data + (index / bitsizeof(T))};
        }

    public:
        auto operator=(const constBitmap&) -> constBitmap& = default;
        auto operator=(constBitmap&&) noexcept -> constBitmap& = default;

    public:
        constBitmap() = default;
        ~constBitmap() = default;

        constBitmap(constBitmap&) = default;
        constBitmap(constBitmap&&) noexcept = default;
    };
    
    template<typename T>
    class TRIVIAL freeBitmap final : public PRIVATE::bitmapAPI<T>{
        using PRIVATE::bitmapAPI<T>::_data;
        using PRIVATE::bitmapAPI<T>::_size;

    public:
        using PRIVATE::bitmapAPI<T>::size;
        using PRIVATE::bitmapAPI<T>::sizeOfData;
        using PRIVATE::bitmapAPI<T>::sizeOfArray;
        using PRIVATE::bitmapAPI<T>::toAPI;

    public:
        always_inline void forceResize(size_t newSize) {
            _size = newSize;
        }

    public:
        inline auto operator=(freeBitmap<T> &&other) noexcept -> freeBitmap& {
            _size = std::exchange(other._size, 0);
            _data = std::exchange(other._data, nullptr);
        }

        auto operator=(const PRIVATE::bitmapAPI<T> &other) -> freeBitmap& {
            if (sizeOfArray() < other.sizeOfArray()) UNLIKELY {
                _size = 0;
                _data = nullptr;
                return *this;
            }

            _size = other.size();
            cerb::memcpy(_data, other.data(), sizeOfArray());

            return *this;
       }

    public:
        freeBitmap(freeBitmap&) = default;
        freeBitmap(freeBitmap<T> &&other) noexcept = default;

        always_inline freeBitmap()
            : PRIVATE::bitmapAPI<T>::bitmapAPI(nullptr, 0)
        {}

        always_inline freeBitmap(T *buffer, size_t size)
                : PRIVATE::bitmapAPI<T>::bitmapAPI(buffer, size)
        {}

        inline freeBitmap(PRIVATE::bitmapAPI<T> &other, T *buffer)
            : PRIVATE::bitmapAPI<T>::bitmapAPI(buffer, other.size())
        {
            cerb::memcpy(_data, other.data(), sizeOfArray());
        }

        template<size_t bitmapSize>
        inline freeBitmap(const constBitmap<T, bitmapSize> &other, T *buffer)
            : PRIVATE::bitmapAPI<T>::bitmapAPI(buffer, bitmapSize)
        {
            cerb::memcpy(buffer, other.data(), other.sizeOfArray());
        }
    };

    template<typename T>
    class TRIVIAL bitmap final : public PRIVATE::bitmapAPI<T>{
        u64 _capacity;

    private:
        using PRIVATE::bitmapAPI<T>::_data;
        using PRIVATE::bitmapAPI<T>::_size;

    public:
        using PRIVATE::bitmapAPI<T>::size;
        using PRIVATE::bitmapAPI<T>::sizeOfData;
        using PRIVATE::bitmapAPI<T>::sizeOfArray;
        using PRIVATE::bitmapAPI<T>::toAPI;

    public:
        [[nodiscard]]
        always_inline auto capacity() const -> size_t {
            return _capacity;
        }

        [[nodiscard]]
        always_inline auto capacityOfArray() const -> size_t {
            return capacity() / bitsizeof(T);
        }

        [[nodiscard]]
        always_inline auto capacitySize() const -> size_t {
            return capacityOfArray() * sizeof(T);
        }

    public:
        inline void forceResize(size_t newSize) {
            if (capacityOfArray() > sizeOfArray()) {
                auto oldSize = capacityOfArray();
                auto oldBuffer = _data;

                _capacity = cerb::align<cerb::log2(bitsizeof(T))>(newSize);
                _data = static_cast<T*>(::operator new(capacitySize()));
                cerb::memcpy(_data, oldBuffer, oldSize);

                ::operator delete(oldBuffer);

                for (; oldSize < sizeOfArray(); oldSize++) {
                    _data[oldSize] = 0;
                }
            }

            _size = newSize;
        }

    public:

        inline auto operator=(bitmap &&other) noexcept -> bitmap<T>& {
            ::operator delete(_data);

            _size = std::exchange(other.size(), 0);
            _data = std::exchange(other._data, nullptr);
            _capacity = std::exchange(other.capacity(), nullptr);

            return *this;
        }

        inline auto operator=(const PRIVATE::bitmapAPI<T> &other) noexcept -> bitmap<T>& {
            if (capacityOfArray() < other.sizeOfArray()) UNLIKELY {
                ::operator delete(_data);
                _capacity = cerb::align<cerb::log2(bitsizeof(T))>(other.size());
                _data = static_cast<T*>(::operator new(capacitySize()));
            }

            _size = other.size();
            cerb::memcpy(_data, other.data(), sizeOfArray());

            return *this;
        }

        template<size_t bitmapSize>
        inline auto operator=(constBitmap<T, bitmapSize> &other) noexcept -> bitmap<T>& {
            if (other.sizeOfArray() < capacityOfArray()) UNLIKELY {
                ::operator delete (_data);
                _capacity = cerb::align<cerb::log2(bitsizeof(T))>(other.size());
                _data = static_cast<T*>(::operator new(capacitySize()));
            }

            _size = other.size();
            cerb::memcpy(_data, other.data(), other.sizeOfData());

            return *this;
        }

        template<size_t bitmapSize>
        auto operator=(constBitmap<T, bitmapSize> &&other) -> bitmap<T>& = delete;

    public:
        always_inline bitmap() :
                _capacity(0),
                PRIVATE::bitmapAPI<T>::bitmapAPI(nullptr, 0)
        {}

        inline bitmap(PRIVATE::bitmapAPI<T> &other) noexcept :
                _capacity(cerb::align<cerb::log2(bitsizeof(T))>(other.size())),
                PRIVATE::bitmapAPI<T>::bitmapAPI(
                        static_cast<T*>(::operator new(other.sizeOfData())),
                        other.size()
                )
        {
            cerb::memcpy(_data, other.data(), other.sizeOfArray());
        }

        inline bitmap(bitmap &&other) noexcept {
            _size = std::exchange(other._size, 0);
            _data = std::exchange(other._data, nullptr);
            _capacity = std::exchange(other._capacity, 0);
        }

        template<size_t bitmapSize>
        inline bitmap(constBitmap<T, bitmapSize> &other) noexcept :
                _capacity(other.sizeOfArray() * bitsizeof(T)),
                PRIVATE::bitmapAPI<T>::bitmapAPI(
                        static_cast<T*>(::operator new(other.sizeOfData())),
                        bitmapSize
                )
        {
            cerb::memcpy(_data, other.data(), capacityOfArray());
        }

        inline bitmap(size_t numberOfElems) :
                PRIVATE::bitmapAPI<T>::bitmapAPI(
                    static_cast<T*>(::operator new(cerb::align<cerb::log2(bitsizeof(T))>(numberOfElems) / sizeof(T))),
                    numberOfElems
            ), _capacity(cerb::align<cerb::log2(bitsizeof(T))>(numberOfElems))
        {}

        ~bitmap() {
            ::operator delete (_data);
        }
    };
}

#if defined(__has_warning)
#  if __has_warning("-Wreorder-ctor")
#     pragma GCC diagnostic pop
#  endif
#endif

#endif /* bitmap_hpp */
