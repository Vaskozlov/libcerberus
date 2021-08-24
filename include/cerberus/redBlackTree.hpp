#ifndef CERBERUS_REDBLACKTREE_HPP
#define CERBERUS_REDBLACKTREE_HPP

#include <memory>
#include <functional>
#include <cerberus/pair.hpp>
#include <cerberus/operators.hpp>
#include <cerberus/algorithms.hpp>

namespace cerb::PRIVATE {
    namespace gl {
        template<typename T, size_t Size>
        struct BasicSet
        {
            using value_type       = T;
            using const_value_type = const T;
            using storage_t        = std::array<value_type, Size>;

            using iterator       = typename storage_t::iterator;
            using const_iterator = typename storage_t::const_iterator;

            using reverse_iterator = typename storage_t::reverse_iterator;
            using const_reverse_iterator =
                typename storage_t::const_reverse_iterator;

        protected:
            size_t m_size{ 0 };
            storage_t m_data{};

        public:
            constexpr auto size() const -> size_t
            {
                return m_size;
            }

        public:
            constexpr auto begin() noexcept -> iterator
            {
                return m_data.begin();
            }

            constexpr auto end() noexcept -> iterator
            {
                return m_data.begin() + m_size;
            }

            constexpr auto begin() const noexcept -> const_iterator
            {
                return m_data.begin();
            }

            constexpr auto end() const noexcept -> const_iterator
            {
                return m_data.begin() + m_size;
            }

            constexpr auto rbegin() noexcept -> reverse_iterator
            {
                return reverse_iterator(begin());
            }

            constexpr auto rend() noexcept -> reverse_iterator
            {
                return reverse_iterator(end());
            }

            constexpr auto rbegin() const noexcept -> const_reverse_iterator
            {
                return const_reverse_iterator(begin());
            }

            constexpr auto rend() const noexcept -> const_reverse_iterator
            {
                return const_reverse_iterator(end());
            }

            constexpr auto cbegin() noexcept -> const_iterator
            {
                return m_data.cbegin();
            }

            constexpr auto cend() noexcept -> const_iterator
            {
                return m_data.cbegin() + m_size;
            }

            constexpr auto crbegin() noexcept -> const_reverse_iterator
            {
                return const_reverse_iterator(cbegin());
            }

            constexpr auto crend() noexcept -> const_reverse_iterator
            {
                return const_reverse_iterator(cend());
            }

        public:
            constexpr auto self() noexcept -> BasicSet &
            {
                return *this;
            }

            constexpr auto self() const noexcept -> const BasicSet &
            {
                return *this;
            }

            constexpr auto clear() noexcept -> void
            {
                m_size = 0;
            }

        public:
            constexpr auto operator=(const BasicSet &other) noexcept -> BasicSet &
            {
                if (this == &other) {
                    return *this;
                }

                m_size = other.m_size;
                memcpy(m_data, other.m_data, m_size);
                return *this;
            }

            constexpr auto operator=(BasicSet &&other) noexcept -> BasicSet &
            {
                m_size = other.m_size;
                memcpy(m_data, other.m_data, m_size);
                return *this;
            }

        public:
            constexpr BasicSet() noexcept = default;

            constexpr BasicSet(const BasicSet &other) noexcept : m_size(other.m_size)
            {
                memcpy(m_data, other.m_data, m_size);
            }

            constexpr BasicSet(BasicSet &&other) noexcept : m_size(other.m_size)
            {
                memcpy(m_data, other.m_data, m_size);
            }

            constexpr BasicSet(
                const std::initializer_list<const_value_type> &args) noexcept
            {
                CERBLIB_UNROLL_N(2)
                for (const auto &elem : args) {
                    m_data[m_size++] = std::move(elem);
                }
            }

            template<typename... Ts>
            explicit constexpr BasicSet(Ts &&...args) noexcept
            {
                forEach<false>(
                    [&](const auto &elem) { m_data[m_size++] = std::move(elem); },
                    args...);
            }

            constexpr ~BasicSet() noexcept = default;
        };
    }// namespace gl

