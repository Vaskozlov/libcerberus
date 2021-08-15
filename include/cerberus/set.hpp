#ifndef CERBERUS_SET_HPP
#define CERBERUS_SET_HPP

#include <memory>
#include <functional>
#include <cerberus/pair.hpp>
#include <cerberus/operators.hpp>
#include <cerberus/redBlackTree.hpp>

namespace cerb {
    template<typename T, auto OnThrowing = cerb::Throwable {}, typename Compare = less<void>, typename Alloc = std::allocator<T>>
    class Set : public PRIVATE::RBTree<T, Compare, Alloc> {
        using PRIVATE::RBTree<T, Compare, Alloc>::search;
        using PRIVATE::RBTree<T, Compare, Alloc>::RBTreeErase;
        using PRIVATE::RBTree<T, Compare, Alloc>::RBTreeEmplace;
        using Node = typename PRIVATE::RBTree<T, Compare, Alloc>::Node;

    private:
        constexpr static auto MayThrow = std::is_same_v<Throwable, decltype(OnThrowing)>;

    public:
        constexpr auto count(const T& key) {
            auto value = search(key);
            return value != nullptr && value->value == key;
        }

        constexpr auto insert(const T& value) noexcept {
            this->template RBTreeEmplace<true>(value);
        }

        constexpr auto erase(const T& key) noexcept(!MayThrow) {
            if constexpr (MayThrow) {
                if (!RBTreeErase(key)) UNLIKELY {
                    throw std::out_of_range("Unable to erase elem from cerb::Set");
                }
                return true;
            } else {
                return RBTreeErase(key);
            }
        }
    };

    template<typename T, auto OnThrowing = cerb::Throwable {}, typename Compare = cerb::less<void>, typename Alloc = std::allocator<T>>
    class Multiset : public PRIVATE::RBTree<Pair<size_t, T, BY_SECOND_VALUE>, Compare, Alloc> {
        using value_type = Pair<size_t, T, BY_SECOND_VALUE>;
        using PRIVATE::RBTree<value_type, Compare, Alloc>::search;
        using PRIVATE::RBTree<value_type, Compare, Alloc>::RBTreeErase;
        using PRIVATE::RBTree<value_type, Compare, Alloc>::RBTreeEmplace;
        using PRIVATE::RBTree<value_type, Compare, Alloc>::deleteNode;
        using Node = typename PRIVATE::RBTree<value_type, Compare, Alloc>::Node;

    private:
        constexpr static auto MayThrow = std::is_same_v<Throwable, decltype(OnThrowing)>;

    public:
        constexpr auto count(const T& key) const noexcept -> size_t {
            auto value = search(key);

            if (value == nullptr || value->value.second != key) {
                return 0;
            } else {
                return value->value.first;
            }
        }

        constexpr auto insert(const T& value) noexcept {
            this->template RBTreeEmplace<true>(static_cast<size_t>(0), value)->value.first += 1;
        }

        constexpr auto erase(const T &key) noexcept(!MayThrow) {
            auto value = search(key);

            if constexpr (MayThrow) {
                if (value != nullptr && value->value.second == key) LIKELY {
                    value->value.first -= 1;
                } else UNLIKELY {
                    throw std::out_of_range("Unable to erase elem from cerb::Multiset");
                }
            } else {
                if (value != nullptr && value->value.second == key) LIKELY {
                    value->value.first -= 1;
                } else UNLIKELY {
                    return false;
                }
            }

            if (value->value.second == 0) {
                return deleteNode(value);
            }

            return true;
        }
    };

    template<typename T1, typename T2, auto OnThrowing = cerb::Throwable {}, typename Compare = cerb::less<void>, typename Alloc = std::allocator<T1>>
    class Map : public PRIVATE::RBTree<Pair<T1, T2, BY_FIRST_VALUE>, Compare, Alloc> {
        using value_type = Pair<T1, T2, BY_FIRST_VALUE>;
        using PRIVATE::RBTree<value_type, Compare, Alloc>::search;
        using PRIVATE::RBTree<value_type, Compare, Alloc>::RBTreeErase;
        using PRIVATE::RBTree<value_type, Compare, Alloc>::RBTreeEmplace;
        using PRIVATE::RBTree<value_type, Compare, Alloc>::deleteNode;
        using Node = typename PRIVATE::RBTree<value_type, Compare, Alloc>::Node;

    private:
        constexpr static auto MayThrow = std::is_same_v<Throwable, decltype(OnThrowing)>;

    public:
        constexpr auto count(const T1& key) const noexcept -> size_t {
            auto value = search(key);
            return value != nullptr && value->value == key;
        }

        constexpr auto erase(const T1& key) noexcept(!MayThrow) {
            if constexpr (MayThrow) {
                if (!RBTreeErase(key)) UNLIKELY {
                    throw std::out_of_range("Unable to erase elem from cerb::Set");
                }
                return true;
            } else {
                return RBTreeErase(key);
            }
        }

        constexpr auto operator[](const T1 &key) noexcept(!MayThrow) -> T2&{
            auto value = search(key);

            if (value != nullptr && value->value.first == key) {
                return value->value.second;
            } else {
                auto elem = this->template RBTreeEmplace<false>(key);
                std::construct_at(&elem->value.first, key);
                return elem->value.second;
            }
        }
    };
}

#endif /* CERBERUS_SET_HPP */
