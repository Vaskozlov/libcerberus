#ifndef CERBERUS_DEQUE_HPP
#define CERBERUS_DEQUE_HPP

#include <memory>
#include <cerberus/types.h>
#include <cerberus/algorithms.hpp>

namespace cerb {

    template<
        typename T, size_t Size = 8, bool MayThrow = true,
        typename Alloc = std::allocator<T>>
    class CERBLIB_TRIVIAL Deque
    {
        static_assert(Size % 2 == 0);

        struct DequeNode
        {
            std::array<T, Size> data;
            DequeNode *front;
            DequeNode *back;
            u8 first;
            u8 last;

        public:
            constexpr auto init() -> void
            {
                first = last = Size / 2;
                front = back = nullptr;
            }

            constexpr auto init_back(DequeNode *previous) -> void
            {
                front = nullptr;
                back  = previous;
                first = last = 0;

                previous->front = this;
            }

            constexpr auto init_forward(DequeNode *next) -> void
            {
                front = next;
                back  = nullptr;
                first = last = Size;

                next->back = this;
            }

        public:
            auto operator=(DequeNode &&) = delete;

            constexpr auto operator=(const DequeNode &other) noexcept -> DequeNode &
            {
                if (this == &other) {
                    return *this;
                }

                back  = other.back;
                front = other.front;
                first = other.first;
                last  = other.last;
                memcpy(data, other.data, Size);

                return *this;
            }

        public:
            constexpr DequeNode() noexcept  = delete;
            constexpr ~DequeNode() noexcept = delete;
            DequeNode(DequeNode &&other)    = delete;

            constexpr DequeNode(const DequeNode &other) noexcept
              : front(other.front), back(other.back), first(other.first),
                last(other.last)
            {
                memcpy(data, other.data, Size);
            }
        };

    private:
        using Node         = DequeNode;
        using NodePtr      = DequeNode *;
        using ConstNodePtr = const DequeNode *;

        using NodeAllocator =
            typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
        using NodeTraits = std::allocator_traits<NodeAllocator>;

    public:
        class iterator
        {
            NodePtr m_node;
            u8 m_index;

        public:
            using value_type        = T;
            using reference         = T &;
            using pointer           = T *;
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type   = ptrdiff_t;

        public:
            constexpr auto operator++() -> iterator &
            {
                if (m_index != Size) {
                    ++m_index;
                } else {
                    m_index = 0;
                    m_node  = m_node->front;
                }

                return *this;
            }

            constexpr auto operator--() -> iterator &
            {
                if (m_index != 0) {
                    --m_index;
                } else {
                    m_index = Size;
                    m_node  = m_node->back;
                }

                return *this;
            }

            constexpr auto operator==(const iterator &other) const -> bool = default;
            constexpr auto operator<=>(const iterator &other) const
                -> std::strong_ordering = default;

        public:
            constexpr auto operator*() -> T &
            {
                return m_node->data[m_index];
            }

        public:
            constexpr auto operator=(const iterator &) -> iterator & = default;
            constexpr auto operator=(iterator &&) noexcept -> iterator & = default;

        public:
            constexpr iterator()  = default;
            constexpr ~iterator() = default;

            constexpr iterator(const iterator &) = default;
            constexpr iterator(iterator &&) noexcept = default;

            constexpr explicit iterator(u8 index, NodePtr node)
              : m_index(index), m_node(node)
            {}
        };

    public:
        using reverse_iterator = std::reverse_iterator<iterator>;

    public:
        constexpr auto begin() -> iterator
        {
            return iterator(m_begin->first, m_begin);
        }

        constexpr auto begin() const -> iterator
        {
            return iterator(m_begin->first, m_begin);
        }

        constexpr auto end() -> iterator
        {
            return iterator(m_end->last, m_end);
        }

        constexpr auto end() const -> iterator
        {
            return iterator(m_end->last, m_end);
        }

        constexpr auto rbegin() -> reverse_iterator
        {
            return reverse_iterator(end());
        }

        constexpr auto rbegin() const -> reverse_iterator
        {
            return reverse_iterator(end());
        }

        constexpr auto rend() -> reverse_iterator
        {
            return reverse_iterator(begin());
        }

        constexpr auto rend() const -> reverse_iterator
        {
            return reverse_iterator(begin());
        }

    private:
        NodeAllocator m_allocator{};
        NodePtr m_begin{ NodeTraits::allocate(m_allocator, 1) };
        NodePtr m_end{ m_begin };
        size_t m_size{ 0 };

    private:
        constexpr auto get_back() -> NodePtr
        {
            if (m_end->last == Size) [[unlikely]] {
                NodePtr new_node = NodeTraits::allocate(m_allocator, 1);
                new_node->init_back(m_end);
                m_end = new_node;
            }
            return m_end;
        }