    template<
        typename T, typename Compare = less<T>, typename Alloc = std::allocator<T>>
    class RBTree
    {
        enum RBTreeNodeColor : bool
        {
            RED   = false,
            BLACK = true
        };

        struct RBTreeNode
        {
            RBTreeNode *left, *right, *parent;
            T value;
            RBTreeNodeColor color;

        public:
            [[nodiscard]] constexpr auto operator==(const RBTreeNode &other) const
                -> bool
            {
                return value == other.value;
            }

            [[nodiscard]] constexpr auto operator==(const T &other) const -> bool
            {
                return value == other;
            }

            [[nodiscard]] constexpr auto operator<=>(const RBTreeNode &other) const
                -> std::strong_ordering
            {
                return value <=> other.value;
            }

            [[nodiscard]] constexpr auto operator<=>(const T &other) const
                -> std::strong_ordering
            {
                return value <=> other.value;
            }

        public:
            [[nodiscard]] constexpr auto isLeftChild() const
            {
                return this == parent->left;
            }

            [[nodiscard]] constexpr auto hasRedChild() const -> bool
            {
                return (left != nullptr && left->color == RED) ||
                       (right != nullptr && right->color == RED);
            }

            [[nodiscard]] constexpr auto hasChild() const -> bool
            {
                return left != nullptr || right != nullptr;
            }

            [[nodiscard]] constexpr auto uncle() -> RBTreeNode *
            {
                if (parent == nullptr || parent->parent == nullptr) [[unlikely]] {
                    return nullptr;
                } else if (isLeftChild()) {
                    return parent->parent->right;
                } else {
                    return parent->parent->left;
                }
            }

            [[nodiscard]] constexpr auto uncle() const -> RBTreeNode *
            {
                if (parent == nullptr || parent->parent == nullptr) [[unlikely]] {
                    return nullptr;
                } else if (isLeftChild()) {
                    return parent->parent->right;
                } else {
                    return parent->parent->left;
                }
            }

            [[nodiscard]] constexpr auto sibling() -> RBTreeNode *
            {
                if (parent == nullptr) [[unlikely]] {
                    return nullptr;
                } else if (isLeftChild()) {
                    return parent->right;
                } else {
                    return parent->left;
                }
            }

            constexpr auto swapColor(RBTreeNode *node) -> void
            {
                std::swap(color, node->color);
            }

            constexpr auto modeDown(RBTreeNode *node) -> void
            {
                if (parent != nullptr) [[likely]] {
                    if (isLeftChild()) {
                        parent->left = node;
                    } else [[unlikely]] {
                        parent->right = node;
                    }
                }

                node->parent = parent;
                parent       = node;
            }

        public:
            constexpr static auto init(RBTreeNode *node) -> void
            {
                node->color = RED;
                node->left = node->right = node->parent = nullptr;
            }

        public:
            constexpr auto operator=(RBTreeNode &&other) noexcept -> RBTree &
            {
                value  = move(other.value);
                left   = other.left;
                right  = other.right;
                parent = other.parent;
                color  = other.color;
            }

            constexpr auto operator=(const RBTreeNode &other) noexcept
                -> RBTreeNode &
            {
                value  = other.value;
                left   = other.left;
                right  = other.right;
                parent = other.parent;
                color  = other.color;
            }

        public:
            CERBLIB_DISABLE_WARNING("-Wreorder", "-Wreorder-ctor", 0)

            template<typename... Ts>
            constexpr explicit RBTreeNode(Ts &&...args) noexcept
              : value(args...), color(RED), left(nullptr), right(nullptr),
                parent(nullptr)
            {}

            constexpr explicit RBTreeNode(const T &t_value) noexcept
              : value(t_value), color(RED), left(nullptr), right(nullptr),
                parent(nullptr)
            {}

            constexpr explicit RBTreeNode(const T &&t_value) noexcept(
                std::is_nothrow_move_constructible_v<T>)
              : value(t_value), color(RED), left(nullptr), right(nullptr),
                parent(nullptr)
            {}

