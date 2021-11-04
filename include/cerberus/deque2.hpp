#ifndef CERBERUS_DEQUE2_HPP
#define CERBERUS_DEQUE2_HPP

#include <memory>
#include <cerberus/types.h>

namespace cerb::experimental {
    /**
     * Class which has approximately the same functionality as std::deque
     * @tparam T type of elements which are stored in the deque
     * @tparam Throwable throws error if set (true is set as default)
     * @tparam Alloc type of allocator (std::allocator is set as default)
     */
    template<typename T, bool Throwable = true, typename Alloc = std::allocator<T>>
    class Deque
    {
        static_assert(std::is_constructible_v<T> && std::is_destructible_v<T>);

        struct DequeNode
        {
            static constexpr size_t NodeSize = 16;
            using storage_t                  = std::array<T, NodeSize>;

            storage_t m_data;
            DequeNode *front{};
            DequeNode *back{};
            u8 first{ 0 };
            u8 last{ 0 };

            /**
             * initialize local array with default objects if deque is used in
             * constant expression
             * @return
             */
            constexpr auto init() -> void
            {
                if (std::is_constant_evaluated()) {
                    CERBLIB_UNROLL_N(4)
                    for (auto &elem : m_data) {
                        std::construct_at(&elem);
                    }
                }
            }

            /**
             * destroys objects in array if deque is not used in constant expression
             * @return
             */
            constexpr auto clear() -> void
            {
                if (!std::is_constant_evaluated()) {
                    std::destroy_n(&m_data[first], last - first);
                }
            }

            /**
             * initializes end node of deque
             * @param current_last_node m_end of deque
             * @param new_last_node new node to add
             * @return
             */
            constexpr static auto
                init_back(DequeNode *current_last_node, DequeNode *new_last_node)
                    -> DequeNode *
            {
                current_last_node->front = new_last_node;
                new_last_node->back      = current_last_node;
                return new_last_node;
            }

            /**
             * initializes first node of deque
             * @param current_first_node m_begin of deque
             * @param new_first_node new node to add
             * @return
             */
            constexpr static auto
                init_front(DequeNode *current_first_node, DequeNode *new_first_node)
                    -> DequeNode *
            {
                new_first_node->front    = current_first_node;
                current_first_node->back = new_first_node;
                return new_first_node;
            }

            constexpr auto operator=(const DequeNode &other) -> DequeNode &
            {
                this->clear();
                first = other.first;
                last  = other.last;
                if (std::is_constant_evaluated()) {
                    this->init();
                    cerb::copy(
                        m_data.begin(), other.m_data.begin(), other.m_data.end());
                } else {
                    cerb::construct(
                        m_data.begin() + first,
                        other.m_data.begin() + first,
                        other.m_data.begin() + last);
                }
                return *this;
            }

            /**
             *
             * @param first_ 0 if node is at the end of a deque, NodeSize if node is
             * at the beginning of the deque, NodeSize / 2 if it is root node of
             * deque
             * @param last_ NodeSize if node is at the beginning of the deque, 0 if
             * node is at the end of the deque, NodeSize / 2 if it is root node
             */
            constexpr DequeNode(u8 first_, u8 last_) : first(first_), last(last_)
            {
                this->init();
            }

            constexpr DequeNode(const DequeNode &other)
              : first(other.first), last(other.last)
            {
                if (std::is_constant_evaluated()) {
                    this->init();
                    cerb::copy(
                        m_data.begin(), other.m_data.begin(), other.m_data.end());
                    return;
                }
                cerb::construct(
                    m_data.begin() + first,
                    other.m_data.begin() + first,
                    other.m_data.begin() + last);
            }
        };

        /**
         * Throws exception of condition is false and Deque is allowed to throw
         * @param condition if false throws exception
         * @param message message for the exception
         * @return
         */
        static constexpr auto assert(bool condition, const char *message)
        {
            if constexpr (Throwable) {
                if (!condition) {
                    throw std::runtime_error(message);
                }
            }
        }

    public:
        using Node         = DequeNode;
        using ConstNode    = const Node;
        using NodePtr      = Node *;
        using ConstNodePtr = const Node *;
        using NodeRef      = Node &;
        using ConstNodeRef = const Node &;

        using NodeAllocator =
            typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
        using NodeTraits                 = std::allocator_traits<NodeAllocator>;
        static constexpr size_t NodeSize = DequeNode::NodeSize;

    private:
        size_t m_size{};
        NodePtr m_begin{};
        NodePtr m_end{};
        NodeAllocator m_allocator{};

    public:
        class iterator
        {
            NodePtr m_node{};
            u8 m_index{};

        public:
            using value_type        = T;
            using reference         = T &;
            using pointer           = T *;
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type   = ptrdiff_t;

            constexpr auto operator++() -> iterator &
            {
                ++m_index;

               if (m_index == NodeSize) {
                    m_index = 0;
                    m_node  = m_node->front;
                }

                return *this;
            }

