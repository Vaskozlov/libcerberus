#ifndef CERBERUS_REDBLACKTREE_HPP
#define CERBERUS_REDBLACKTREE_HPP

#include <memory>
#include <functional>
#include <cerberus/pair.hpp>
#include <cerberus/operators.hpp>

namespace cerb::PRIVATE {
    template<typename T, typename Compare = less<T>, typename Alloc = std::allocator<T>>
    class RBTree {
        enum RBTreeNodeColor : bool {
            RED = false,
            BLACK = true
        };

        struct RBTreeNode {
            RBTreeNode *left, *right, *parent;
            T value;
            RBTreeNodeColor color;

        public:
            [[nodiscard]] constexpr
            auto operator==(const RBTreeNode &other) const noexcept -> bool {
                return value == other.value;
            }

            [[nodiscard]] constexpr
            auto operator==(const T &other) const noexcept -> bool {
                return value == other;
            }

            [[nodiscard]] constexpr
            std::strong_ordering operator<=>(const RBTreeNode &other) const noexcept {
                return value <=> other.value;
            }

            [[nodiscard]] constexpr
            std::strong_ordering operator<=>(const T &other) const noexcept {
                return value <=> other.value;
            }
        public:
            [[nodiscard]] constexpr
            auto isLeftChild() const noexcept {
                return this == parent->left;
            }

            [[nodiscard]] constexpr
            auto hasRedChild() const noexcept -> bool {
                return  (left != nullptr && left->color == RED) ||
                        (right != nullptr && right->color == RED);
            }

            [[nodiscard]] constexpr
            auto hasChild() const noexcept -> bool {
                return left != nullptr || right != nullptr;
            }

            [[nodiscard]] constexpr
            auto uncle() noexcept -> RBTreeNode * {
                if (parent == nullptr || parent->parent == nullptr) UNLIKELY {
                    return nullptr;
                } else if (isLeftChild()) {
                    return parent->parent->right;
                } else {
                    return parent->parent->left;
                }
            }

            [[nodiscard]] constexpr
            auto uncle() const noexcept -> RBTreeNode * {
                if (parent == nullptr || parent->parent == nullptr) UNLIKELY {
                    return nullptr;
                } else if (isLeftChild()) {
                    return parent->parent->right;
                } else {
                    return parent->parent->left;
                }
            }

            [[nodiscard]] constexpr
            auto sibling() noexcept -> RBTreeNode * {
                if (parent == nullptr) UNLIKELY {
                    return nullptr;
                } else if (isLeftChild()) {
                    return parent->right;
                } else {
                    return parent->left;
                }
            }

            constexpr auto swapColor(RBTreeNode *node) noexcept -> void {
                std::swap(color, node->color);
            }

            constexpr
            auto modeDown(RBTreeNode *node) noexcept -> void {
                if (parent != nullptr) LIKELY {
                    if (isLeftChild()) {
                        parent->left = node;
                    } else {
                        parent->right = node;
                    }
                }

                node->parent = parent;
                parent = node;
            }

        public:
            constexpr static auto init(RBTreeNode *node) noexcept -> void {
                node->color = RED;
                node->left = node->right = node->parent = nullptr;
            }

        public:
            CERBLIB_DISABLE_WARNING(reorder-ctor, reorder-ctor, 0)

            template<typename... Ts>
            constexpr explicit RBTreeNode(Ts&&... args) noexcept
            : value(args...), color(RED),
              left(nullptr), right(nullptr), parent(nullptr)
            {}

            constexpr explicit RBTreeNode(const T &t_value) noexcept
            : value(t_value), color(RED),
              left(nullptr), right(nullptr), parent(nullptr)
            {}

            constexpr explicit RBTreeNode(const T &&t_value) noexcept(std::is_nothrow_move_constructible_v<T>)
            : value(t_value), color(RED),
              left(nullptr), right(nullptr), parent(nullptr)
            {}

            constexpr RBTreeNode(RBTreeNode &&other) noexcept(std::is_nothrow_move_constructible_v<T>)
            : value(std::move(other.value)), color(other.color),
              left(other.left), right(other.right), parent(other.parent)
            {}

            constexpr RBTreeNode(const RBTreeNode &other) noexcept(std::is_nothrow_copy_constructible_v<T>)
            : value(other.value), color(other.color),
              left(other.left), right(other.right), parent(other.parent)
            {}

            CERBLIB_ENABLE_WARNING(reorder-ctor, reorder-ctor, 0)
        };

    protected:
        using Node = RBTreeNode;
        using NodePtr = RBTreeNode*;
        using ConstNodePtr = const RBTreeNode*;

        using NodeAllocator = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
        using NodeTraits = std::allocator_traits<NodeAllocator>;

