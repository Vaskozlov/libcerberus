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
    class constDoubleBitmap final: public PRIVATE::doubleBitmapAPI<T, false, _size> {
        using PRIVATE::doubleBitmapAPI<T, false, _size>::_data1;
        using PRIVATE::doubleBitmapAPI<T, false, _size>::_data2;

    public:
        using PRIVATE::doubleBitmapAPI<T, false, _size>::size;
        using PRIVATE::doubleBitmapAPI<T, false, _size>::sizeOfArray;

    public:
        auto operator=(const constDoubleBitmap &) -> constDoubleBitmap& = default;
        auto operator=(constDoubleBitmap&&) noexcept -> constDoubleBitmap& = default;

        template<int POINTABLE, size_t SIZE>
        auto operator=(const PRIVATE::doubleBitmapAPI<T, POINTABLE, SIZE> &other) noexcept -> constDoubleBitmap& {
            
            if (size() != other.size()) {
                return *this;
            }

            cerb::memcpy(_data1.get(), other.data1(), sizeOfArray());
            cerb::memcpy(_data2.get(), other.data1(), sizeOfArray());

            return *this;
        }

    public:
        constDoubleBitmap() = default;

        constDoubleBitmap(constDoubleBitmap&) = default;
        constDoubleBitmap(constDoubleBitmap&&) noexcept = default;
    };

    template<typename T>
    class TRIVIAL freeDoubleBitmap final : public PRIVATE::doubleBitmapAPI<T, true> {
        using PRIVATE::doubleBitmapAPI<T, true>::_size;
        using PRIVATE::doubleBitmapAPI<T, true>::_data1;
        using PRIVATE::doubleBitmapAPI<T, true>::_data2;

    public:
        using PRIVATE::doubleBitmapAPI<T, true>::size;
        using PRIVATE::doubleBitmapAPI<T, true>::data1;
        using PRIVATE::doubleBitmapAPI<T, true>::data2;
        using PRIVATE::doubleBitmapAPI<T, true>::toAPI;
        using PRIVATE::doubleBitmapAPI<T, true>::sizeOfData;
        using PRIVATE::doubleBitmapAPI<T, true>::sizeOfArray;

    public:
        always_inline void forceResize(size_t newSize) {
            _size[0] = newSize;
        }

    public:
        template<size_t bitmapSize>
        auto operator=(const constDoubleBitmap<T, bitmapSize> &other) -> freeDoubleBitmap<T>& {
            if (sizeOfArray() < other.sizeOfArray()) UNLIKELY {
                _size[0] = 0;
                _data1 = _data2 = nullptr;
                return *this;
            }

            _size[0] = other.size();
            cerb::memcpy(_data1.get(), other.data1(), sizeOfArray());
            cerb::memcpy(_data2.get(), other.data2(), sizeOfArray());

            return *this;
        }

        template<int POINTABLE, size_t SIZE>
        auto operator=(const PRIVATE::doubleBitmapAPI<T, POINTABLE, SIZE> &other) -> freeDoubleBitmap<T>& {
            if (sizeOfArray() < other.sizeOfArray()) UNLIKELY {
                _size[0] = 0;
                _data1 = _data2 = nullptr;
                return *this;
            }

            _size[0] = other.size();
            cerb::memcpy(_data1.get(), other.data1(), sizeOfArray());
            cerb::memcpy(_data2.get(), other.data2(), sizeOfArray());

            return *this;
        }

        inline auto operator=(freeDoubleBitmap &&other) noexcept -> freeDoubleBitmap& {
            _size[0] = std::exchange(other._size, 0);
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
                : PRIVATE::doubleBitmapAPI<T, true>(buffer, size)
        {}

        always_inline freeDoubleBitmap(T *buffer1, T *buffer2, size_t size)
            : PRIVATE::doubleBitmapAPI<T, true>(buffer1, buffer2, size)
        {}

        template<int POINTABLE, size_t SIZE>
        inline freeDoubleBitmap(PRIVATE::doubleBitmapAPI<T, POINTABLE, SIZE> &other, T *buffer)
                : PRIVATE::doubleBitmapAPI<T, true>(buffer, other.size())
        {
            cerb::memcpy(_data1, other.data1(), other.sizeOfArray());
            cerb::memcpy(_data2, other.data2(), other.sizeOfArray());
        }

        template<int POINTABLE, size_t SIZE>
        inline freeDoubleBitmap(PRIVATE::doubleBitmapAPI<T, POINTABLE, SIZE> &other, T *buffer1, T *buffer2)
            : PRIVATE::doubleBitmapAPI<T, true>(buffer1, buffer2, other.size())
        {
            cerb::memcpy(buffer1, other.data1(), other.sizeOfArray());
            cerb::memcpy(buffer2, other.data2(), other.sizeOfArray());
        }

        template<size_t bitmapSize>
        inline freeDoubleBitmap(constDoubleBitmap<T, bitmapSize> &other, T *buffer)
                : PRIVATE::doubleBitmapAPI<T, true>(buffer, other.size())
        {
            cerb::memcpy(_data1, other.data1(), other.sizeOfArray());
            cerb::memcpy(_data2, other.data2(), other.sizeOfArray());
        }

        template<size_t bitmapSize>
        inline freeDoubleBitmap(constDoubleBitmap<T, bitmapSize> &other, T *buffer1, T *buffer2)
            : PRIVATE::doubleBitmapAPI<T, true>(buffer1, buffer2, other.size())
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
