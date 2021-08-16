#ifndef CERBERUS_SET_HPP
#define CERBERUS_SET_HPP

#include <memory>
#include <functional>
#include <cerberus/pair.hpp>
#include <cerberus/operators.hpp>
#include <cerberus/redBlackTree.hpp>

namespace cerb {
    template<typename T, auto OnThrowing = cerb::Throwable{}, typename Compare = less<void>, typename Alloc = std::allocator<T>>
    class Set : public PRIVATE::RBTree<T, Compare, Alloc>
    {
        using BasicTree = PRIVATE::RBTree<T, Compare, Alloc>;
        using Node      = typename BasicTree::Node;

        using BasicTree::RBTreeEmplace;
        using BasicTree::RBTreeErase;
        using BasicTree::search;

    private:
        constexpr static auto MayThrow = std::is_same_v<Throwable, decltype(OnThrowing)>;

    public:
        constexpr auto count(const T &key) {
            auto value = search(key);
            return value != nullptr && value->value == key;
        }

        constexpr auto insert(T &&value) noexcept {
            RBTreeEmplace(value);
        }

        constexpr auto insert(const T &value) noexcept {
            RBTreeEmplace(value);
        }

        constexpr auto erase(const T &key) noexcept(!MayThrow) {
            if constexpr (MayThrow) {
                if (!RBTreeErase(key)) [[unlikely]] {
                    throw std::out_of_range("Unable to erase elem from cerb::Set");
                }
                return true;
            } else {
                return RBTreeErase(key);
            }
        }

    public:
        constexpr Set() noexcept  = default;
        constexpr ~Set() noexcept = default;

        constexpr Set(const std::initializer_list<T> &values) noexcept {
            CERBLIB_UNROLL_N(2)
            for (auto &elem : values) {
                insert(std::move(elem));
            }
        }
    };

    template<typename T, auto OnThrowing = cerb::Throwable{}, typename Compare = cerb::less<void>, typename Alloc = std::allocator<T>>
    class Multiset : public PRIVATE::RBTree<Pair<size_t, T, BY_SECOND_VALUE>, Compare, Alloc>
    {
        using value_type = Pair<size_t, T, BY_SECOND_VALUE>;
        using BasicTree  = PRIVATE::RBTree<value_type, Compare, Alloc>;
        using Node       = typename BasicTree::Node;

        using BasicTree::deleteNode;
        using BasicTree::RBTreeEmplace;
        using BasicTree::RBTreeErase;
        using BasicTree::search;

    private:
        constexpr static auto MayThrow = std::is_same_v<Throwable, decltype(OnThrowing)>;

    public:
        constexpr auto count(const T &key) const noexcept -> size_t {
            auto value = search(key);

            if (value == nullptr || value->value.second != key) {
                return 0;
            }

            return value->value.first;
        }

        constexpr auto insert(T &&value) noexcept {
            RBTreeEmplace(static_cast<size_t>(0), value)->value.first += 1;
        }

        constexpr auto insert(const T &value) noexcept {
            RBTreeEmplace(static_cast<size_t>(0), value)->value.first += 1;
        }

        constexpr auto erase(const T &key) noexcept(!MayThrow) {
            auto value = search(key);

            if constexpr (MayThrow) {
                if (value != nullptr && value->value.second == key) [[likely]] {
                    value->value.first -= 1;
                } else [[unlikely]] {
                    throw std::out_of_range("Unable to erase elem from cerb::Multiset");
                }
            } else {
                if (value != nullptr && value->value.second == key) [[likely]] {
                    value->value.first -= 1;
                } else [[unlikely]] {
                    return false;
                }
            }

            if (value->value.second == 0) {
                return deleteNode(value);
            }

            return true;
        }

    public:
        constexpr Multiset() noexcept  = default;
        constexpr ~Multiset() noexcept = default;

        constexpr Multiset(const std::initializer_list<T> &values) noexcept {
            CERBLIB_UNROLL_N(2)
            for (auto &elem : values) {
                insert(elem);
            }
        }
    };

    template<typename T1, typename T2, auto OnThrowing = cerb::Throwable{}, typename Compare = cerb::less<void>, typename Alloc = std::allocator<T1>>
    class Map : public PRIVATE::RBTree<Pair<const T1, T2, BY_FIRST_VALUE>, Compare, Alloc>
    {
        using value_type = Pair<const T1, T2, BY_FIRST_VALUE>;
        using BasicTree  = PRIVATE::RBTree<value_type, Compare, Alloc>;
        using Node       = typename PRIVATE::RBTree<value_type, Compare, Alloc>::Node;

        using BasicTree::deleteNode;
        using BasicTree::RBTreeEmplace;
        using BasicTree::RBTreeErase;
        using BasicTree::search;

    private:
        constexpr static auto MayThrow = std::is_same_v<Throwable, decltype(OnThrowing)>;

    public:
        constexpr auto insert(value_type &&value) noexcept {
            this->template RBTreeEmplace<true, true>(value);
        }

        constexpr auto insert(const value_type &value) noexcept {
            this->template RBTreeEmplace<true, true>(value);
        }

        constexpr auto count(const T1 &key) const noexcept -> size_t {
            auto value = search(key);
            return value != nullptr && value->value == key;
        }

        constexpr auto erase(const T1 &key) noexcept(!MayThrow) {
            if constexpr (MayThrow) {
                if (!RBTreeErase(key)) [[unlikely]] {
                    throw std::out_of_range("Unable to erase elem from cerb::Set");
                }
                return true;
            } else {
                return RBTreeErase(key);
            }
        }

        constexpr auto operator[](const T1 &key) noexcept(!MayThrow) -> T2 & {
            auto value = search(key);

            if (value != nullptr && value->value.first == key) {
                return value->value.second;
            } else {
                auto elem = RBTreeEmplace(key);
                return elem->value.second;
            }
        }

    public:
        constexpr Map() noexcept  = default;
        constexpr ~Map() noexcept = default;

        constexpr Map(const std::initializer_list<value_type> &values) noexcept {
            CERBLIB_UNROLL_N(2)
            for (auto &elem : values) {
                insert(std::move(elem));
            }
        }
    };
}// namespace cerb

#endif /* CERBERUS_SET_HPP */
