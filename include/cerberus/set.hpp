#ifndef CERBERUS_SET_HPP
#define CERBERUS_SET_HPP

#include <memory>
#include <iostream>
#include <cerberus/types.h>
#include <cerberus/allocator.hpp>

namespace cerb {
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

#endif /* CERBERUS_SET_HPP */
