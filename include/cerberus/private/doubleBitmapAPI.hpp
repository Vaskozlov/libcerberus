#ifndef CERBERUS_DOUBLEBITMAPAPI_HPP
#define CERBERUS_DOUBLEBITMAPAPI_HPP

#include <cerberus/types.h>
#include <cerberus/private/bitmapAPI.hpp>

namespace cerb {
    namespace PRIVATE {

        struct TRIVIAL DoubleReturn {
            u8 first : 1;
            u8 second : 1;
        };

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

        template<typename T> [[nodiscard]]
        static auto at2(const T *buffer1, const T *buffer2, size_t index) noexcept -> DoubleReturn {
            DoubleReturn result;
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex = index % bitsizeof(T);

            result.first  = (buffer1[elemIndex] & (static_cast<T>(1) << bitIndex)) != 0;
            result.second = (buffer2[elemIndex] & (static_cast<T>(1) << bitIndex)) != 0;

            return result;
        }

        template<u8 value, typename T>
        void set(T *buffer1, T *buffer2, size_t index) noexcept {
            static_assert(value < 2U, "cerb::bitmap elem can hold only 0 or 1");

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

        template<typename T>
        void set(T *buffer1, T *buffer2, size_t index, u8 value) noexcept {
            auto elemIndex = index / bitsizeof(T);
            auto bitIndex  = index % bitsizeof(T);

            buffer1[elemIndex] &= ~(static_cast<T>(1) << static_cast<T>(bitIndex));
            buffer2[elemIndex] &= ~(static_cast<T>(1) << static_cast<T>(bitIndex));

            buffer1[elemIndex] |= static_cast<T>(value) << static_cast<T>(bitIndex);
            buffer2[elemIndex] |= static_cast<T>(value) << static_cast<T>(bitIndex);
        }

        template<u8 firstValue, u8 secondValue, typename T>
        auto findWithRule(const T *buffer1, const T *buffer2, size_t limit) noexcept -> size_t {
            size_t index = 0;
            auto maxElemIndex = limit / bitsizeof(T);

            auto computeFunction = [](T first, T second) {
                if constexpr (firstValue == 1) {
                    first = ~first;
                }
                if constexpr (secondValue == 1) {
                    second = ~second;
                }

                return first | second;
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

        template<typename T> [[nodiscard]]
        always_inline auto isEmpty(const T *buffer1, const T *buffer2, size_t limit) {
            size_t index = 0;
            auto maxElemIndex = limit / bitsizeof(T);

            // search for value which is smaller than MAX_T
            for (; index < maxElemIndex; index++) {
                if (buffer1[index] > static_cast<T>(0) || buffer2[index] > static_cast<T>(0)) {
                    return false;
                }
            }

            return (
                    (buffer1[index] & value2bits[limit % bitsizeof(T)]) == 0 &&
                    (buffer2[index] & value2bits[limit % bitsizeof(T)]) == 0
            );
        }

        template<typename T>
        class TRIVIAL doubleBitmapAPI {
        protected:
            size_t _size;
            T *_data1, *_data2;

        public:
            [[nodiscard]]
            always_inline auto size() const -> size_t {
                return _size;
            }

            [[nodiscard]]
            always_inline auto data1() const -> T * {
                return _data1;
            }

            [[nodiscard]]
            always_inline auto data2() const -> T * {
                return _data2;
            }

            [[nodiscard]]
            always_inline auto sizeOfArray() const -> size_t {
                return size() / bitsizeof(T) + (size() % bitsizeof(T) != 0);
            }

            [[nodiscard]]
            always_inline auto sizeOfData() const -> size_t {
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
            always_inline auto at(size_t index) const noexcept -> DoubleReturn {
                return PRIVATE::at2(data1(), data2(), index);
            }

            [[nodiscard]]
            always_inline auto operator[](size_t index) noexcept -> DoubleBitmapElem<T> {
                return {
                    static_cast<u16>(index % bitsizeof(T)),
                    _data1 + (index / bitsizeof(T)),
                    _data2 + (index / bitsizeof(T))
                };
            }

        public:
            [[nodiscard]]
            always_inline auto toAPI() -> doubleBitmapAPI<T>& {
                return dynamic_cast<doubleBitmapAPI<T>&>(*this);
            }

        public:
            doubleBitmapAPI() = default;
            ~doubleBitmapAPI() = default;

            doubleBitmapAPI(doubleBitmapAPI&) = default;
            doubleBitmapAPI(doubleBitmapAPI&&) noexcept = default;

            always_inline doubleBitmapAPI(T *buffer, size_t size)
                    : _data1(buffer), _size(size)
            {
                _data2 = _data1 + sizeOfArray();
            }

            always_inline doubleBitmapAPI(T *buffer1, T *buffer2, size_t size)
                : _data1(buffer1), _data2(buffer2), _size(size)
            {}
        };
    } // namespace cerb::PRIVATE
} // namespace cerb

#endif //CERBERUS_DOUBLEBITMAPAPI_HPP
