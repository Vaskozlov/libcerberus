#ifndef CERBERUS_DOUBLEBITMAPAPI_HPP
#define CERBERUS_DOUBLEBITMAPAPI_HPP

#include <cerberus/types.h>
#include <cerberus/private/bitmapAPI.hpp>

namespace cerb {
    namespace PRIVATE {

        template<typename T>
        struct DoubleBitmapElem {
            BitmapElem<T> first;
            BitmapElem<T> second;

        public:
            auto operator=(DoubleBitmapElem<T>&) -> DoubleBitmapElem<T>& = default;
            auto operator=(DoubleBitmapElem<T>&&) noexcept -> DoubleBitmapElem<T>& = default;

            always_inline auto operator=(std::pair<u8, u8> newValues) noexcept -> DoubleBitmapElem<T>& {
                first = newValues.first;
                second = newValues.second;

                return *this;
            }

        public:
            ~DoubleBitmapElem() = default;
            DoubleBitmapElem(DoubleBitmapElem&) = default;
            DoubleBitmapElem(DoubleBitmapElem&&) noexcept = default;

            always_inline DoubleBitmapElem(u16 bitIndex, T *elem1, T *elem2) noexcept
                : first(bitIndex, elem1), second(bitIndex, elem2)
            {
            }
        } __attribute__((packed));

        template<auto value, typename T>
        static void set(T *buffer1, T *buffer2, size_t index) noexcept {
            static_assert(value < 2, "cerb::bitmap elem can hold only 0 or 1");

            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            if constexpr (value) {
                buffer1[elemIndex] |= static_cast<T>(value) << static_cast<T>(bitIndex);
                buffer2[elemIndex] |= static_cast<T>(value) << static_cast<T>(bitIndex);
            } else {
                buffer1[elemIndex] &= ~(static_cast<T>(1) << static_cast<T>(bitIndex));
                buffer2[elemIndex] &= ~(static_cast<T>(1) << static_cast<T>(bitIndex));
            }
        }

        template<auto firstValue, auto secondValue, typename T>
        static auto findWithRule(const T *buffer1, const T *buffer2, size_t limit) noexcept -> size_t {
            size_t index = 0;
            auto maxElemIndex = limit / bitsizeof(T);

            auto computeFunction = [](T first, T second) {
                if constexpr (firstValue == 1) {
                    firstValue = ~firstValue;
                }
                if constexpr (secondValue == 1) {
                    secondValue = ~secondValue;
                }

                return firstValue | secondValue;
            };

            for (; index < maxElemIndex; index++) {
                T value = computeFunction(buffer1[index], buffer2[index]);

                if (value < ~static_cast<T>(0)) {
                    return index * bitsizeof(T) + cerb::findFreeBit(static_cast<u64>(value));
                }
            }

            T value = computeFunction(buffer1[index], buffer2[index]);

            if (value < ~static_cast<T>(0)) {
                auto bitIndex = cerb::findFreeBit(static_cast<u64>(value));
                return cerb::cmov(bitIndex < limit % bitsizeof(T), index * bitsizeof(T) + bitIndex, UINTMAX_MAX);
            }

            return UINTMAX_MAX;
        }
    } // namespace cerb::PRIVATE
} // namespace cerb

#endif //CERBERUS_DOUBLEBITMAPAPI_HPP
