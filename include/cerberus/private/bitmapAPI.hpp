#ifndef cerberusBitmapAPI_hpp
#define cerberusBitmapAPI_hpp

#include <utility>
#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include <cerberus/string.hpp>
#include <cerberus/arrayProtocol.hpp>

#if defined(__has_warning)
#  if __has_warning("-Wreorder-ctor")
#    pragma GCC diagnostic push
#    pragma clang diagnostic ignored "-Wreorder-ctor"
#  endif
#endif

namespace cerb {
    namespace PRIVATE {
        static inline const u64 value2bits[] = {
            0b0,
            0b1UL,
            0b11UL,
            0b111UL,
            0b1111UL,
            0b11111UL,
            0b111111UL,
            0b1111111UL,
            0b11111111UL,
            0b111111111UL,
            0b1111111111UL,
            0b11111111111UL,
            0b111111111111UL,
            0b1111111111111UL,
            0b11111111111111UL,
            0b111111111111111UL,
            0b1111111111111111UL,
            0b11111111111111111UL,
            0b111111111111111111UL,
            0b1111111111111111111UL,
            0b11111111111111111111UL,
            0b111111111111111111111UL,
            0b1111111111111111111111UL,
            0b11111111111111111111111UL,
            0b111111111111111111111111UL,
            0b1111111111111111111111111UL,
            0b11111111111111111111111111UL,
            0b111111111111111111111111111UL,
            0b1111111111111111111111111111UL,
            0b11111111111111111111111111111UL,
            0b111111111111111111111111111111UL, 
            0b1111111111111111111111111111111UL,
            0b11111111111111111111111111111111UL,
            0b111111111111111111111111111111111UL,
            0b1111111111111111111111111111111111UL,
            0b11111111111111111111111111111111111UL,
            0b111111111111111111111111111111111111UL,
            0b1111111111111111111111111111111111111UL,
            0b11111111111111111111111111111111111111UL,
            0b111111111111111111111111111111111111111UL,
            0b1111111111111111111111111111111111111111UL,
            0b11111111111111111111111111111111111111111UL,
            0b111111111111111111111111111111111111111111UL,
            0b1111111111111111111111111111111111111111111UL,
            0b11111111111111111111111111111111111111111111UL,
            0b111111111111111111111111111111111111111111111UL,
            0b1111111111111111111111111111111111111111111111UL,
            0b11111111111111111111111111111111111111111111111UL,
            0b111111111111111111111111111111111111111111111111UL,
            0b1111111111111111111111111111111111111111111111111UL,
            0b11111111111111111111111111111111111111111111111111UL,
            0b111111111111111111111111111111111111111111111111111UL,
            0b1111111111111111111111111111111111111111111111111111UL,
            0b11111111111111111111111111111111111111111111111111111UL,
            0b111111111111111111111111111111111111111111111111111111UL,
            0b1111111111111111111111111111111111111111111111111111111UL,
            0b11111111111111111111111111111111111111111111111111111111UL,
            0b111111111111111111111111111111111111111111111111111111111UL,
            0b1111111111111111111111111111111111111111111111111111111111UL,
            0b11111111111111111111111111111111111111111111111111111111111UL,
            0b111111111111111111111111111111111111111111111111111111111111UL,
            0b1111111111111111111111111111111111111111111111111111111111111UL,
            0b11111111111111111111111111111111111111111111111111111111111111UL,
            0b111111111111111111111111111111111111111111111111111111111111111UL,
            0b1111111111111111111111111111111111111111111111111111111111111111UL
        };

        template<typename T>
        class TRIVIAL BitmapElem {
            u16 _bitIndex;
            T *_elem;

        public:
            always_inline auto value() noexcept -> u8 {
                return (*_elem & (static_cast<T>(1) << _bitIndex)) != 0;
            }

        public:
            auto operator=(const BitmapElem&) -> BitmapElem& = default;
            auto operator=(BitmapElem&&) noexcept -> BitmapElem& = default;

            always_inline auto operator=(u8 newValue) noexcept -> BitmapElem& {
                *_elem &= ~(static_cast<T>(1) << _bitIndex);
                *_elem |= static_cast<T>(newValue) << _bitIndex;
                return *this;
            }

        public:
            ~BitmapElem() = default;
            BitmapElem(BitmapElem&) = default;
            BitmapElem(BitmapElem&&) noexcept = default;

            always_inline BitmapElem(u16 bitIndex, T *elem) noexcept
                : _bitIndex(bitIndex), _elem(elem)
            {
            }
        } __attribute__((packed));        

        template<typename T>
        static void set(T *buffer, size_t index, u8 value) noexcept {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);
            
            buffer[elemIndex] &= ~(static_cast<T>(1) << bitIndex);
            buffer[elemIndex] |= static_cast<T>(value) << bitIndex;
        }

        template<auto value, typename T>
        static void set(T *buffer, size_t index) noexcept {
            static_assert(value < 2, "cerb::bitmap elem can hold only 0 or 1");

            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);
            