            constexpr RBTreeNode(RBTreeNode &&other) noexcept(
                std::is_nothrow_move_constructible_v<T>)
              : value(std::move(other.value)), color(other.color), left(other.left),
                right(other.right), parent(other.parent)
            {}

            constexpr RBTreeNode(const RBTreeNode &other) noexcept(
                std::is_nothrow_copy_constructible_v<T>)
              : value(other.value), color(other.color), left(other.left),
                right(other.right), parent(other.parent)
            {}

            CERBLIB_ENABLE_WARNING("-Wreorder-ctor", "-Wreorder-ctor", 0)

            constexpr ~RBTreeNode() noexcept = default;
        };

    protected:
        using Node         = RBTreeNode;
        using NodePtr      = RBTreeNode *;
        using ConstNodePtr = const RBTreeNode *;

        using NodeAllocator =
            typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
        using NodeTraits = std::allocator_traits<NodeAllocator>;

    private:
        NodePtr m_root{ nullptr };
        size_t m_size{ 0 };
        NodeAllocator m_allocator{};

    public:
        [[nodiscard]] constexpr auto size() const noexcept
        {
            return m_size;
        }

    private:
        constexpr auto leftRotate(NodePtr node) -> void
        {
            NodePtr new_parent = node->right;// new parent of the node

            if (node == m_root) [[unlikely]] {
                m_root = new_parent;// if node is root node we need to update root
            }

            node->modeDown(new_parent);
            node->right = new_parent->left;

            if (new_parent->left != nullptr) [[likely]] {
                new_parent->left->parent = node;
            }

            new_parent->left = node;// complete the rotation
        }

        constexpr auto rightRotate(NodePtr node) noexcept -> void
        {
            NodePtr new_parent = node->left;

            if (node == m_root) [[unlikely]] {
                m_root = new_parent;
            }

            node->modeDown(new_parent);
            node->left = new_parent->right;

            if (new_parent->right != nullptr) [[likely]] {
                new_parent->right->parent = node;
            }

            new_parent->right = node;
        }

        constexpr auto fixRedRed(NodePtr node) -> void
        {
            if (node == m_root) [[unlikely]] {
                node->color = BLACK;// root is always black
                return;
            }

            NodePtr parent      = node->parent;
            NodePtr grandparent = parent->parent;
            NodePtr uncle       = node->uncle();

            if (parent->color == RED) {
                if (uncle != nullptr && uncle->color == RED) {
                    parent->color = uncle->color = BLACK;
                    grandparent->color           = RED;
                    return fixRedRed(grandparent);
                }

                if (parent->isLeftChild()) {
                    if (node->isLeftChild()) {
                        parent->swapColor(grandparent);
                    } else {
                        leftRotate(parent);
                        node->swapColor(grandparent);
                    }
                    return rightRotate(grandparent);
                }

                if (node->isLeftChild()) {
                    rightRotate(parent);
                    node->swapColor(grandparent);
                } else {
                    parent->swapColor(grandparent);
                }
                return leftRotate(grandparent);
            }
        }

        [[nodiscard]] constexpr static auto leftNode(NodePtr node) -> NodePtr
        {
            CERBLIB_UNROLL_N(2)
            while (true) {
                if (node->left == nullptr) {
                    return node;
                }
                node = node->left;
            }
        }

        [[nodiscard]] constexpr static auto rightNode(NodePtr node) -> NodePtr
        {
            CERBLIB_UNROLL_N(2)
            while (true) {
                if (node->right == nullptr) {
                    return node;
                }
                node = node->right;
            }
        }

        [[nodiscard]] constexpr static auto RBTreeReplace(NodePtr node) -> NodePtr
        {
            if (node->left != nullptr && node->right != nullptr) {
                return leftNode(node->right);
            }
            if (node->left != nullptr) {
                return node->left;
            }
            if (node->right != nullptr) {
                return node->right;
            }
            return nullptr;
        }