    protected:
        NodePtr m_root { nullptr };
        size_t  m_size { 0 };
        NodeAllocator m_allocator {};

    public:
        [[nodiscard]] constexpr
        auto size() const noexcept {
            return m_size;
        }

    private:
        constexpr auto leftRotate(NodePtr node) noexcept -> void {
            NodePtr new_parent = node->right; // new parent of the node

            if (node == m_root) UNLIKELY {
                m_root = new_parent; // if node is root node we need to update root
            }

            node->modeDown(new_parent);
            node->right = new_parent->left;

            if (new_parent->left != nullptr) LIKELY {
                new_parent->left->parent = node;
            }

            new_parent->left = node; // complete the rotation
        }

        constexpr auto rightRotate(NodePtr node) noexcept -> void {
            NodePtr new_parent = node->right;

            if (node == m_root) UNLIKELY {
                m_root = new_parent;
            }

            node->modeDown(new_parent);
            node->left = new_parent->right;

            if (new_parent->right != nullptr) LIKELY {
                new_parent->right->parent = node;
            }

            new_parent->right = node;
        }

        constexpr auto fixRedRed(NodePtr node) noexcept -> void {
            if (node == m_root) UNLIKELY {
                node->color = BLACK; // root is always black
                return;
            }

            NodePtr parent = node->parent, grandparent = parent->parent, uncle = node->uncle();

            if (parent->color == RED) {
                if (uncle != nullptr && uncle->color == RED) {
                    parent->color = uncle->color = BLACK;
                    grandparent->color = RED;
                    return fixRedRed(grandparent);
                } else {
                    if (parent->isLeftChild()) {
                        if (node->isLeftChild()) {
                            parent->swapColor(grandparent);
                        } else {
                            leftRotate(parent);
                            node->swapColor(grandparent);
                        }
                        return rightRotate(grandparent);
                    } else {
                        if (node->isLeftChild()) {
                            rightRotate(parent);
                            node->swapColor(grandparent);
                        } else {
                            parent->swapColor(grandparent);
                        }
                        return leftRotate(grandparent);
                    }
                }
            }
        }

        [[nodiscard]] constexpr static
        auto leftNode(NodePtr node) noexcept -> NodePtr {
            while (node->left != nullptr) {
                node = node->left;
            }
            return node;
        }

        [[nodiscard]] constexpr static
        auto rightNode(NodePtr node) noexcept -> NodePtr {
            while (node->right != nullptr) {
                node = node->right;
            }
            return node;
        }

        [[nodiscard]] constexpr static
        auto RBTreeReplace(NodePtr node) noexcept -> NodePtr {
            if (node->left != nullptr && node->right != nullptr) {
                return leftNode(node->right);
            } else if (node->left != nullptr) {
                return node->left;
            } else if (node->right != nullptr) {
                return node->right;
            } else UNLIKELY {
                return nullptr;
            }
        }

