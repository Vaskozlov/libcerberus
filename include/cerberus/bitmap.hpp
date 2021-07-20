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
    class constBitmap : public PRIVATE::bitmapAPI<T, false, _size>{
        static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);

    public:
        auto operator=(const constBitmap&) -> constBitmap& = default;
        auto operator=(constBitmap&&) noexcept -> constBitmap& = default;

    public:
        constBitmap() = default;

        constBitmap(constBitmap&) = default;
        constBitmap(constBitmap&&) noexcept = default;
    };
    
    template<typename T>
    class TRIVIAL freeBitmap final : public PRIVATE::bitmapAPI<T, true>{
        using PRIVATE::bitmapAPI<T, true>::_data;
        using PRIVATE::bitmapAPI<T, true>::_size;

    public:
        using PRIVATE::bitmapAPI<T, true>::size;
        using PRIVATE::bitmapAPI<T, true>::sizeOfData;
        using PRIVATE::bitmapAPI<T, true>::sizeOfArray;
        using PRIVATE::bitmapAPI<T, true>::toAPI;

    public:
        always_inline void forceResize(size_t newSize) {
            _size[0] = newSize;
        }

    public:
        inline auto operator=(freeBitmap<T> &&other) noexcept -> freeBitmap& {
            _size[0] = std::exchange(other._size, 0);
            _data = std::exchange(other._data, nullptr);
        }

        template<int POINTABLE, size_t SIZE>
        auto operator=(const PRIVATE::bitmapAPI<T, POINTABLE, SIZE> &other) -> freeBitmap& {
            if (sizeOfArray() < other.sizeOfArray()) UNLIKELY {
                _size[0] = 0;
                _data = nullptr;
                return *this;
            }

            _size[0] = other.size();
            cerb::memcpy(_data.get(), other.data(), sizeOfArray());

            return *this;
       }

    public:
        freeBitmap(freeBitmap&) = default;
        freeBitmap(freeBitmap<T> &&other) noexcept = default;

        always_inline freeBitmap()
            : PRIVATE::bitmapAPI<T, true>::bitmapAPI(nullptr, 0)
        {}

        always_inline freeBitmap(T *buffer, size_t size)
                : PRIVATE::bitmapAPI<T, true>::bitmapAPI(buffer, size)
        {}

        template<int POINTABLE, size_t SIZE>
        inline freeBitmap(PRIVATE::bitmapAPI<T, POINTABLE, SIZE> &other, T *buffer)
            : PRIVATE::bitmapAPI<T, true>::bitmapAPI(buffer, other.size())
        {
            cerb::memcpy(_data.get(), other.data(), sizeOfArray());
        }

        template<size_t bitmapSize>
        inline freeBitmap(const constBitmap<T, bitmapSize> &other, T *buffer)
            : PRIVATE::bitmapAPI<T, true>::bitmapAPI(buffer, bitmapSize)
        {
            cerb::memcpy(buffer, other.data(), other.sizeOfArray());
        }
    };

    template<typename T>
    class TRIVIAL bitmap final : public PRIVATE::bitmapAPI<T, true>{
        u64 _capacity;

    private:
        using PRIVATE::bitmapAPI<T, true>::_data;
        using PRIVATE::bitmapAPI<T, true>::_size;

    public:
        using PRIVATE::bitmapAPI<T, true>::size;
        using PRIVATE::bitmapAPI<T, true>::sizeOfData;
        using PRIVATE::bitmapAPI<T, true>::sizeOfArray;
        using PRIVATE::bitmapAPI<T, true>::toAPI;

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
                auto oldBuffer = _data.get();

                _capacity = cerb::align<cerb::log2(bitsizeof(T))>(newSize);
                _data = static_cast<T*>(::operator new(capacitySize()));
                cerb::memcpy(_data.get(), oldBuffer, oldSize);

                ::operator delete(oldBuffer);

                for (; oldSize < sizeOfArray(); oldSize++) {
                    _data[oldSize] = 0;
                }
            }

            _size[0] = newSize;
        }

    public:

        inline auto operator=(bitmap &&other) noexcept -> bitmap<T>& {
            ::operator delete(_data.get());

            _size[0] = std::exchange(other.size(), 0);
            _data = std::exchange(other._data, nullptr);
            _capacity = std::exchange(other.capacity(), nullptr);

            return *this;
        }

        template<int POINTABLE, size_t SIZE>
        inline auto operator=(const PRIVATE::bitmapAPI<T, POINTABLE, SIZE> &other) noexcept -> bitmap<T>& {
            if (capacityOfArray() < other.sizeOfArray()) UNLIKELY {
                ::operator delete(_data.get());
                _capacity = cerb::align<cerb::log2(bitsizeof(T))>(other.size());
                _data = static_cast<T*>(::operator new(capacitySize()));
            }

            _size[0] = other.size();
            cerb::memcpy(_data.get(), other.data(), sizeOfArray());

            return *this;
        }

        template<size_t bitmapSize>
        inline auto operator=(constBitmap<T, bitmapSize> &other) noexcept -> bitmap<T>& {
            if (other.sizeOfArray() < capacityOfArray()) UNLIKELY {
                ::operator delete (_data.get());
                _capacity = cerb::align<cerb::log2(bitsizeof(T))>(other.size());
                _data = static_cast<T*>(::operator new(capacitySize()));
            }

            _size[0] = other.size();
            cerb::memcpy(_data.get(), other.data(), other.sizeOfData());

            return *this;
        }

        template<size_t bitmapSize>
        auto operator=(constBitmap<T, bitmapSize> &&other) -> bitmap<T>& = delete;

    public:
        always_inline bitmap() :
                _capacity(0),
                PRIVATE::bitmapAPI<T, true>::bitmapAPI(nullptr, 0)
        {}

        template<int POINTABLE, size_t SIZE>
        inline bitmap(PRIVATE::bitmapAPI<T, POINTABLE, SIZE> &other) noexcept :
                _capacity(cerb::align<cerb::log2(bitsizeof(T))>(other.size())),
                PRIVATE::bitmapAPI<T, true>::bitmapAPI(
                        static_cast<T*>(::operator new(other.sizeOfData())),
                        other.size()
                )
        {
            cerb::memcpy(_data.get(), other.data(), other.sizeOfArray());
        }

        inline bitmap(bitmap &&other) noexcept {
            _size[0] = std::exchange(other._size, 0);
            _data = std::exchange(other._data, nullptr);
            _capacity = std::exchange(other._capacity, 0);
        }

        template<size_t bitmapSize>
        inline bitmap(constBitmap<T, bitmapSize> &other) noexcept :
                _capacity(other.sizeOfArray() * bitsizeof(T)),
                PRIVATE::bitmapAPI<T, true>::bitmapAPI(
                        static_cast<T*>(
                                ::operator new(other.sizeOfData())
                        ),
                        bitmapSize
                )
        {
            cerb::memcpy(_data.get(), other.data(), capacityOfArray());
        }

        inline bitmap(size_t numberOfElems) :
                PRIVATE::bitmapAPI<T, true>::bitmapAPI(
                    static_cast<T*>(::operator new(cerb::align<cerb::log2(bitsizeof(T))>(numberOfElems) / sizeof(T))),
                    numberOfElems
            ), _capacity(cerb::align<cerb::log2(bitsizeof(T))>(numberOfElems))
        {}

        ~bitmap() {
            ::operator delete (_data.get());
        }
    };
}

#if defined(__has_warning)
#  if __has_warning("-Wreorder-ctor")
#     pragma GCC diagnostic pop
#  endif
#endif

#endif /* bitmap_hpp */