            constexpr auto operator++(int) -> iterator
            {
                auto save = *this;
                operator++();
                return save;
            }

            constexpr auto operator--() -> iterator &
            {
                if (m_index != 0) {
                    --m_index;
                } else {
                    m_index = NodeSize - 1;
                    m_node  = m_node->back;
                }

                return *this;
            }

            constexpr auto operator*() const -> T&
            {
                return m_node->m_data[m_index];
            }

            constexpr auto operator==(const iterator &) const -> bool = default;
            constexpr auto operator<=>(const iterator &) const
                -> std::strong_ordering = default;

            constexpr iterator(u8 index, NodePtr node) : m_node(node), m_index(index)
            {}

            constexpr ~iterator() = default;
        };

        using reverse_iterator = std::reverse_iterator<iterator>;

        constexpr auto begin() const noexcept -> iterator
        {
            return iterator(m_begin->first, m_begin);
        }

        constexpr auto end() const noexcept -> iterator
        {
            return iterator(m_end->last, m_end);
        }

        constexpr auto rbegin() const noexcept -> reverse_iterator
        {
            return reverse_iterator(begin());
        }

        constexpr auto rend() const noexcept -> reverse_iterator
        {
            return reverse_iterator(iterator(m_end->last + 1, m_end));
        }

        /**
         * Allows to get size of the deque outside of the class
         * @return number of elements in the deque
         */
        CERBLIB_DECL auto size() const noexcept -> size_t
        {
            return m_size;
        }

        /**
         * add a new item to the end of the deque
         * @tparam Ts
         * @param args
         * @return
         */
        template<typename... Ts>
        constexpr auto emplace_back(Ts &&...args) -> void
        {
            // if last node is full at the end we need to create a new node
            if (m_end->last == NodeSize) {
                [[unlikely]];// this case is unlikely to happen
                NodePtr new_node;

                // there can be node if front of m_end, so we need to check for it
                if (m_end->front != nullptr) {
                    new_node = m_end->front;
                } else {
                    // if there is not allocated node we need to allocate one
                    new_node = NodeTraits::allocate(m_allocator, 1);
                }

                // it is end node of the deque, so it must be initialized with zeros
                NodeTraits::construct(m_allocator, new_node, 0, 0);
                m_end = Node::init_back(m_end, new_node);
            }

            ++m_size;
            if (std::is_constant_evaluated()) {
                // in constant expression array contains initialized objects, so we
                // need to call copy operator
                m_end->m_data[m_end->last++] = T(args...);
            } else {
                std::construct_at(&m_end->m_data[m_end->last++], args...);
            }
        }

        /**
         * add a new item to the end of the deque
         * @param value
         * @return
         */
        constexpr auto push_back(const T &value) -> void
        {
            return this->template emplace_back<const T &>(value);
        }

        /**
         * add a new item to the end of the deque
         * @param value
         * @return
         */
        constexpr auto push_back(T &&value) -> void
        {
            return this->template emplace_back<T &&>(std::move(value));
        }

        /**
         * add a new item in the front of the deque
         * @tparam Ts
         * @param args
         * @return
         */
        template<typename... Ts>
        constexpr auto emplace_front(Ts &&...args)
        {
            // if first node is full at the beginning we need to create new node
            if (m_begin->first == 0) {
                [[unlikely]];// this case is unlikely to happen
                NodePtr new_node;

                // there can be node at the before m_begin, so we need to check for
                // it
                if (m_begin->back != nullptr) {
                    new_node = m_begin->back;
                } else {
                    // if there is not allocated node we need to allocate one
                    new_node = NodeTraits::allocate(m_allocator, 1);
                }

                // this node is in front of the deque, so it must be initialized with
                // NodeSize
                NodeTraits::construct(m_allocator, new_node, NodeSize, NodeSize);
                m_begin = Node::init_front(m_begin, new_node);
            }

            ++m_size;
            if (std::is_constant_evaluated()) {
                // in constant expression array contains initialized objects, so we
                // need to call copy operator
                m_begin->m_data[--m_begin->first] = T(args...);
            } else {
                std::construct_at(&m_begin->m_data[--m_begin->first], args...);
            }
        }

        /**
         * add a new item in the front of the deque
         * @param value
         * @return
         */
        constexpr auto push_front(const T &value) -> void
        {
            return this->template emplace_front<const T &>(value);
        }

        /**
         * add a new item in the front of the deque
         * @param value
         * @return
         */
        constexpr auto push_front(T &&value) -> void
        {
            return this->template emplace_front<T &&>(std::move(value));
        }

        /**
         * remove element from the end of the deque
         * @return
         */
        constexpr auto pop_back() -> void
        {
            if (m_end->last == 0) {
                this->assert(false, "Unable to pop from empty deque");
                return;
            }

            --m_size;
            // we won't destroy object in constant expression
            if (!std::is_constant_evaluated()) {
                NodeTraits::destroy(m_allocator, &m_end->m_data[--m_end->last]);
            }

            // if DequeNode does not contain any elements it is not end node
            if (m_end->last == 0 && m_end->back != nullptr) {
                m_end = m_end->back;
            }
        }

