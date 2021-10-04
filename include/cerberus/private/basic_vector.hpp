#ifndef CERBERUS_BASIC_VECTOR_HPP
#define CERBERUS_BASIC_VECTOR_HPP

#include <memory>
#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include <cerberus/algorithms.hpp>

namespace cerb::PRIVATE {
    template<typename T, typename Alloc = std::allocator<T>>
    class BasicVector
    {
    protected:
        using reference        = T &;
        using const_reference  = const T &;
        using value_type       = T;
        using const_value_type = const T;
        using size_type        = size_t;

        using ValueAllocator = Alloc;
        using ValueTraits    = std::allocator_traits<Alloc>;

        using pointer       = typename ValueTraits::pointer;
        using const_pointer = typename ValueTraits::const_pointer;

    private:
        ValueAllocator m_allocator{};
        pointer m_data{ ValueTraits::allocate(m_allocator, 4) };
        size_type m_size{ 0 };
        size_type m_capacity{ 4 };

        constexpr static bool isVector = true;

    public:
        [[nodiscard]] constexpr auto data() const noexcept -> pointer
        {
            return m_data;
        }

        [[nodiscard]] constexpr auto size() const noexcept -> size_type
        {
            return m_size;
        }

        [[nodiscard]] constexpr auto capacity() const noexcept -> size_type
        {
            return m_capacity;
        }

    protected:
        constexpr auto check_size() -> void
        {
            if (size() >= capacity()) {
                pointer new_buffer =
                    ValueTraits::allocate(m_allocator, capacity() * 2U);

                if constexpr (std::is_nothrow_move_constructible_v<value_type>) {
                    raw_move(new_buffer, m_data, m_data + m_size);
                } else {
                    raw_copy(new_buffer, m_data, m_data + m_size);
                }

                std::destroy_n(m_data, m_size);
                m_data = new_buffer;
                m_capacity *= 2U;
            }
        }

    protected:
        class CERBLIB_TRIVIAL iterator
        {
            pointer m_p;

        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type   = ptrdiff_t;

        public:
            constexpr auto operator++() -> iterator &
            {
                ++m_p;
                return *this;
            }

            constexpr auto operator++(int) -> iterator
            {
                auto copy = *this;
                operator++();
                return copy;
            }

            constexpr auto operator--() -> iterator &
            {
                --m_p;
                return *this;
            }

            constexpr auto operator--(int) -> iterator
            {
                auto copy = *this;
                operator--();
                return copy;
            }

            constexpr auto operator+(size_t offset) -> iterator
            {
                return iterator(m_p + offset);
            }

            constexpr auto operator*() const -> reference
            {
                return *m_p;
            }

            constexpr auto operator->() const noexcept -> pointer
            {
                return m_p;
            }

            [[nodiscard]] explicit constexpr operator pointer() const noexcept
            {
                return m_p;
            }

        public:
            constexpr auto operator==(const iterator &) const noexcept
                -> bool                                                 = default;
            constexpr auto operator<=>(const iterator &) const noexcept = default;

        public:
            constexpr auto operator=(const iterator &) noexcept
                -> iterator      & = default;
            constexpr auto operator=(iterator &&) noexcept -> iterator & = default;

        public:
            constexpr iterator() noexcept  = default;
            constexpr ~iterator() noexcept = default;

            constexpr iterator(iterator &&) noexcept      = default;
            constexpr iterator(const iterator &) noexcept = default;

            constexpr explicit iterator(pointer p) noexcept : m_p(p)
            {}
        };

        using const_iterator         = const iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = const reverse_iterator;

        [[nodiscard]] constexpr auto begin() const noexcept -> iterator
        {
            return iterator(m_data);
        }

        [[nodiscard]] constexpr auto end() const noexcept -> iterator
        {
            return iterator(m_data + m_size);
        }

        [[nodiscard]] constexpr auto rbegin() const noexcept -> reverse_iterator
        {
            return reverse_iterator(end());
        }

        [[nodiscard]] constexpr auto rend() const noexcept -> reverse_iterator
        {
            return reverse_iterator(begin());
        }

