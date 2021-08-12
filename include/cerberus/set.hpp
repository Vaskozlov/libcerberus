#ifndef CERBERUS_SET_HPP
#define CERBERUS_SET_HPP

#include <memory>
#include <iostream>
#include <vector>
#include <set>
#include <functional>
#include <cerberus/types.h>
#include <cerberus/allocator.hpp>
#include <cerberus/operators.hpp>

namespace cerb::PRIVATE {
    enum Color : bool {
        RED = false,
        BLACK = true
    };

    template<typename T, auto Compare = less<void>{}, typename Alloc = std::allocator<T>>
    class BasicSet {
    protected:
        struct Node {
            Node *left, *right, *parent;
            T value;
            Color color;

        public:
            constexpr friend auto operator==(const Node &lhs, const Node &rhs) noexcept {
                return lhs.value == rhs.value;
            }

            constexpr friend auto operator==(const Node &lhs, const T &rhs) noexcept {
                return lhs.value == rhs;
            }

            constexpr friend auto operator==(const T &lhs, const Node &rhs) noexcept {
                return lhs == rhs.value;
            }

            constexpr std::strong_ordering operator<=>(const Node &other) const {
                return value <=> other.value;
            }

            constexpr std::strong_ordering operator<=>(const T &other) const {
                return value <=> other;
            }

        public:
            [[nodiscard]] constexpr
            auto isLeftChild() const noexcept -> bool {
                return this == parent->left;
            }

            [[nodiscard]] constexpr
            auto hasRedChild() const noexcept -> bool {
                return  (left != nullptr && left->color == RED) ||
                (right != nullptr && right->color == RED);
            }

        public:
            [[nodiscard]] constexpr
            auto getUncle() noexcept -> Node * {
                if (parent == nullptr || parent->parent == nullptr) UNLIKELY {
                    return nullptr;
                } else if (isLeftChild()) {
                    return parent->parent->right;
                } else {
                    return parent->parent->left;
                }
            }

            [[nodiscard]] constexpr
            auto getUncle() const noexcept -> Node * {
                if (parent == nullptr || parent->parent == nullptr) UNLIKELY {
                    return nullptr;
                } else if (isLeftChild()) {
                    return parent->parent->right;
                } else {
                    return parent->parent->left;
                }
            }

            [[nodiscard]] constexpr
            auto getSibling() noexcept -> Node * {
                if (parent == nullptr) UNLIKELY {
                    return nullptr;
                } else if (isLeftChild()) {
                    return parent->right;
                } else {
                    return parent->left;
                }
            }

            [[nodiscard]] constexpr
            auto getSibling() const noexcept -> Node * {
                if (parent == nullptr) UNLIKELY {
                    return nullptr;
                } else if (isLeftChild()) {
                    return parent->right;
                } else {
                    return parent->left;
                }
            }

            constexpr auto moveDown(Node *node) noexcept -> void {
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

            constexpr auto get() noexcept -> T& {
                return value;
            }

            constexpr auto get() const noexcept -> T& {
                return value;
            }

        public:
            CERBLIB_DISABLE_WARNING(reorder-ctor, reorder-ctor, 0)

            template<typename... Ts>
            constexpr explicit Node(Ts&&... args)
            : value(args...), color(RED),
            left(nullptr), right(nullptr), parent(nullptr)
            {}

            CERBLIB_ENABLE_WARNING(reorder-ctor, reorder-ctor, 0)
        };

    public:
        using NodePtr = Node *;
        using const_Node = const Node;
        using const_NodePtr = const Node*;

        using NodeAllocator = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
        using NodeTraits = std::allocator_traits<NodeAllocator>;

    protected:
        NodePtr root { nullptr };
        NodeAllocator m_allocator {};

    private:
        constexpr auto leftRotate(NodePtr node) noexcept -> void {
            NodePtr newParent = node->right; // new parent of node

            if (node == root) UNLIKELY {
                root = newParent; // if node is root node we need to update root
            }

            node->moveDown(newParent);
            node->right = newParent->left; // exchange node->right and newParent->left

            if (newParent->left != nullptr) LIKELY { // if there is node on the left side of the newParent
                newParent->left->parent = node;
            }

            newParent->left = node; // complete the rotation
        }