        constexpr auto fixDoubleBlack(NodePtr node) noexcept -> bool {
            NodePtr sibling = node->sibling(), parent = node->parent;

            while (true) {
                if (node == m_root) UNLIKELY {
                    return true;
                }

                if (sibling == nullptr) {
                    node = parent;
                    sibling = node->sibling();
                    parent = node->parent;
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
            } else {
                if (sibling->left != nullptr && sibling->left->color == RED) {
                    if (sibling->isLeftChild()) {
                        sibling->left->color = sibling->color;
                        sibling->color = parent->color;
                        rightRotate(parent);
                    } else {
                        sibling->left->color = parent->color;
                        rightRotate(sibling);
                        leftRotate(parent);
                    }

                    parent->color = BLACK;
                } else if (sibling->right != nullptr && sibling->right->color == RED){
                    if (sibling->isLeftChild()) {
                        sibling->right->color = parent->color;
                        leftRotate(sibling);
                        rightRotate(parent);
                    } else {
                        sibling->right->color = sibling->color;
                        sibling->color = parent->color;
                        leftRotate(parent);
                    }

                    parent->color = BLACK;
                } else {
                    sibling->color = RED;

                    if (parent->color != RED) {
                        return fixDoubleBlack(parent);
                    } else {
                        parent->color = BLACK;
                    }
                }
            }

            return true;
        }

    protected:
        constexpr auto deleteNode(NodePtr node) noexcept -> bool {
            NodePtr u = RBTreeReplace(node);
            NodePtr parent = node->parent;

            if (u == nullptr) {
                if (node == m_root) UNLIKELY {
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

            if (!node->hasChild()) {
                if (node == m_root) UNLIKELY {
                    node->value = std::move(u->value);
                    node->left = node->right = nullptr;

                    --m_size;
                    NodeTraits::destroy(m_allocator, u);
                    NodeTraits::deallocate(m_allocator, u, 1);
                } else LIKELY {
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
                    } else {
                        u->color = BLACK;
                    }
                }
                return true;
            }

            std::swap(u->value, node->value);
            return deleteNode(u);
        }

    private:
        template<typename U> [[nodiscard]] static constexpr CERBLIB_INLINE
        auto RBTreeSearch(NodePtr root, const U &key) noexcept -> NodePtr {
            NodePtr tmp = root;

            while (tmp != nullptr) {
                if (Compare{}(key, tmp->value)) {
                    if constexpr (Compare::is_less) {
                        if (tmp->left == nullptr) {
                            return tmp;
                        } else {
                            tmp = tmp->left;
                        }
                    } else {
                        if (tmp->right == nullptr) {
                            return tmp;
                        } else {
                            tmp = tmp->right;
                        }
                    }
                } else if (key == tmp->value) {
                    return tmp;
                } else {
                    if constexpr (Compare::is_less) {
                        if (tmp->right == nullptr) {
                            return tmp;
                        } else {
                            tmp = tmp->right;
                        }
                    } else {
                        if (tmp->left == nullptr) {
                            return tmp;
                        } else {
                            tmp = tmp->left;
                        }
                    }
                }
            }

            return tmp;
        }

    protected:
        template<typename U> [[nodiscard]] constexpr
        auto search(const U &key) noexcept -> NodePtr {
            return RBTreeSearch<U>(m_root, key);
        }

        template<typename U> [[nodiscard]] constexpr
        auto search(const U &key) const noexcept -> NodePtr {
            return RBTreeSearch<U>(m_root, key);
        }

        template<bool Construct = true, typename... Ts> constexpr
        auto RBTreeEmplace(Ts&&... args) -> NodePtr {
            if (m_root == nullptr) UNLIKELY {
                ++m_size;
                NodePtr new_node = NodeTraits::allocate(m_allocator, 1);

                if constexpr (Construct) {
                    NodeTraits::construct(m_allocator, new_node, args...);
                } else {
                    Node::init(new_node);
                }

                new_node->color = BLACK;
                m_root = new_node;

                return new_node;
            }

            T value = T(args...);
            NodePtr tmp = search(value);

            if (tmp->value == value) UNLIKELY {
                return tmp;
            } else LIKELY {
                NodePtr new_node = NodeTraits::allocate(m_allocator, 1);

                if (Compare{}(value, tmp->value) && Compare::is_less) {
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
        }

        template<typename U> constexpr
        auto RBTreeErase(const U &key) noexcept -> bool {
            if (m_root == nullptr) {
                return false;
            }

            NodePtr v = search<U>(key);

            if (v->value == key) LIKELY {
                return deleteNode(v);
            } else UNLIKELY {
                return false;
            }
        }

    private:
        constexpr static auto decrement(NodePtr m_node) noexcept -> NodePtr {
            if (m_node->left != nullptr) {
                m_node = m_node->left;
                return rightNode(m_node);
            } else {
                auto ptr = m_node->parent;

                while (m_node == ptr->left) {
                    m_node = ptr;
                    ptr = ptr->parent;

                    if (ptr == nullptr) UNLIKELY {
                        return nullptr;
                    }
                }
                if (m_node->left != ptr) {
                    return ptr;
                }
            }

            return m_node;
        }

        constexpr static auto increment(NodePtr m_node) noexcept -> NodePtr {
            if (m_node->right != nullptr) {
                m_node = m_node->right;
                return leftNode(m_node);
            } else {
                auto ptr = m_node->parent;

                if (ptr == nullptr) UNLIKELY {
                    return nullptr;
                }

                while (m_node == ptr->right) {
                    m_node = ptr;
                    ptr = ptr->parent;

                    if (ptr == nullptr) UNLIKELY {
                        return nullptr;
                    }
                }
                if (m_node->right != ptr) {
                    m_node = ptr;
                }
            }

            return m_node;
        }

    public:
        class iterator {
            NodePtr m_node;

        public:
            typedef T  value_type;
            typedef T& reference;
            typedef T* pointer;

            typedef std::bidirectional_iterator_tag iterator_category;
            typedef ptrdiff_t			 difference_type;

        public:
            constexpr auto operator++() noexcept -> iterator& {
                m_node = increment(m_node);
                return *this;
            }

            constexpr auto operator++(int) noexcept -> iterator {
                auto copy = *this;
                this->operator++();
                return copy;
            }

            constexpr auto operator--() noexcept -> iterator& {
                m_node = decrement(m_node);
                return *this;
            }

            constexpr auto operator--(int) noexcept -> iterator {
                auto copy = *this;
                this->operator--();
                return copy;
            }

        public:
            constexpr auto operator*() const noexcept -> T& {
                return m_node->value;
            }

        public:
            constexpr friend auto operator==(const iterator &lhs, const iterator &rhs) noexcept -> bool {
                return lhs.m_node == rhs.m_node;
            }

            constexpr friend auto operator<=>(const iterator &lhs, const iterator &rhs) noexcept {
                return lhs.m_node <=> rhs.m_node;
            }

        public:
            constexpr iterator &operator=(iterator &&) noexcept = default;
            constexpr iterator &operator=(const iterator &) noexcept = default;

        public:
            constexpr iterator(iterator&&) noexcept = default;
            constexpr iterator(const iterator &) noexcept = default;

            constexpr explicit iterator(NodePtr node) noexcept
            : m_node(node)
            {}
        };

        struct reverse_iterator {
            NodePtr m_node;

        public:
            using value_type            = T;
            using reference             = T& ;
            using pointer               = T*;
            using iterator_category     = std::bidirectional_iterator_tag;
            using difference_type       = ptrdiff_t;

        public:
            constexpr auto operator++() noexcept -> reverse_iterator& {
                m_node = decrement(m_node);
                return *this;
            }

            constexpr auto operator++(int) noexcept -> reverse_iterator {
                auto copy = *this;
                this->operator++();
                return copy;
            }

            constexpr auto operator--() noexcept -> reverse_iterator& {
                m_node = increment(m_node);
                return *this;
            }

            constexpr auto operator--(int) noexcept -> reverse_iterator {
                auto copy = *this;
                this->operator--();
                return copy;
            }

        public:
            constexpr auto operator*() const noexcept -> T& {
                return m_node->value;
            }

        public:
            [[nodiscard]] constexpr friend
            auto operator==(const reverse_iterator &lhs, const reverse_iterator &rhs) noexcept -> bool {
                return lhs.m_node == rhs.m_node;
            }

            [[nodiscard]] constexpr friend
            auto operator<=>(const reverse_iterator &lhs, const reverse_iterator &rhs) noexcept {
                return lhs.m_node <=> rhs.m_node;
            }

        public:
            constexpr reverse_iterator &operator=(reverse_iterator &&) noexcept = default;
            constexpr reverse_iterator &operator=(const reverse_iterator &) noexcept = default;

        public:
            constexpr reverse_iterator(reverse_iterator&&) noexcept = default;
            constexpr reverse_iterator(const reverse_iterator &) noexcept = default;

            constexpr explicit reverse_iterator(NodePtr node) noexcept
            : m_node(node)
            {}
        };

        constexpr auto begin() noexcept -> iterator {
            return iterator(leftNode(m_root));
        }

        constexpr auto end() noexcept -> iterator {
            return iterator(nullptr);
        }

        constexpr auto rbegin() noexcept -> reverse_iterator {
            return reverse_iterator(rightNode(m_root));
        }

        constexpr auto rend() noexcept -> reverse_iterator {
            return reverse_iterator(nullptr);
        }

    private:
        constexpr void self_delete(NodePtr x) {
            if (x == nullptr)
                return;

            self_delete(x->left);
            auto right = x->right;

            NodeTraits::destroy(m_allocator, x);
            NodeTraits::deallocate(m_allocator, x, 1);

            self_delete(right);
        }

    public:
        constexpr RBTree() noexcept = default;

        constexpr ~RBTree() {
            self_delete(m_root);
        }
    };

    template<typename T>
    struct CERBLIB_TRIVIAL MultiSetNode {
        T value;
        size_t counter;

    public:
        [[nodiscard]] constexpr friend
        auto operator==(const MultiSetNode &lhs, const MultiSetNode &rhs) {
            return lhs.value == rhs.value;
        }

        [[nodiscard]] constexpr friend
        auto operator==(const MultiSetNode &lhs, const T &rhs) {
            return lhs.value == rhs;
        }

        [[nodiscard]] constexpr friend
        auto operator==(const T &lhs, const MultiSetNode &rhs) {
            return lhs == rhs.value;
        }

        [[nodiscard]] constexpr
        auto operator<=>(const MultiSetNode &rhs) const {
            return value <=> rhs.value;
        }

        [[nodiscard]] constexpr
        auto operator<=>(const T &rhs) const {
            return value <=> rhs;
        }

    public:
        constexpr MultiSetNode &operator=(const MultiSetNode &other) noexcept {
            value = other.value;
            counter = other.counter;
            return *this;
        }

        constexpr MultiSetNode &operator=(MultiSetNode &&other) noexcept {
            value = std::move(other.value);
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

        constexpr MultiSetNode(MultiSetNode&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value(std::move(other.value)), counter(other.counter)
        {}

        constexpr MultiSetNode(const MultiSetNode& other) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : value(other.value), counter(other.counter)
        {}

        constexpr ~MultiSetNode() = default;
    };
} // namespace cerb::PRIVATE

#endif /* CERBERUS_REDBLACKTREE_HPP */