        constexpr auto fixDoubleBlack(NodePtr node) -> bool
        {
            NodePtr parent  = node->parent;
            NodePtr sibling = node->sibling();

            CERBLIB_UNROLL_N(1)
            while (true) {
                if (node == m_root) [[unlikely]] {
                    return true;
                }
                if (sibling == nullptr) {
                    node    = parent;
                    sibling = node->sibling();
                    parent  = node->parent;
                } else {
                    break;
                }
            }

            if (sibling->color == RED) {
                parent->color  = RED;
                sibling->color = BLACK;

                if (sibling->isLeftChild()) {
                    rightRotate(parent);
                } else {
                    leftRotate(parent);
                }
                return fixDoubleBlack(node);
            }

            if (sibling->left != nullptr && sibling->left->color == RED) {
                if (sibling->isLeftChild()) {
                    sibling->left->color = sibling->color;
                    sibling->color       = parent->color;
                    rightRotate(parent);
                } else {
                    sibling->left->color = parent->color;
                    rightRotate(sibling);
                    leftRotate(parent);
                }

                parent->color = BLACK;
            } else if (sibling->right != nullptr && sibling->right->color == RED) {
                if (sibling->isLeftChild()) {
                    sibling->right->color = parent->color;
                    leftRotate(sibling);
                    rightRotate(parent);
                } else {
                    sibling->right->color = sibling->color;
                    sibling->color        = parent->color;
                    leftRotate(parent);
                }

                parent->color = BLACK;
            } else {
                sibling->color = RED;

                if (parent->color != RED) {
                    return fixDoubleBlack(parent);
                }
                parent->color = BLACK;
            }
            return true;
        }

    protected:
        constexpr auto deleteNode(NodePtr node) -> bool
        {
            NodePtr u      = RBTreeReplace(node);
            NodePtr parent = node->parent;

            if (u == nullptr) {
                if (node == m_root) [[unlikely]] {
                    m_root = nullptr;
                } else {
                    if (node->color == BLACK) {
                        fixDoubleBlack(node);
                    } else if (node->sibling() != nullptr) {
                        node->sibling()->color = RED;
                    }

                    if (node->isLeftChild()) {
                        parent->left = nullptr;
                    } else {
                        parent->right = nullptr;
                    }
                }

                --m_size;
                NodeTraits::destroy(m_allocator, node);
                NodeTraits::deallocate(m_allocator, node, 1);
                return true;
            }

            if (node->left == nullptr || node->right == nullptr) {
                if (node == m_root) [[unlikely]] {
                    m_root  = u;
                    u->left = u->right = nullptr;

                    --m_size;
                    NodeTraits::destroy(m_allocator, node);
                    NodeTraits::deallocate(m_allocator, node, 1);
                } else [[likely]] {
                    if (node->isLeftChild()) {
                        parent->left = u;
                    } else {
                        parent->right = u;
                    }

                    bool color_of_node = node->color;

                    --m_size;
                    NodeTraits::destroy(m_allocator, node);
                    NodeTraits::deallocate(m_allocator, node, 1);

                    u->parent = parent;

                    if (u->color == BLACK && color_of_node) {
                        return fixDoubleBlack(u);
                    }

                    u->color = BLACK;
                }
                return true;
            }

            swap(u->value, node->value);
            return deleteNode(u);
        }

    private:
        template<typename U>
        [[nodiscard]] static constexpr auto RBTreeSearch(NodePtr root, const U &key)
            -> NodePtr
        {
            NodePtr tmp = root;

            while (tmp != nullptr) {
                if (Compare{}(key, tmp->value)) {
                    if (tmp->left == nullptr) {
                        return tmp;
                    }
                    tmp = tmp->left;
                } else if (key == tmp->value) {
                    return tmp;
                } else {
                    if (tmp->right == nullptr) {
                        return tmp;
                    }
                    tmp = tmp->right;
                }
            }

            return tmp;
        }

    protected:
        template<typename U>
        [[nodiscard]] constexpr auto search(const U &key) -> NodePtr
        {
            return RBTreeSearch<U>(m_root, key);
        }

        template<typename U>
        [[nodiscard]] constexpr auto search(const U &key) const -> NodePtr
        {
            return RBTreeSearch<U>(m_root, key);
        }