            if constexpr (value) {
                buffer[elemIndex] |= static_cast<T>(value) << static_cast<T>(bitIndex);
            } else {
                buffer[elemIndex] &= ~(static_cast<T>(1) << static_cast<T>(bitIndex));
            }
        }

        template<typename T>
        static auto at(const T *buffer, size_t index) noexcept -> u8 {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex = index % bitsizeof(T);
            return (buffer[elemIndex] & (static_cast<T>(1) << bitIndex)) != 0;
        }

        template<u8 firstValue, typename T>
        static auto findWithRule(const T *buffer, size_t limit) noexcept -> size_t {
            size_t index = 0;
            auto maxElemIndex = limit / bitsizeof(T);

            auto computeFunction = [](T first) {
                if constexpr (firstValue == 1) {
                    first = ~first;
                }
                return first;
            };
            
            // search for value which is smaller than max(T)
            for (; index < maxElemIndex; ++index) {
                T value = computeFunction(buffer[index]);

                if (value < ~static_cast<T>(0)) {
                    return index * bitsizeof(T) + cerb::findFreeBit(static_cast<u64>(value));
                }
            }

            // limit may be not a power of 2, so we need to remainder
            T value = computeFunction(buffer[index]);
            if (value < ~static_cast<T>(0)) { // we MUST check if there are some free values, otherwise findFreeBit is unsafe
                auto bitIndex = cerb::findFreeBit(value);
                return cerb::cmov<size_t>(bitIndex < limit % bitsizeof(T), index * bitsizeof(T) + bitIndex, UINTMAX_MAX);
            }

            return UINTMAX_MAX;
        }

        template<typename T>
        static void clear(T *buffer, size_t limit) noexcept {
            if constexpr (sizeof(T) == sizeof(u8)) {
                cerb::memset8 (buffer, static_cast<u8 >(0), limit / bitsizeof(T) + (limit % bitsizeof(T) != 0));
            } else if constexpr (sizeof(T) == sizeof(u16)) {
                cerb::memset16(buffer, static_cast<u16>(0), limit / bitsizeof(T) + (limit % bitsizeof(T) != 0));
            } else if constexpr (sizeof(T) == sizeof(u32)) {
                cerb::memset32(buffer, static_cast<u32>(0), limit / bitsizeof(T) + (limit % bitsizeof(T) != 0));
            } else if constexpr (sizeof(T) == sizeof(u64)) {
                cerb::memset64(buffer, static_cast<u64>(0), limit / bitsizeof(T) + (limit % bitsizeof(T) != 0));
            } else {
                cerb::memset8 (buffer, static_cast<u8 >(0), limit / bitsizeof(u8) + (limit % bitsizeof(T) != 0));
            }
        }

        template<typename T>
        static auto isEmpty(const T *buffer, size_t limit) -> bool {
            size_t index = 0;
            auto maxElemIndex = limit / bitsizeof(T);
            
            // search for value which is smaller than MAX_T
            for (; index < maxElemIndex; index++) {
                if (buffer[index] > static_cast<T>(0)) {
                    return false;
                }
            }

            return (buffer[index] & value2bits[limit % bitsizeof(T)]) == 0;
        }

        template<typename T, int POINTABLE, size_t SIZE = 0>
        class TRIVIAL bitmapAPI {
        public:
            static constexpr auto protocolSize() -> size_t {
                return SIZE / bitsizeof(T) + (SIZE % bitsizeof(T) > 0);
            }

        protected:
            size_t  _size[POINTABLE];
            cerb::ArrayProtocol<T, POINTABLE, protocolSize()> _data;

        public:
            [[nodiscard]]
            constexpr always_inline auto size() const -> size_t {
                if constexpr (POINTABLE) {
                    return _size[0];
                } else {
                    return SIZE;
                }
            }

            [[nodiscard]]
            constexpr always_inline auto data() const -> const T * {
                return _data.get();
            }

            [[nodiscard]]
            constexpr always_inline auto sizeOfArray() const -> size_t {
                return size() / bitsizeof(T) + (size() % bitsizeof(T) != 0);
            }

            [[nodiscard]]
            constexpr always_inline auto sizeOfData() const -> size_t {
                return sizeOfArray() * sizeof(T);
            }

        public:
            always_inline void clear() {
                PRIVATE::clear(_data.get(), size());
            }

            [[nodiscard]]
            always_inline auto isEmpty() const -> bool {
                return PRIVATE::isEmpty(data(), size());
            }

            template<u8 firstValue> [[nodiscard]]
            always_inline auto findWithRule() const noexcept -> size_t {
                return PRIVATE::findWithRule<firstValue>(data(), size());
            }

            template<u8 value>
            always_inline void set(size_t index) noexcept {
                PRIVATE::set<value>(_data.get(), index);
            }

            always_inline void set(size_t index, u8 value) noexcept {
                PRIVATE::set(_data.get(), index, value);
            }

        public:
            [[nodiscard]]
            always_inline auto at(size_t index) const noexcept -> u8 {
                return PRIVATE::at(_data.get(), index);
            }

            [[nodiscard]]
            always_inline auto operator[](size_t index) noexcept -> BitmapElem<T> {
                return {static_cast<u16>(index % bitsizeof(T)), _data.get() + (index / bitsizeof(T))};
            }

        public:
            [[nodiscard]]
            always_inline auto toAPI() -> bitmapAPI<T, POINTABLE, protocolSize()>& {
                return dynamic_cast<bitmapAPI<T, POINTABLE, protocolSize()>&>(*this);
            }

        public:
            bitmapAPI() = default;
            
            ~bitmapAPI() = default;

            bitmapAPI(bitmapAPI&) = default;
            bitmapAPI(bitmapAPI&&) noexcept = default;

            always_inline bitmapAPI(T *buffer, size_t size)
                : _data(buffer)
            {
                static_assert(POINTABLE);
                _size[0] = size;
            }
        };

        template<typename T>
        using pointableBitmap = bitmapAPI<T, true>;

        template<typename T, size_t SIZE>
        using staticBitmap = bitmapAPI<T, false, SIZE>;

    } // namespace cerb::PRIVATE
} // namespace cerb

#if defined(__has_warning)
#  if __has_warning("-Wreorder-ctor")
#     pragma GCC diagnostic pop
#  endif
#endif

#endif /* cerberusBitmapAPI_hpp */
