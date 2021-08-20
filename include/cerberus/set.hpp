#ifndef CERBERUS_SET_HPP
#define CERBERUS_SET_HPP

#include <memory>
#include <functional>
#include <cerberus/pair.hpp>
#include <cerberus/operators.hpp>
#include <cerberus/redBlackTree.hpp>

namespace cerb {
    namespace gl {
        template<typename T, size_t Size>
        class Set : public PRIVATE::gl::BasicSet<T, Size>
        {
            using value_type       = T;
            using const_value_type = const T;
            using base_class       = PRIVATE::gl::BasicSet<T, Size>;

            using base_class::begin;
            using base_class::end;
            using base_class::m_data;
            using base_class::m_size;
            using base_class::self;

        private:
            constexpr auto search(const_value_type &key) noexcept {
                return find_if(begin(), end(),
                               [&key](const auto &i) { return i == key; });
            }

            constexpr auto search(const_value_type &key) const noexcept {
                return find_if(begin(), end(),
                               [&key](const auto &i) { return i == key; });
            }

        public:
            constexpr auto insert(const_value_type &new_elem) noexcept {
                auto elem = search(new_elem);

                if (elem == end()) {
                    m_data[m_size++] = new_elem;
                }
            }

            constexpr auto erase(const_value_type &value2erase) {
                auto elem = search(value2erase);

                if (elem != end()) {
                    --m_size;

                    CERBLIB_UNROLL_N(2)
                    for (const auto back = end(); elem != back; ++elem) {
                        *elem = *(elem + 1);
                    }
                }
            }

            constexpr auto contains(const_value_type &value) const noexcept -> bool {
                return search(value) != end();
            }

        public:
            constexpr auto operator=(Set &&other) noexcept -> Set & = default;
            constexpr auto operator=(const Set &other) noexcept -> Set & = default;

        public:
            constexpr Set(const Set &other) noexcept : base_class(other.self()) {}

            constexpr Set(Set &&other) noexcept
              : base_class(std::move(other.self())) {}

            constexpr Set(
                const std::initializer_list<const_value_type> &args) noexcept
              : base_class(args) {}

            template<typename... Ts>
            explicit constexpr Set(Ts &&...args) noexcept : base_class(args...) {}
        };
    }// namespace gl

    template<typename T, auto OnThrowing = Throwable{},
             typename Compare = less<void>, typename Alloc = std::allocator<T>>
    class CERBLIB_TRIVIAL Set : public PRIVATE::RBTree<T, Compare, Alloc>
    {
        using BasicTree = PRIVATE::RBTree<T, Compare, Alloc>;
        using Node      = typename BasicTree::Node;

        using BasicTree::RBTreeEmplace;
        using BasicTree::RBTreeErase;
        using BasicTree::search;

    private:
        constexpr static auto MayThrow =
            std::is_same_v<Throwable, decltype(OnThrowing)>;

    public:
        constexpr auto count(const T &key) {
            auto value = search(key);
            return value != nullptr && value->value == key;
        }

        constexpr auto insert(T &&value) noexcept { RBTreeEmplace(value); }

        constexpr auto insert(const T &value) noexcept { RBTreeEmplace(value); }

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
        constexpr auto operator=(const Set &) -> Set & = default;
        constexpr auto operator=(Set &&) noexcept -> Set & = default;

    public:
        constexpr Set() noexcept  = default;
        constexpr ~Set() noexcept = default;

        constexpr Set(const Set &)     = default;
        constexpr Set(Set &&) noexcept = default;

        constexpr Set(const std::initializer_list<T> &values) noexcept {
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : values) {
                insert(elem);
            }
        }
    };

    template<typename T, auto OnThrowing = Throwable{},
             typename Compare = less<void>, typename Alloc = std::allocator<T>>
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

    private:
        constexpr static auto MayThrow =
            std::is_same_v<Throwable, decltype(OnThrowing)>;

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

        constexpr Multiset(const std::initializer_list<T> &values) noexcept {
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : values) {
                insert(elem);
            }
        }
    };
}// namespace cerb

#endif /* CERBERUS_SET_HPP */