        constexpr auto rightRotate(NodePtr node) noexcept -> void {
            NodePtr newParent = node->left; // new parent of node

            if (node == root) UNLIKELY {
                root = newParent; // if node is root node we need to update root
            }

            node->moveDown(newParent);
            node->left = newParent->right; // exchange node->left and newParent->right

            if (newParent->right != nullptr) LIKELY { // if there is node on the right side of the newParent
                newParent->right->parent = node;
            }

            newParent->right = node; // complete the rotation
        }

        constexpr static auto swapColors(NodePtr node1, NodePtr node2) noexcept -> void {
            std::swap(node1->color, node2->color);
        }

        constexpr auto fixRedRed(NodePtr node) noexcept -> void {
            if (node == root) UNLIKELY {
                node->color = BLACK;
                return;
            }

            NodePtr parent = node->parent;
            NodePtr grandparent = parent->parent;
            NodePtr uncle = node->getUncle();

            if (parent->color == RED) {
                if (uncle != nullptr && uncle->color == RED) {
                    parent->color = uncle->color = BLACK;
                    grandparent->color = RED;
                    return fixRedRed(grandparent);
                } else {
                    if (parent->isLeftChild()) {
                        if (node->isLeftChild()) {
                            swapColors(parent, grandparent);
                        } else {
                            leftRotate(parent);
                            swapColors(node, grandparent);
                        }
                        return rightRotate(grandparent);
                    } else {
                        if (node->isLeftChild()) {
                            rightRotate(parent);
                            swapColors(node, grandparent);
                        } else {
                            swapColors(parent, grandparent);
                        }
                        return leftRotate(grandparent);
                    }
                }
            }
        }

        constexpr static auto successor(NodePtr node) noexcept -> NodePtr {
            NodePtr tmp = node;

            while (tmp->left != nullptr) {
                tmp = tmp->left;
            }

            return tmp;
        }

        constexpr static auto BSTReplace(NodePtr node) noexcept -> NodePtr {
            if (node->left != nullptr && node->right != nullptr) {
                return successor(node->right);
            } else if (node->left != nullptr) {
                return node->left;
            } else if (node->right != nullptr) {
                return node->right;
            } else UNLIKELY {
                return nullptr;
            }
        }