        /**
         * remove element from the beginning of the deque
         * @return
         */
        constexpr auto pop_front() -> void
        {
            if (m_begin->first == NodeSize) {
                this->assert(false, "Unable to pop from empty deque");
                return;
            }

            --m_size;
            // we won't destroy object in constant expression
            if (!std::is_constant_evaluated()) {
                NodeTraits::destroy(m_allocator, &m_begin->m_data[m_begin->first++]);
            }

            // if DequeNode does not contain any elements it is not first node
            if (m_begin->first == NodeSize && m_begin->front != nullptr) {
                m_begin = m_begin->front;
            }
        }

        /**
         * Return last element of the deque
         * @return
         */
        CERBLIB_DECL auto back() const -> T &
        {
            this->assert(
                m_end->last != m_end->first,
                "Unable to get last element from deque because it is empty");
            return m_end->m_data[m_end->last - 1];
        }

        /**
         * Return first element of the deque
         * @return
         */
        CERBLIB_DECL auto front() const -> T &
        {
            this->assert(
                m_begin->last != m_begin->first,
                "Unable to get first element from deque because it is empty");
            return m_begin->m_data[m_begin->first];
        }

    public:
        constexpr auto operator=(Deque &&other) noexcept -> Deque &
        {
            std::swap(m_size, other.m_size);
            std::swap(m_begin, other.m_begin);
            std::swap(m_end, other.m_end);
            std::swap(m_allocator, other.m_allocator);
            return *this;
        }

        constexpr auto operator=(const Deque &other) -> Deque &
        {
            // check for copy on itself
            if (this == &other) {
                return *this;
            }
            NodePtr node       = other.m_begin;
            NodePtr begin_node = m_begin;

            // we need to find real first node of the deque
            CERBLIB_UNROLL_N(2)
            while (begin_node->back != nullptr) {
                begin_node = begin_node->back;
            }

            // after coping begin_node is going to be the first node of deque
            m_begin = begin_node;

            CERBLIB_UNROLL_N(2)
            while (node != nullptr) {
                *begin_node = *node;

                if (begin_node->front == nullptr && node->front != nullptr) {
                    // if there are not enough nodes in this deque we need to add
                    // some more
                    auto *new_node = NodeTraits::allocate(m_allocator, 1);
                    NodeTraits::construct(m_allocator, new_node, 0, 0);
                    begin_node->front = new_node;
                }

                node       = node->front;
                m_end      = begin_node;
                begin_node = begin_node->front;
            }

            if (begin_node != nullptr) {
                // if there are more nodes in this deque we need to clear them
                CERBLIB_UNROLL_N(2)
                while (begin_node != nullptr) {
                    begin_node->clear();
                    begin_node = begin_node->front;
                }
            }

            // updates the size of the deque
            m_size = other.size();
            return *this;
        }

    public:
        constexpr Deque()
        {
            m_begin = m_end = NodeTraits::allocate(m_allocator, 1);
            NodeTraits::construct(m_allocator, m_begin, NodeSize / 2, NodeSize / 2);
        }

        constexpr Deque(const Deque &other)
          : m_size(other.size()), m_allocator(other.m_allocator)
        {
            // allocates first node and copies elements to it
            m_begin = m_end = NodeTraits::allocate(m_allocator, 1);
            NodeTraits::construct(m_allocator, m_begin, *other.m_begin);

            // if there is more than one node we need to allocate some more and copy
            // elements to them
            CERBLIB_UNROLL_N(2)
            for (NodePtr i = other.m_begin->front; i != nullptr; i = i->front) {
                NodePtr new_node = NodeTraits::allocate(m_allocator, 1);
                NodeTraits::construct(m_allocator, new_node, *i);
                m_end->front   = new_node;
                new_node->back = m_end;
                m_end          = new_node;
            }
        }

        constexpr Deque(Deque &&other) noexcept
          : m_size(other.size()), m_begin(other.m_begin), m_end(other.m_end),
            m_allocator(other.m_allocator)
        {
            other.m_size  = 0;
            other.m_begin = other.m_end = NodeTraits::allocate(other.m_allocator, 1);
            NodeTraits::construct(
                other.m_allocator, other.m_begin, NodeSize / 2, NodeSize / 2);
        }

        constexpr ~Deque()
        {
            CERBLIB_UNROLL_N(2)
            while (m_begin->back != nullptr) {
                m_begin = m_begin->back;
            }

            CERBLIB_UNROLL_N(2)
            while (m_begin != nullptr) {
                NodePtr next = m_begin->front;
                m_begin->clear();
                NodeTraits::deallocate(m_allocator, m_begin, 1);
                m_begin = next;
            }
        }
    };
}// namespace cerb::experimental

#endif /* CERBERUS_DEQUE2_HPP */
