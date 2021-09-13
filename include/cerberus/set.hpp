#ifndef CERBERUS_SET_HPP
#define CERBERUS_SET_HPP

#include <memory>
#include <functional>
#include <cerberus/pair.hpp>
#include <cerberus/operators.hpp>
#include <cerberus/redBlackTree.hpp>

namespace cerb {
    namespace gl {
        template<typename T, size_t Size, bool MayThrow = true>
        class Set : public PRIVATE::gl::BasicSet<T, Size, MayThrow>
        {
            using parent           = PRIVATE::gl::BasicSet<T, Size>;
            using value_type       = typename parent::value_type;
            using const_value_type = typename parent::const_value_type;

        public:
            using parent::begin;
            using parent::cbegin;
            using parent::cend;
            using parent::clear;
            using parent::contains;
            using parent::crbegin;
            using parent::crend;
            using parent::data;
            using parent::end;
            using parent::erase;
            using parent::hidden;
            using parent::hide;
            using parent::insert;
            using parent::rbegin;
            using parent::rend;
            using parent::self;
            using parent::show;
            using parent::size;
            using parent::last;

            using iterator               = typename parent::iterator;
            using const_iterator         = typename parent::const_iterator;
            using reverse_iterator       = typename parent::reverse_iterator;
            using const_reverse_iterator = typename parent::const_reverse_iterator;

        public:
            constexpr auto operator=(const Set &) -> Set & = default;
            constexpr auto operator=(Set &&) noexcept -> Set & = default;

        public:
            constexpr Set() = default;

            constexpr Set(const Set &)     = default;
            constexpr Set(Set &&) noexcept = default;

            constexpr Set(const std::initializer_list<const_value_type> &args)
              : parent(args)
            {}
        };
    }// namespace gl

    template<
        typename T, bool MayThrow = true, typename Compare = less<void>,
        typename Alloc = std::allocator<T>>
    class CERBLIB_TRIVIAL Set : public PRIVATE::RBTree<T, Compare, Alloc>
    {
        using BasicTree = PRIVATE::RBTree<T, Compare, Alloc>;
        using Node      = typename BasicTree::Node;

        using BasicTree::RBTreeEmplace;
        using BasicTree::RBTreeErase;
        using BasicTree::search;
        using BasicTree::RBTreeEmplaceKey;

    public:
        constexpr auto count(const T &key) const
        {
            auto value = search(key);
            return value != nullptr && value->value == key;
        }

        constexpr auto insert(T &&value) noexcept
        {
            RBTreeEmplace(value);
        }

        constexpr auto insert(const T &value) noexcept
        {
            RBTreeEmplace(value);
        }

        template<typename... Ts>
        constexpr auto emplace(Ts&&... values) noexcept
        {
            RBTreeEmplace(values...);
        }

        template<typename U>
        constexpr auto insert(const U& key, T &&value) noexcept
        {
            RBTreeEmplaceKey(value);
        }

        template<typename U>
        constexpr auto insert(const U& key, const T &value) noexcept
        {
            RBTreeEmplaceKey(key, value);
        }

        template<typename U, typename... Ts>
        constexpr auto emplaceKey(const U& key, Ts&&... values) noexcept
        {
            RBTreeEmplaceKey(key, values...);
        }

        constexpr auto erase(const T &key) noexcept(!MayThrow)
        {
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
        constexpr auto operator=(const Set &) -> Set & = default;
        constexpr auto operator=(Set &&) noexcept -> Set & = default;

    public:
        constexpr Set() noexcept  = default;
        constexpr ~Set() noexcept = default;

        constexpr Set(const Set &)     = default;
        constexpr Set(Set &&) noexcept = default;

        constexpr Set(const std::initializer_list<T> &values) noexcept
        {
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : values) {
                insert(elem);
            }
        }
    };

    template<
        typename T, bool MayThrow = true, typename Compare = less<void>,
        typename Alloc = std::allocator<T>>
    class CERBLIB_TRIVIAL Multiset
      : public PRIVATE::RBTree<Pair<size_t, T, BY_SECOND_VALUE>, Compare, Alloc>
    {
        using value_type = Pair<size_t, T, BY_SECOND_VALUE>;
        using BasicTree  = PRIVATE::RBTree<value_type, Compare, Alloc>;
        using Node       = typename BasicTree::Node;

        using BasicTree::deleteNode;
        using BasicTree::RBTreeEmplace;
        using BasicTree::RBTreeErase;
        using BasicTree::search;

    public:
        constexpr auto count(const T &key) const noexcept -> size_t
        {
            auto value = search(key);

            if (value == nullptr || value->value.second != key) {
                return 0;
            }

            return value->value.first;
        }

        constexpr auto insert(T &&value) noexcept
        {
            RBTreeEmplace(static_cast<size_t>(0), value)->value.first += 1;
        }

        constexpr auto insert(const T &value) noexcept
        {
            RBTreeEmplace(static_cast<size_t>(0), value)->value.first += 1;
        }

        constexpr auto erase(const T &key) noexcept(!MayThrow)
        {
            auto value = search(key);

            if constexpr (MayThrow) {
                if (value != nullptr && value->value.second == key) [[likely]] {
                    value->value.first -= 1;
                } else [[unlikely]] {
                    throw std::out_of_range(
                        "Unable to erase elem from cerb::Multiset");
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
        constexpr auto operator=(const Multiset &) -> Multiset & = default;
        constexpr auto operator=(Multiset &&) noexcept -> Multiset & = default;

    public:
        constexpr Multiset() noexcept  = default;
        constexpr ~Multiset() noexcept = default;

        constexpr Multiset(const Multiset &)     = default;
        constexpr Multiset(Multiset &&) noexcept = default;

        constexpr Multiset(const std::initializer_list<T> &values) noexcept
        {
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : values) {
                insert(elem);
            }
        }
    };
}// namespace cerb

#endif /* CERBERUS_SET_HPP */
