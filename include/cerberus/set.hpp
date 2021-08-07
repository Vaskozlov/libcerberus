#ifndef CERBERUS_SET_HPP
#define CERBERUS_SET_HPP

#include <memory>
#include <iostream>
#include <cerberus/types.h>
#include <cerberus/allocator.hpp>

namespace cerb::old {
    template<typename T>
    struct Set {
        enum Color : bool {
            red = true,
            black = false
        };

        struct Node {
            Color color;
            T data;
            Node *left, *right, *root;

        public:
            explicit Node(const T &value)
            : data(value), color(red),
            left(nullptr), right(nullptr), root(nullptr)
            {}
        };

    public:
        Node *root;

    protected:
        void rotateLeft(Node *&root_node , Node *&pt) {
            Node *pt_right = pt->right;
            pt->right = pt->right->left;

            if (pt->right != nullptr) {
                pt->right->root = pt;
            }

            if (pt->root == nullptr) {
                root_node = pt_right;
            } else if (pt == pt->root->left) {
                pt->root->left = pt_right;
            } else {
                pt->root->right = pt_right;
            }

            pt_right->left = pt;
            pt->root = pt_right;
        }

        void rotateRight(Node *&root_node, Node *&pt) {
            Node *pt_left = pt->left;
            pt->left = pt->left->right;

            if (pt->left != nullptr) {
                pt->left->root = pt;
            }

            pt_left->root = pt->root;

            if (pt->root == nullptr) {
                root_node = pt_left;
            } else if (pt == pt->root->left) {
                pt->root->left = pt_left;
            } else {
                pt->root->right = pt_left;
            }

            pt_left->right = pt;
            pt->root = pt_left;
        }

        void fixViolation(Node *&root_node, Node *&pt) {
            Node *parent_pt = nullptr, *grand_parent_pt = nullptr;

            while (
                (pt != root_node) &&
                (pt->color != black) &&
                (pt->root->color == red)
            ) {
                parent_pt = pt->root;
                grand_parent_pt = parent_pt->root;

                if (parent_pt == grand_parent_pt->left) {
                    Node *uncle_pt = grand_parent_pt->right;

                    if (uncle_pt != nullptr && uncle_pt->color == red) {
                        grand_parent_pt->color = red;
                        parent_pt->color = black;
                        uncle_pt->color = black;
                        pt = grand_parent_pt;
                    } else {
                        if (pt == parent_pt->right) {
                            rotateLeft(root_node, parent_pt);
                            pt = parent_pt;
                            parent_pt = pt->root;
                        }

                        rotateRight(root_node, parent_pt);
                        std::swap(parent_pt->color, grand_parent_pt->color);
                        pt = parent_pt;
                    }
                }
                else {
                    Node *uncle_pt = grand_parent_pt->left;
                    if ((uncle_pt != nullptr) && (uncle_pt->color == red)) {
                        grand_parent_pt->color = red;
                        parent_pt->color = black;
                        uncle_pt->color = black;
                        pt = grand_parent_pt;
                    }
                    else {
                        if (pt == parent_pt->left) {
                            rotateRight(root_node, parent_pt);
                            pt = parent_pt;
                            parent_pt = pt->root;
                        }
                        rotateLeft(root_node, grand_parent_pt);
                        std::swap(parent_pt->color, grand_parent_pt->color);
                        pt = parent_pt;
                    }
                }
            }
            root_node->color = black;
        }

    private:
        static Node *RBTInsert(Node *root, Node *pt) {
            if (root == nullptr) UNLIKELY {
                return pt;
            } else if (pt->data < root->data) {
                root->left = RBTInsert(root->left, pt);
                root->left->root = root;
            } else if (pt->data > root->data) {
                root->right = RBTInsert(root->right, pt);
                root->right->root = root;
            }

            return root;
        }

    public:
        void insert(const T& value) {
            Node *pt = new Node(value);
            root = RBTInsert(root, pt);
            fixViolation(root, pt);
        }

