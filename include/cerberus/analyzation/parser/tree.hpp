#ifndef CERBERUS_TREE_HPP
#define CERBERUS_TREE_HPP

#include <memory>
#include <iostream>
#include <iomanip>
#include <cerberus/types.h>
#include <cerberus/vector.hpp>

namespace cerb::analysis::parser {
    template<typename T, typename Alloc = std::allocator<T>>
    struct TreeNode
    {
        TreeNode *root{}, *left{}, *right{};
        T value{};

        using NodeAllocator =
            typename std::allocator_traits<Alloc>::template rebind_alloc<TreeNode>;
        using NodeTraits = std::allocator_traits<NodeAllocator>;
        static inline NodeAllocator m_allocator{};

        static constexpr auto
            construct_root(TreeNode *left, TreeNode *root, TreeNode *right)
                -> TreeNode *
        {
            root->left  = left;
            root->right = right;
            left->root  = root;
            right->root = root;
            return root;
        }

        static constexpr auto construct_node(const T &value) -> TreeNode *
        {
            TreeNode *new_node = NodeTraits::allocate(m_allocator, 1);
            NodeTraits::construct(m_allocator, new_node, value);
            return new_node;
        }

        static constexpr auto selfDelete(TreeNode *x)
        {
            if (x == nullptr) {
                return;
            }

            selfDelete(x->left);
            auto *right = x->right;

            NodeTraits::destroy(m_allocator, x);
            NodeTraits::deallocate(m_allocator, x, 1);

            selfDelete(right);
        }

        constexpr TreeNode() = default;

        constexpr TreeNode(const T &value_) : value(value_)
        {}

        constexpr ~TreeNode()
        {}
    };

    template<typename T, typename Alloc = std::allocator<T>>
    struct ParserNode
    {
        using NodeAllocator =
            typename std::allocator_traits<Alloc>::template rebind_alloc<ParserNode>;
        using NodeTraits = std::allocator_traits<NodeAllocator>;

        ParserNode *root;
        Vector<ParserNode *> children;
        T value;

        static inline NodeAllocator m_allocator{};
        static inline cerb::ConstBitmap<1, 1000000> rec{};


        static constexpr auto construct_node(const T &value) -> ParserNode *
        {
            ParserNode *new_node = NodeTraits::allocate(m_allocator, 1);
            NodeTraits::construct(m_allocator, new_node, value);
            return new_node;
        }

        static constexpr auto construct_node() -> ParserNode *
        {
            ParserNode *new_node = NodeTraits::allocate(m_allocator, 1);
            NodeTraits::construct(m_allocator, new_node);
            return new_node;
        }

        static constexpr auto construct_root(
            ParserNode *root, const std::initializer_list<ParserNode *> &children)
            -> ParserNode *
        {
            CERBLIB_UNROLL_N(2)
            for (const auto child : children) {
                child->root = root;
                root->children.push_back(child);
            }
            return root;
        }

        static auto print(
            const ParserNode *node, size_t depth = 0)
            -> void
        {
            if (node == nullptr) {
                return;
            }

            size_t i = 0;
            putchar('\t');
            rec.template clear<0>();

            CERBLIB_UNROLL_N(2)
            for (; i < depth; i++) {
                if (i == depth - 1) {
                    fmt::print(
                        "{}\u2014\u2014\u2014", rec.at<0>(depth - 1) ? "\u0371" : "\u221F");
                } else {
                    fmt::print("{}   ", rec.at<0>(i) ? "\u23B8" : "  ");
                }
            }

            std::cout << node->value << std::endl;

            CERBLIB_UNROLL_N(2)
            for (const auto &elem: node->children) {
                rec.set<1, 0>(depth);
                print(elem, depth + 1);
            }
        }

        constexpr ParserNode(const T &value_) : value(value_)
        {}

        constexpr ParserNode() = default;
    };
}// namespace cerb::analysis::parser

cerb::ConstBitmap<1, 1000000> rec{};

template<typename T, typename Alloc>
void printTree(
    const cerb::analysis::parser::TreeNode<T, Alloc> *node, size_t depth = 0)
{
    size_t i = 0;

    if (node == nullptr) {
        return;
    }

    putchar('\t');

    CERBLIB_UNROLL_N(2)
    for (; i < depth; i++) {
        if (i == depth - 1) {
            fmt::print(
                "{}\u2014\u2014\u2014", rec.at<0>(depth - 1) ? "\u0371" : "\u221F");
        } else {
            fmt::print("{}   ", rec.at<0>(i) ? "\u23B8" : "  ");
        }
    }

    std::cout << node->value << std::endl;
    rec.set<1, 0>(depth);
    printTree(node->left, depth + 1);
    rec.set<0, 0>(depth);
    printTree(node->right, depth + 1);
}


#endif /* CERBERUS_TREE_HPP */
