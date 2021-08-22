#ifndef CERBERUS_CONTAINER_HPP
#define CERBERUS_CONTAINER_HPP

#include <cerberus/types.h>
#include <cerberus/bitmap.hpp>

namespace cerb::gl {
    template<typename T, size_t Size, bool CheckMemory = true>
    struct CERBLIB_TRIVIAL Container
    {
        using storage_t = std::array<T, Size>;
        using tracker_t = ConstBitmap<1, Size>;

    private:
        storage_t m_data{};
        tracker_t m_tracker{};

    public:
        constexpr auto size() const
        {
            return Size;
        }

        constexpr auto tracker() const noexcept -> const tracker_t &
        {
            return m_tracker;
        }

    private:
        constexpr auto checkIndex(size_t index) noexcept(!CheckMemory)
        {
            if constexpr (CheckMemory) {
                if (index >= Size) {
                    throw std::out_of_range(
                        "cerb::Container is out of free elems "
                        "or index is out of bounce");
                }
            }
        }

    public:
        constexpr auto at(size_t index) const noexcept(!CheckMemory) -> T &
        {
            checkIndex(index);
            m_tracker.template set<1>(index);
            return m_data[index];
        }

        constexpr auto operator[](size_t index) const noexcept(!CheckMemory) -> T &
        {
            return at(index);
        }

        constexpr auto operator[](size_t index) noexcept(!CheckMemory) -> T &
        {
            checkIndex(index);
            m_tracker.template set<1>(index);
            return m_data[index];
        }

    public:
        [[nodiscard]] constexpr auto find_free() const noexcept -> size_t
        {
            return m_tracker.template find_if<0>();
        }

        [[nodiscard]] constexpr auto find_set() const noexcept -> size_t
        {
            return m_tracker.template find_if<1>();
        }

        [[nodiscard]] constexpr auto put(const T &value) noexcept(!CheckMemory)
            -> T &
        {
            auto index = find_free();
            checkIndex(index);
            T &elem = operator[](index);
            elem    = value;
            return elem;
        }

        [[nodiscard]] constexpr auto put_i(const T &value) noexcept(!CheckMemory)
            -> size_t
        {
            auto index = find_free();
            checkIndex(index);
            operator[](index) = value;
            return index;
        }

    public:
        constexpr auto operator=(Container &&other) noexcept -> Container &
        {
            m_tracker = std::move(other.m_tracker);
            memcpy(m_data, other.m_data, size());
            return *this;
        }

        constexpr auto operator=(const Container &other) noexcept -> Container &
        {
            if (this == &other) {
                return *this;
            }

            m_tracker = other.m_tracker;
            memcpy(m_data, other.m_data, size());
            return *this;
        }

    public:
        constexpr Container() noexcept(std::is_nothrow_constructible_v<T>) = default;

        constexpr Container(const Container &other) noexcept(
            std::is_nothrow_constructible_v<T>)
          : m_tracker(other.m_tracker)
        {
            memcpy(m_data, other.m_data, Size);
        }

        constexpr Container(Container &&other) noexcept(
            std::is_nothrow_constructible_v<T>)
          : m_tracker(std::move(other.m_tracker))
        {
            CERBLIB_UNROLL_N(4)
            for (size_t i = 0; i < Size; i++) {
                m_data[i] = std::move(other.m_data[i]);
            }
        }

        constexpr ~Container() noexcept = default;
    };
}// namespace cerb::gl

#endif /* CERBERUS_CONTAINER_HPP */