        constexpr auto get_front() -> NodePtr
        {
            if (m_begin->first == 0) {
                NodePtr new_node = NodeTraits::allocate(m_allocator, 1);
                new_node->init_forward(m_begin);
                m_begin = new_node;
            }
            return m_begin;
        }

    public:
        constexpr auto push_back(T &&value) -> void
        {
            ++m_size;
            NodePtr node = get_back();

            node->data[node->last++] = value;
        }

        constexpr auto push_back(const T &value) -> void
        {
            ++m_size;
            NodePtr node = get_back();

            node->data[node->last++] = value;
        }

        template<typename... Ts>
        constexpr auto emplace_back(Ts &&...values) -> void
        {
            ++m_size;
            NodePtr node = get_back();
            std::construct_at(&node->data[node->last++], values...);
        }

        constexpr auto push_front(T &&value) -> void
        {
            ++m_size;
            NodePtr node = get_front();

            node->data[--node->first] = value;
        }

        constexpr auto push_front(const T &value) -> void
        {
            ++m_size;
            NodePtr node = get_front();

            node->data[--node->first] = value;
        }

        template<typename... Ts>
        constexpr auto emplace_front(Ts &&...values) -> void
        {
            ++m_size;
            NodePtr node = get_front();
            std::construct_at(&node->data[--node->first], values...);
        }

    public:
        constexpr auto pop_back() -> void
        {
            if (m_end->last > m_end->first) {
                --m_size;
                cerb::destroy(m_end->data[--m_end->last]);

                if (m_end->last == 0) {
                    NodePtr last_node = m_end->back;
                    NodeTraits::deallocate(m_allocator, m_end, 1);
                    m_end        = last_node;
                    m_end->front = nullptr;
                }
            }
        }

        constexpr auto pop_front() -> void
        {
            --m_size;
            if (m_begin->first < m_end->last) {
                cerb::destroy(m_end->data[m_end->first++]);

                if (m_begin->first == Size) {
                    NodePtr last_node = m_begin->front;
                    NodeTraits::deallocate(m_allocator, m_begin, 1);
                    m_begin       = last_node;
                    m_begin->back = nullptr;
                }
            }
        }

    public:
        constexpr auto back() -> T &
        {
            if constexpr (MayThrow) {
                if (m_size == 0) [[unlikely]] {
                    throw std::out_of_range("cerb::Deque does not have back elem");
                }
            }
            return m_end->data[m_end->last - 1];
        }

        constexpr auto back() const -> T &
        {
            if constexpr (MayThrow) {
                if (m_size == 0) [[unlikely]] {
                    throw std::out_of_range("cerb::Deque does not have back elem");
                }
            }
            return m_end->data[m_end->last - 1];
        }

        constexpr auto front() -> T &
        {
            if constexpr (MayThrow) {
                if (m_size == 0) [[unlikely]] {
                    throw std::out_of_range("cerb::Deque does not have back elem");
                }
            }
            return m_begin->data[m_begin->first];
        }

        constexpr auto front() const -> T &
        {
            if constexpr (MayThrow) {
                if (m_size == 0) [[unlikely]] {
                    throw std::out_of_range("cerb::Deque does not have back elem");
                }
            }
            return m_begin->data[m_begin->first];
        }

        constexpr auto at(size_t index) const -> T &
        {
            if constexpr (MayThrow) {
                if (index >= m_size) [[unlikely]] {
                    throw std::out_of_range(
                        "cerb::Deque does not have item at given index");
                }
            }

            auto node = m_begin;
            index += node->first;

            CERBLIB_UNROLL_N(1)
            for (size_t i = 0; i < index / Size; ++i) {
                node = node->front;
            }

            return node->data[index % Size];
        }

        constexpr auto operator[](size_t index) const -> T &
        {
            return at(index);
        }

        constexpr auto operator[](size_t index) -> T &
        {
            if constexpr (MayThrow) {
                if (index >= m_size) [[unlikely]] {
                    throw std::out_of_range(
                        "cerb::Deque does not have item at given index");
                }
            }

            auto node = m_begin;
            index += node->first;

            CERBLIB_UNROLL_N(1)
            for (size_t i = 0; i < index / Size; ++i) {
                node = node->front;
            }

            return node->data[index % Size];
        }

    public:
        constexpr Deque()
        {
            m_begin->init();
        }

        constexpr ~Deque()
        {
            NodePtr node = m_begin;

            CERBLIB_UNROLL_N(1)
            while (node != nullptr) {
                NodePtr next = node->front;
                std::destroy_n(&node->data[node->first], node->last - node->first);
                NodeTraits::deallocate(m_allocator, node, 1);
                node = next;
            }
        }
    };
}// namespace cerb

#endif /* CERBERUS_DEQUE_HPP */