        static void inorderHelper(Node *root)
        {
            if (root == NULL)
                return;

            inorderHelper(root->left);
            std::cout << root->data << "  ";
            inorderHelper(root->right);
        }
    };
}

namespace cerb {
    template<typename T>
    class RBTree {
        enum Color : bool {
            RED = true,
            BLACK = false
        };

        struct Node {
            Node *left, *right, *parent;
            T value;
            Color color;

        public:
            constexpr auto isLeftChild() const noexcept -> bool {
                return this == parent->left;
            }

            constexpr auto hasRedChild() const noexcept -> bool {
                return  (left != nullptr && left->color == RED) ||
                        (right != nullptr && right->color == RED);
            }

        public:
            constexpr auto getUncle() noexcept -> Node * {
                if (parent == nullptr || parent->parent == nullptr) {
                    return nullptr;
                }
                else if (isLeftChild()) {
                    return parent->right;
                }
                else {
                    return parent->left;
                }
            }

            constexpr auto getSibling() {
                if (parent == nullptr) {
                    return nullptr;
                }

                if (isLeftChild()) {
                    return parent->right;
                }
                else {
                    return parent->left;
                }
            }

            constexpr auto moveDown(Node *nParent) noexcept -> void {
                if (parent != nullptr) {
                    if (isLeftChild()) {
                        parent->left = nParent;
                    }
                    else {
                        parent->right = nParent;
                    }
                }

                nParent->parent = parent;
                parent = nParent;
            }

        public:
            explicit constexpr Node(const T &t_value)
            : value(t_value), color(RED),
              left(nullptr), right(nullptr), parent(nullptr)
            {}
        };

    protected:
        Node *root;

    protected:
        constexpr auto leftRotate(Node *x) -> void {
            Node *nParent = x->right;

            if (x == root) {
                root = nParent;
            }

            x->moveDown(nParent);
            x->right = nParent->left;

            if (nParent->left != nullptr) {
                nParent->left->parent = x;
            }

            nParent->left = x;
        }

        constexpr auto rightRotate(Node *x) -> void {
            Node *nParent = x->left;

            if (x == root) {
                root = nParent;
            }

            x->moveDown(nParent);
            x->left = nParent->right;

            if (nParent->right != nullptr) {
                nParent->right->parent = x;
            }

            nParent->right = x;
        }

        constexpr auto swapColors(Node *x1, Node *x2) -> void {
            std::swap(x1->color, x2->color);
        }

        constexpr auto fixRedRed(Node *x) -> void {
            if (x == root) {
                x->color = BLACK;
                return;
            }

            Node *uncle = x->getUncle();
            Node *parent = x->parent, *grandparent = parent->parent;

            if (parent->color == RED) {
                if (uncle != nullptr && uncle->color == RED) {
                    parent->color = uncle->color = BLACK;
                    grandparent->color = RED;
                    fixRedRed(grandparent);
                }
                else {
                    if (parent->isLeftChild()) {
                        if (x->isLeftChild()) {
                            swapColors(parent, grandparent);
                        }
                        else {
                            leftRotate(parent);
                            swapColors(x, grandparent);
                        }
                        rightRotate(grandparent);
                    }
                    else {
                        if (x->isLeftChild()) {
                            rightRotate(parent);
                            swapColors(x, grandparent);
                        }
                        else {
                            swapColors(parent, grandparent);
                        }
                        leftRotate(grandparent);
                    }
                }
            }
        }

        constexpr auto successor(Node *x) noexcept -> Node * {
            Node *tmp = x;

            while (tmp->left != nullptr) {
                tmp = tmp->left;
            }

            return tmp;
        }

        constexpr auto BSTReplace(Node *x) noexcept {
            if (x->left != nullptr && x->right != nullptr) {
                return successor(x->right);
            }
            else if (x->left == nullptr && x->right == nullptr) {
                return nullptr;
            }
            else if (x->left != nullptr) {
                return x->left;
            }
            else {
                return x->right;
            }
        }
        // deleteNode
    };
}

#endif /* CERBERUS_SET_HPP */