        template<bool Construct = true, bool Update = false, typename... Ts>
        constexpr auto RBTreeEmplace(Ts &&...args) -> NodePtr
        {
            if (m_root == nullptr) [[unlikely]] {
                ++m_size;
                NodePtr new_node = NodeTraits::allocate(m_allocator, 1);

                if constexpr (Construct) {
                    NodeTraits::construct(m_allocator, new_node, args...);
                } else {
                    Node::init(new_node);
                }

                new_node->color = BLACK;
                m_root          = new_node;

                return new_node;
            }

            T value     = T(args...);
            NodePtr tmp = search(value);

            if (tmp->value == value) [[unlikely]] {
                if constexpr (Update) {
                    destroy(tmp->value);
                    std::construct_at(&tmp->value, std::move(value));
                }

                return tmp;
            }

            NodePtr new_node = NodeTraits::allocate(m_allocator, 1);

            if (Compare{}(value, tmp->value)) {
                tmp->left = new_node;
            } else {
                tmp->right = new_node;
            }

            ++m_size;

            if constexpr (Construct) {
                NodeTraits::construct(m_allocator, new_node, std::move(value));
            } else {
                Node::init(new_node);
            }

            new_node->parent = tmp;
            fixRedRed(new_node);
            return new_node;
        }

        template<typename U>
        constexpr auto RBTreeErase(const U &key) noexcept -> bool
        {
            if (m_root == nullptr) {
                return false;
            }

            NodePtr v = search<U>(key);

            if (v->value == key) [[likely]] {
                return deleteNode(v);
            }

            return false;
        }

    private:
        constexpr static auto decrement(NodePtr m_node) -> NodePtr
        {
            if (m_node->left != nullptr) {
                m_node = m_node->left;
                return rightNode(m_node);
            }

            auto ptr = m_node->parent;

            CERBLIB_UNROLL_N(1)
            while (m_node == ptr->left) {
                m_node = ptr;
                ptr    = ptr->parent;

                if (ptr == nullptr) [[unlikely]] {
                    return nullptr;
                }
            }
            if (m_node->left != ptr) {
                return ptr;
            }

            return m_node;
        }

        constexpr static auto increment(NodePtr m_node) -> NodePtr
        {
            if (m_node->right != nullptr) {
                m_node = m_node->right;
                return leftNode(m_node);
            }

            auto ptr = m_node->parent;

            if (ptr == nullptr) [[unlikely]] {
                return nullptr;
            }

            while (m_node == ptr->right) {
                m_node = ptr;
                ptr    = ptr->parent;

                if (ptr == nullptr) [[unlikely]] {
                    return nullptr;
                }
            }
            if (m_node->right != ptr) {
                m_node = ptr;
            }

            return m_node;
        }

    public:
        class iterator
        {
            NodePtr m_node;

        public:
            using value_type = T;
            using reference  = T &;
            using pointer    = T *;

            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type   = ptrdiff_t;

        public:
            constexpr auto operator++() -> iterator &
            {
                m_node = increment(m_node);
                return *this;
            }

            constexpr auto operator++(int) -> iterator
            {
                auto copy = *this;
                this->operator++();
                return copy;
            }

            constexpr auto operator--() -> iterator &
            {
                m_node = decrement(m_node);
                return *this;
            }

            constexpr auto operator--(int) -> iterator
            {
                auto copy = *this;
                this->operator--();
                return copy;
            }

        public:
            constexpr auto operator*() const -> T &
            {
                return m_node->value;
            }

        public:
            constexpr friend auto
                operator==(const iterator &lhs, const iterator &rhs) -> bool
            {
                return lhs.m_node == rhs.m_node;
            }

            constexpr friend auto
                operator<=>(const iterator &lhs, const iterator &rhs)
            {
                return lhs.m_node <=> rhs.m_node;
            }

        public:
            constexpr auto operator=(iterator &&) noexcept -> iterator & = default;
            constexpr auto operator=(const iterator &) noexcept
                -> iterator      & = default;

        public:
            constexpr ~iterator() noexcept = default;