        constexpr auto fixDoubleBlack(NodePtr node) noexcept -> bool {
            if (node == root) UNLIKELY {
                return true;
            }
            NodePtr sibling = node->getSibling(), parent = node->parent;

            if (sibling == nullptr) {
                return fixDoubleBlack(parent);
            } else {
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
                    if (sibling->hasRedChild()) {
                        if (sibling->left != nullptr && sibling->left->color == RED) UNLIKELY {
                            if (sibling->isLeftChild()) {
                                sibling->left->color = sibling->color;
                                sibling->color = parent->color;
                                rightRotate(parent);
                            } else {
                                sibling->left->color = parent->color;
                                rightRotate(sibling);
                                leftRotate(parent);
                            }
                        } else {
                            if (sibling->isLeftChild()) {
                                sibling->right->color = parent->color;
                                leftRotate(sibling);
                                rightRotate(parent);
                            } else {
                                sibling->right->color = sibling->color;
                                sibling->color = parent->color;
                                leftRotate(parent);
                            }
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
            }
            return true;
        }

    protected:
        constexpr auto deleteNode(NodePtr node) noexcept -> bool {
            NodePtr u = BSTReplace(node);
            NodePtr parent = node->parent;

            if (u == nullptr) {
                if (node == root) UNLIKELY {
                    root = nullptr;
                } else {
                    if (node->color == BLACK) {
                        fixDoubleBlack(node);
                    } else if (node->getSibling() != nullptr) {
                        node->getSibling()->color = RED;
                    }

                    if (node->isLeftChild()) {
                        parent->left = nullptr;
                    } else {
                        parent->right = nullptr;
                    }
                }
                NodeTraits::destroy(m_allocator, node);
                NodeTraits::deallocate(m_allocator, node, 1);
                return true;
            }

            if (node->left == nullptr || node->right == nullptr) {
                if (node == root) UNLIKELY {
                    node->value = std::move(u->value);
                    node->left = node->right = nullptr;

                    NodeTraits::destroy(m_allocator, u);
                    NodeTraits::deallocate(m_allocator, u, 1);
                } else {
                    if (node->isLeftChild()) {
                        parent->left = u;
                    } else {
                        parent->right = u;
                    }
                    NodeTraits::destroy(m_allocator, node);
                    NodeTraits::deallocate(m_allocator, node, 1);

                    u->parent = parent;

                    if (u->color == BLACK && node->color == BLACK) {
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

    protected:
        template<typename U>
        constexpr auto search(const U& key) noexcept -> NodePtr {
            NodePtr tmp = root;

            while (tmp != nullptr) {
                if (Compare(key, tmp->value)) {
                    if (tmp->left == nullptr) {
                        return tmp;
                    } else {
                        tmp = tmp->left;
                    }
                } else if (key == tmp->value) {
                    return tmp;
                } else {
                    if (tmp->right == nullptr) {
                        return tmp;
                    } else {
                        tmp = tmp->right;
                    }
                }
            }

            return tmp;
        }

        template<typename U>
        constexpr auto search(const U& key) const noexcept -> NodePtr {
            NodePtr tmp = root;

            while (tmp != nullptr) {
                if (Compare(key, tmp->value)) {
                    if (tmp->left == nullptr) {
                        return tmp;
                    } else {
                        tmp = tmp->left;
                    }
                } else if (key == tmp->value) {
                    return tmp;
                } else {
                    if (tmp->right == nullptr) {
                        return tmp;
                    } else {
                        tmp = tmp->right;
                    }
                }
            }

            return tmp;
        }

    protected:
        template<typename... Ts>
        constexpr auto BSTEmplace(Ts&&... args) -> NodePtr {
            if (root == nullptr) UNLIKELY {
                NodePtr newNode = NodeTraits::allocate(m_allocator, 1);
                NodeTraits::construct(m_allocator, newNode, args...);
                newNode->color = BLACK;
                root = newNode;
                return newNode;
            }

            Node tmpNode = Node(args...);
            NodePtr tmp = search(tmpNode.value);

            if (tmp->value == tmpNode.value) UNLIKELY {
                return tmp;
            } else LIKELY {
                NodePtr newNode = NodeTraits::allocate(m_allocator, 1);
                NodeTraits::construct(m_allocator, newNode, args...);
                newNode->parent = tmp;

                if (Compare(tmpNode.value, tmp->value)) {
                    tmp->left = newNode;
                } else {
                    tmp->right = newNode;
                }

                fixRedRed(newNode);
                return tmp;
            }
        }

        template<typename U>
        constexpr auto BSTErase(const U& key) {
            if (root == nullptr) UNLIKELY {
                return false;
            }
            NodePtr v = search(key);

            if (v->value == key) LIKELY {
                return deleteNode(v);
            } else UNLIKELY {
                return false;
            }
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
        class iterator {
            NodePtr m_node { nullptr };
            bool isLeftChild;

        public:
            constexpr auto operator++() noexcept -> iterator& {
                NodePtr parent = m_node->parent;

                if (parent != nullptr) {
                    if (isLeftChild) {
                        m_node = parent->right;
                        isLeftChild = false;
                    } else {
                        m_node = parent;

                        if (m_node->parent != nullptr) {
                            isLeftChild = m_node->isLeftChild();
                        } else {
                            isLeftChild = false;
                        }
                    }
                } else {
                    m_node = nullptr;
                }
                return *this;
            }

        public:
            constexpr auto operator*() {
                return m_node->value;
            }

        public:
            constexpr iterator() noexcept = default;

            constexpr iterator(NodePtr node) noexcept
            : m_node(node)
            {
                if (node->parent != nullptr) {
                    isLeftChild = node->isLeftChild();
                } else {
                    isLeftChild = false;
                }
            }

            constexpr ~iterator() noexcept = default;
        };

        constexpr auto begin() noexcept -> iterator {
            NodePtr tmp = root;

            while (tmp != nullptr) {
                if (tmp->left == nullptr) {
                    return tmp;
                }
                tmp = tmp->left;
            }

            return tmp;
        }

        constexpr auto end() noexcept -> iterator {
            return iterator(nullptr);
        }

    public:
        constexpr BasicSet() noexcept = default;

        constexpr ~BasicSet() {
            self_delete(root);
        }
    };
}

namespace cerb {
    template<typename T, auto OnThrowing = cerb::Throwable {}, auto Compare = less<void>{}, typename Alloc = std::allocator<T>>
    class Set : public PRIVATE::BasicSet<T, Compare, Alloc> {
        using PRIVATE::BasicSet<T, Compare, Alloc>::search;
        using PRIVATE::BasicSet<T, Compare, Alloc>::BSTErase;
        using PRIVATE::BasicSet<T, Compare, Alloc>::BSTEmplace;
        using Node = typename PRIVATE::BasicSet<T, Compare, Alloc>::Node;

    private:
        constexpr static auto MayThrow = std::is_same_v<Throwable, decltype(OnThrowing)>;

    public:
        constexpr auto count(const T& key) {
            auto value = search(key);
            return value != nullptr && value->value == key;
        }

        constexpr auto insert(const T& key) noexcept {
            BSTEmplace(key);
        }

        constexpr auto erase(const T& key) noexcept(!MayThrow) {
            if constexpr (MayThrow) {
                if (!BSTErase(key)) UNLIKELY {
                    throw std::out_of_range("Unable to erase elem from cerb::Set");
                }
                return true;
            } else {
                return BSTErase(key);
            }
        }
    };

    namespace PRIVATE {
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
            template<typename... Ts>
            constexpr explicit MultiSetNode(Ts&&... args)
            : value(args...), counter(0)
            {}

            constexpr ~MultiSetNode() = default;
        };
    }

    template<typename T, auto OnThrowing = cerb::Throwable {}, auto Compare = less<void>{}, typename Alloc = std::allocator<T>>
    class Multiset : public PRIVATE::BasicSet<PRIVATE::MultiSetNode<T>, Compare, Alloc> {
        using PRIVATE::BasicSet<PRIVATE::MultiSetNode<T>, Compare, Alloc>::search;
        using PRIVATE::BasicSet<PRIVATE::MultiSetNode<T>, Compare, Alloc>::BSTErase;
        using PRIVATE::BasicSet<PRIVATE::MultiSetNode<T>, Compare, Alloc>::BSTEmplace;
        using PRIVATE::BasicSet<PRIVATE::MultiSetNode<T>, Compare, Alloc>::deleteNode;
        using Node = typename PRIVATE::BasicSet<PRIVATE::MultiSetNode<T>, Compare, Alloc>::Node;

    private:
        constexpr static auto MayThrow = std::is_same_v<Throwable, decltype(OnThrowing)>;

    public:
        constexpr auto count(const T& key) const noexcept -> size_t {
            auto value = search(key);

            if (value == nullptr || value->value.value != key) {
                return 0;
            } else {
                return value->value.counter;
            }
        }

        constexpr auto insert(const T& key) noexcept {
            BSTEmplace(key)->value.counter += 1;
        }

        constexpr auto erase(const T &key) noexcept(!MayThrow) {
            auto value = search(key);

            if constexpr (MayThrow) {
                if (value != nullptr && value->value.value == key) LIKELY {
                    value->value.counter -= 1;
                } else UNLIKELY {
                    throw std::out_of_range("Unable to erase elem from cerb::Multiset");
                }
            } else {
                if (value != nullptr && value->value.value == key) LIKELY {
                    value->value.counter -= 1;
                } else UNLIKELY {
                    return false;
                }
            }

            if (value->value.counter == 0) {
                return deleteNode(value);
            }

            return true;
        }
    };
}

#endif /* CERBERUS_SET_HPP */