        [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator
        {
            return const_iterator(data());
        }

        [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator
        {
            return const_iterator(data() + size());
        }

        [[nodiscard]] constexpr auto crbegin() const noexcept
            -> const_reverse_iterator
        {
            return const_reverse_iterator(cbegin());
        }

        [[nodiscard]] constexpr auto crend() const noexcept -> const_reverse_iterator
        {
            return const_reverse_iterator(cend());
        }

    protected:
        constexpr auto push_back(value_type &&value) -> void
        {
            check_size();
            ValueTraits::construct(
                m_allocator, m_data + (m_size++), std::move(value));
        }

        constexpr auto push_back(const_reference value) -> void
        {
            check_size();
            ValueTraits::construct(m_allocator, m_data + (m_size++), value);
        }

        template<typename... Ts>
        constexpr auto emplace_back(Ts &&...args) -> void
        {
            check_size();
            ValueTraits::construct(m_allocator, m_data + (m_size++), args...);
        }

        constexpr auto back() -> reference
        {
            return m_data[m_size - 1];
        }

        constexpr auto back() const -> reference
        {
            return m_data[m_size - 1];
        }

        constexpr auto pop_back() -> void
        {
            ValueTraits::destroy(m_allocator, &m_data[--m_size]);
        }

    protected:
        constexpr auto insert(size_type index, const_reference value)
        {
            check_size();
            index = cerb::min(index, size());

            if (size() > 0) {
                CERBLIB_UNROLL_N(2)
                for (long long i = size(); i > static_cast<long long>(index); --i) {
                    if constexpr (std::is_nothrow_move_constructible_v<T>) {
                        m_data[i] = cerb::move(m_data[i - 1]);
                    } else {
                        m_data[i] = m_data[i - 1];
                    }
                }
            }
            m_data[index] = value;
            ++m_size;
        }

        constexpr auto
            insert(size_type index, const std::initializer_list<T> &values)
        {
            index           = cerb::min(index, size());
            size_type elems = static_cast<ptrdiff_t>(values.size());

            m_size += elems;
            check_size();

            if (size() > 0) {
                CERBLIB_UNROLL_N(2)
                for (size_type i = size(); i > index; --i) {
                    if constexpr (std::is_nothrow_move_constructible_v<T>) {
                        m_data[i] = cerb::move(m_data[i - elems]);
                    } else {
                        m_data[i] = m_data[i - elems];
                    }
                }
            }

            CERBLIB_UNROLL_N(2)
            for (const auto &elem : values) {
                m_data[index++] = elem;
            }
        }

        template<typename Iterator>
        constexpr auto insert(size_type index, Iterator first, Iterator last)
        {
            index           = cerb::min(index, size());
            size_type elems = static_cast<ptrdiff_t>(last - first);

            m_size += elems;
            check_size();

            if (size() > 0) {
                CERBLIB_UNROLL_N(2)
                for (size_type i = size(); i > index; --i) {
                    if constexpr (std::is_nothrow_move_constructible_v<T>) {
                        m_data[i] = cerb::move(m_data[i - elems]);
                    } else {
                        m_data[i] = m_data[i - elems];
                    }
                }
            }

            CERBLIB_UNROLL_N(2)
            for (; first != last; ++first) {
                m_data[index++] = *first;
            }
        }

        constexpr auto erase(pointer first)
        {
            size_type i = static_cast<ptrdiff_t>(first - m_data);

            if (i >= size() || size() == 0) {
                [[unlikely]];
                return;
            }

            CERBLIB_UNROLL_N(2)
            for (; i < size() - 1; ++i) {
                if constexpr (std::is_nothrow_move_constructible_v<T>) {
                    m_data[i] = std::move(m_data[i + 1]);
                } else {
                    m_data[i] = m_data[i + 1];
                }
            }

            --m_size;
            ValueTraits::destroy(m_allocator, m_data + i);
        }

        constexpr auto erase(iterator first)
        {
            return erase(static_cast<pointer>(first));
        }

        constexpr auto erase(pointer first, pointer last)
        {
            size_type elems = static_cast<ptrdiff_t>(last - first);
            size_type i     = static_cast<ptrdiff_t>(first - m_data);

            if (i + elems > size() || size() == 0) [[unlikely]] {
                return;
            }

            CERBLIB_UNROLL_N(2)
            for (; i < size() - elems; ++i) {
                if constexpr (std::is_nothrow_move_constructible_v<T>) {
                    m_data[i] = std::move(m_data[i + elems]);
                } else {
                    m_data[i] = m_data[i + elems];
                }
            }

            m_size -= elems;
            std::destroy_n(m_data + i, elems);
        }

        constexpr auto erase(iterator first, iterator last)
        {
            return erase(static_cast<pointer>(first), static_cast<pointer>(last));
        }

    protected:
        constexpr friend auto
            operator==(const BasicVector &lhs, const BasicVector &rhs)
        {
            if (lhs.size() != rhs.size()) {
                return false;
            }

            CERBLIB_UNROLL_N(2)
            for (size_t i = 0; i < lhs.size(); ++i) {
                if (lhs.data()[i] != rhs.data()[i]) {
                    return false;
                }
            }

            return true;
        }

        constexpr friend auto
            operator!=(const BasicVector &lhs, const BasicVector &rhs)
        {
            if (lhs.size() != rhs.size()) {
                return true;
            }

            CERBLIB_UNROLL_N(2)
            for (size_t i = 0; i < lhs.m_size; ++i) {
                if (lhs.data()[i] == rhs.data()[i]) {
                    return false;
                }
            }

            return true;
        }

        constexpr friend auto
            operator<(const BasicVector &lhs, const BasicVector &rhs)
        {
            if (lhs.size() == 0) {
                return true;
            }

            CERBLIB_UNROLL_N(2)
            for (size_t i = 0; i < cerb::min(lhs.size(), rhs.size()); ++i) {
                if (lhs.data()[i] < rhs.data()[i]) {
                    return true;
                }
                if (lhs.m_data[i] > rhs.data()[i]) {
                    return false;
                }
            }

            return lhs.size() < rhs.size();
        }

        constexpr friend auto
            operator<=(const BasicVector &lhs, const BasicVector &rhs)
        {
            return operator<(lhs, rhs) || operator==(lhs, rhs);
        }

        constexpr friend auto
            operator>(const BasicVector &lhs, const BasicVector &rhs)
        {
            if (lhs.size() == 0) {
                return false;
            }

            CERBLIB_UNROLL_N(2)
            for (size_t i = 0; i < cerb::min(lhs.size(), rhs.size()); ++i) {
                if (lhs.data()[i] > rhs.data()[i]) {
                    return true;
                }
                if (lhs.data()[i] < rhs.data()[i]) {
                    return false;
                }
            }

            return lhs.size() > rhs.size();
        }

        constexpr friend auto
            operator>=(const BasicVector &lhs, const BasicVector &rhs)
        {
            return operator>(lhs, rhs) || operator==(lhs, rhs);
        }

    public:
        constexpr auto self() const -> const BasicVector &
        {
            return *this;
        }

    public:
        constexpr auto operator=(const BasicVector &other) -> BasicVector &
        {
            if (m_data == other.m_data) {
                return *this;
            }

            std::destroy(m_data, m_data + m_size);
            m_size = other.size();

            if (capacity() < other.size()) {
                ValueTraits::deallocate(m_allocator, m_data, m_capacity);
                m_capacity = other.capacity();
                m_data     = ValueTraits::allocate(m_allocator, m_capacity);
            }

            raw_copy(m_data, other.data(), other.data() + other.size());
            return *this;
        }

        constexpr auto operator=(BasicVector &&other) noexcept -> BasicVector &
        {
            m_data      = other.data();
            m_size      = other.size();
            m_capacity  = other.capacity();
            m_allocator = cerb::move(other.m_allocator);

            other.m_size     = 0;
            other.m_capacity = 4;
            std::construct_at(&other.m_allocator);
            other.m_data = ValueTraits::allocate(m_allocator, 4);

            return *this;
        }

        constexpr auto clear() -> void
        {
            std::destroy(begin(), end());
            m_size = 0;
        }

    public:
        constexpr BasicVector() = default;
        constexpr ~BasicVector()
        {
            CERBLIB_UNROLL_N(4)
            for (iterator i = begin(); i != end(); ++i) {
                i->~T();
            }
            ValueTraits::deallocate(m_allocator, m_data, m_capacity);
        }

        constexpr BasicVector(const BasicVector &other)
          : m_allocator(other.m_allocator), m_size(other.size()),
            m_capacity(other.capacity())
        {
            m_data = ValueTraits::allocate(m_allocator, m_capacity);
            raw_copy<T>(m_data, other.data(), other.data() + other.size());
        }

        constexpr BasicVector(BasicVector &&other) noexcept
          : m_allocator(cerb::move(other.m_allocator)), m_data(other.m_data),
            m_size(other.size()), m_capacity(other.capacity())
        {
            other.m_size     = 0;
            other.m_capacity = 4;
            other.m_data     = ValueTraits::allocate(other.m_allocator, 4);
        }

        constexpr BasicVector(const std::initializer_list<T> &args)
          : m_allocator(), m_data(nullptr), m_size(args.size()),
            m_capacity(args.size())
        {
            m_capacity = cmov<size_t>(m_capacity == 0, 1, m_capacity);

            m_data = ValueTraits::allocate(m_allocator, m_capacity);
            raw_copy(m_data, args.begin(), args.end());
        }

        constexpr BasicVector(size_t t_capacity)
          : m_allocator(), m_data(nullptr), m_capacity(t_capacity)
        {
            m_capacity = cmov<size_t>(m_capacity == 0, 1, m_capacity);
            m_data     = ValueTraits::allocate(m_allocator, m_capacity);
        }
    };
}// namespace cerb::PRIVATE

#endif /* CERBERUS_BASIC_VECTOR_HPP */