            constexpr iterator(iterator &&) noexcept      = default;
            constexpr iterator(const iterator &) noexcept = default;

            constexpr explicit iterator(NodePtr node) noexcept : m_node(node)
            {}
        };

        struct reverse_iterator
        {
            NodePtr m_node;

        public:
            using value_type        = T;
            using reference         = T &;
            using pointer           = T *;
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type   = ptrdiff_t;

        public:
            constexpr auto operator++() -> reverse_iterator &
            {
                m_node = decrement(m_node);
                return *this;
            }

            constexpr auto operator++(int) -> reverse_iterator
            {
                auto copy = *this;
                this->operator++();
                return copy;
            }

            constexpr auto operator--() -> reverse_iterator &
            {
                m_node = increment(m_node);
                return *this;
            }

            constexpr auto operator--(int) -> reverse_iterator
            {
                auto copy = *this;
                this->operator--();
                return copy;
            }

        public:
            constexpr auto operator*() const -> T &
            {
                return m_node->value;
            }

        public:
            [[nodiscard]] constexpr friend auto
                operator==(const reverse_iterator &lhs, const reverse_iterator &rhs)
                    -> bool
            {
                return lhs.m_node == rhs.m_node;
            }

            [[nodiscard]] constexpr friend auto
                operator<=>(const reverse_iterator &lhs, const reverse_iterator &rhs)
            {
                return lhs.m_node <=> rhs.m_node;
            }

        public:
            constexpr auto operator   =(reverse_iterator &&) noexcept
                -> reverse_iterator & = default;
            constexpr auto operator   =(const reverse_iterator &) noexcept
                -> reverse_iterator & = default;

        public:
            constexpr reverse_iterator(reverse_iterator &&) noexcept      = default;
            constexpr reverse_iterator(const reverse_iterator &) noexcept = default;

            constexpr explicit reverse_iterator(NodePtr node) noexcept : m_node(node)
            {}

            constexpr ~reverse_iterator() noexcept = default;
        };

        constexpr auto begin() -> iterator
        {
            if (m_root == nullptr) [[unlikely]] {
                return iterator(nullptr);
            }

            return iterator(leftNode(m_root));
        }

        constexpr auto end() -> iterator
        {
            return iterator(nullptr);
        }

        constexpr auto rbegin() -> reverse_iterator
        {
            return reverse_iterator(rightNode(m_root));
        }

        constexpr auto rend() -> reverse_iterator
        {
            return reverse_iterator(nullptr);
        }

    private:
        constexpr auto selfDelete(NodePtr x) -> void
        {
            if (x == nullptr) {
                return;
            }

            selfDelete(x->left);
            auto right = x->right;

            NodeTraits::destroy(m_allocator, x);
            NodeTraits::deallocate(m_allocator, x, 1);

            selfDelete(right);
        }

        constexpr auto copyElem(RBTreeNode *root, const RBTreeNode *src) -> void
        {
            if (src->left != nullptr) {
                root->left = NodeTraits::allocate(m_allocator, 1);
                NodeTraits::construct(
                    m_allocator, root->left,
                    static_cast<const T &>(src->left->value));
                root->left->parent = root;
                copyElem(root->left, src->left);
            }
            if (src->right != nullptr) {
                root->right = NodeTraits::allocate(m_allocator, 1);
                NodeTraits::construct(
                    m_allocator, root->right,
                    static_cast<const T &>(src->right->value));
                root->right->parent = root;
                return copyElem(root->right, src->right);
            }
        }

        constexpr auto copyFrom(const RBTree &other) -> void
        {
            if (other.m_root != nullptr) {
                m_root = NodeTraits::allocate(m_allocator, 1);
                NodeTraits::construct(
                    m_allocator, m_root,
                    static_cast<const T &>(other.m_root->value));

                copyElem(m_root, other.m_root);
            } else {
                m_root = nullptr;
            }
        }

        constexpr auto saveCopyElem(RBTreeNode *root, const RBTreeNode *src) -> void
        {
            if (src->left != nullptr) {
                if (root->left == nullptr) {
                    root->left = NodeTraits::allocate(m_allocator, 1);
                } else {
                    destroy(root->left->value);
                }
                NodeTraits::construct(
                    m_allocator, root->left,
                    static_cast<const T &>(src->left->value));
                root->left->parent = root;
                copyElem(root->left, src->left);
            }
            if (src->right != nullptr) {
                if (root->right == nullptr) {
                    root->right = NodeTraits::allocate(m_allocator, 1);
                } else {
                    destroy(root->right->value);
                }
                NodeTraits::construct(
                    m_allocator, root->right,
                    static_cast<const T &>(src->right->value));
                root->right->parent = root;
                return copyElem(root->right, src->right);
            }
        }

        constexpr auto saveCopyFrom(const RBTree &other) -> void
        {
            if (other.m_root != nullptr) {
                if (m_root == nullptr) {
                    m_root = NodeTraits::allocate(m_allocator, 1);
                } else {
                    destroy(m_root->value);
                }
                NodeTraits::construct(
                    m_allocator, m_root,
                    static_cast<const T &>(other.m_root->value));
                copyElem(m_root, other.m_root);
            } else {
                m_root = nullptr;
            }
        }

    public:
        constexpr auto operator=(const RBTree &other) -> RBTree &
        {
            saveCopyFrom(other);
            m_size = other.m_size;
            return *this;
        }

        constexpr auto operator=(RBTree &&other) noexcept -> RBTree &
        {
            m_size       = other.m_size;
            m_root       = other.m_root;
            other.m_root = nullptr;
            other.m_size = 0;
            return *this;
        }

    public:
        constexpr RBTree() noexcept = default;

        constexpr ~RBTree()
        {
            selfDelete(m_root);
        }

        constexpr RBTree(const RBTree &other) : m_size(other.m_size)
        {
            copyFrom(other);
        }

        constexpr RBTree(RBTree &&other) noexcept
          : m_root(other.m_root), m_size(other.m_size)
        {
            other.m_size = 0;
            other.m_root = nullptr;
        }
    };

    template<typename T>
    struct CERBLIB_TRIVIAL MultiSetNode
    {
        T value;
        size_t counter;

    public:
        [[nodiscard]] constexpr friend auto
            operator==(const MultiSetNode &lhs, const MultiSetNode &rhs)
        {
            return lhs.value == rhs.value;
        }

        [[nodiscard]] constexpr friend auto
            operator==(const MultiSetNode &lhs, const T &rhs)
        {
            return lhs.value == rhs;
        }

        [[nodiscard]] constexpr friend auto
            operator==(const T &lhs, const MultiSetNode &rhs)
        {
            return lhs == rhs.value;
        }

        [[nodiscard]] constexpr auto operator<=>(const MultiSetNode &rhs) const
        {
            return value <=> rhs.value;
        }

        [[nodiscard]] constexpr auto operator<=>(const T &rhs) const
        {
            return value <=> rhs;
        }

    public:
        constexpr auto operator=(const MultiSetNode &other) -> MultiSetNode &
        {
            value   = other.value;
            counter = other.counter;
            return *this;
        }

        constexpr auto operator=(MultiSetNode &&other) noexcept -> MultiSetNode &
        {
            value   = std::move(other.value);
            counter = other.counter;
            return *this;
        }

    public:
        constexpr explicit MultiSetNode(const T &t_value) noexcept
          : value(t_value), counter(0)
        {}

        constexpr explicit MultiSetNode(T &&t_value) noexcept
          : value(t_value), counter(0)
        {}

        constexpr MultiSetNode(MultiSetNode &&other) noexcept(
            std::is_nothrow_move_constructible_v<T>)
          : value(std::move(other.value)), counter(other.counter)
        {}

        constexpr MultiSetNode(const MultiSetNode &other) noexcept(
            std::is_nothrow_copy_constructible_v<T>)
          : value(other.value), counter(other.counter)
        {}

        constexpr ~MultiSetNode() = default;
    };
}// namespace cerb::PRIVATE

#endif /* CERBERUS_REDBLACKTREE_HPP */
