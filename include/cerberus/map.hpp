#ifndef CERBERUS_MAP_HPP
#define CERBERUS_MAP_HPP

#include <memory>
#include <functional>
#include <initializer_list>
#include <cerberus/types.h>
#include <cerberus/pair.hpp>
#include <cerberus/string.hpp>
#include <cerberus/algorithms.hpp>
#include <cerberus/redBlackTree.hpp>

namespace cerb {
    /*
    namespace gl {
        template<typename T1, typename T2, size_t Size>
        class Map : public PRIVATE::gl::BasicSet<Pair<T1, T2>, Size>
        {
            using key_type         = T1;
            using value_type       = T2;
            using const_key_type   = const T1;
            using const_value_type = const T2;
            using map_elem         = Pair<key_type, value_type>;
            using const_map_elem   = const Pair<key_type, value_type>;
            using parent_class     = PRIVATE::gl::BasicSet<Pair<T1, T2>, Size>;

            using parent_class::begin;
            using parent_class::end;
            using parent_class::m_data;
            using parent_class::m_size;
            using parent_class::self;

        private:
            constexpr auto search(const_key_type &key) noexcept
            {
                return cerb::find_if(begin(), end(), [&key](const auto &i) {
                    return i.first == key;
                });
            }

            constexpr auto search(const_key_type &key) const noexcept
            {
                return cerb::find_if(begin(), end(), [&key](const auto &i) {
                    return i.first == key;
                });
            }

        public:
            constexpr auto at(const_key_type &key) const noexcept
                -> const_value_type &
            {
                const auto elem = search(key);
                return elem->second;
            }

            constexpr auto operator[](const_key_type &key) const noexcept
                -> const_value_type &
            {
                return at(key);
            }

            constexpr auto operator[](const_key_type &key) noexcept -> value_type &
            {
                auto elem = static_cast<size_t>(search(key) - begin());

                if (elem == m_size) {
                    m_data[m_size++] = map_elem{ key };
                }

                return m_data[elem].second;
            }

        public:
            constexpr auto operator=(Map &&other) noexcept -> Map & = default;
            constexpr auto operator=(const Map &other) noexcept -> Map & = default;

        public:
            constexpr Map(const Map &other) noexcept : parent_class(other.self())
            {}

            constexpr Map(Map &&other) noexcept
              : parent_class(std::move(other.self()))
            {}

            constexpr Map(const std::initializer_list<const_map_elem> &args) noexcept
              : parent_class(args)
            {}

            template<typename... Ts>
            explicit constexpr Map(Ts &&...args) noexcept : parent_class(args...)
            {}
        };
    }// namespace gl
    */

    namespace gl {
        template<typename T1, typename T2, size_t Size, bool MayThrow = true>
        class Map
          : public PRIVATE::gl::BasicSet<
                Pair<T1, T2, BY_FIRST_VALUE>, Size, MayThrow>
        {
            using key_type         = T1;
            using value_type       = T2;
            using const_key_type   = const T1;
            using const_value_type = const T2;
            using map_elem         = Pair<T1, T2, BY_FIRST_VALUE>;
            using const_map_elem   = const Pair<T1, T2, BY_FIRST_VALUE>;
            using parent           = PRIVATE::gl::BasicSet<map_elem, Size, MayThrow>;

        private:
            using parent::search;

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
            using parent::last;
            using parent::rbegin;
            using parent::rend;
            using parent::self;
            using parent::show;
            using parent::size;

            using iterator               = typename parent::iterator;
            using const_iterator         = typename parent::const_iterator;
            using reverse_iterator       = typename parent::reverse_iterator;
            using const_reverse_iterator = typename parent::const_reverse_iterator;

        public:
            constexpr auto at(const_key_type &key) const -> const value_type &
            {
                auto elem = search(key);

                if (elem == end()) {
                    if constexpr (MayThrow) {
                        if (elem >= begin() + Size) {
                            throw std::out_of_range("cerb::Map is full");
                        }
                    } else {
                        insert(map_elem(key));
                    }
                }

                return elem->second;
            }

            constexpr auto operator[](const_key_type &key) -> value_type &
            {
                auto elem = search(key);

                if (elem == end()) {
                    if constexpr (MayThrow) {
                        if (elem >= begin() + Size) {
                            throw std::out_of_range("cerb::Map is full");
                        }
                    }
                    insert(map_elem(key));
                }

                return elem->second;
            }

            constexpr auto operator[](const_key_type &key) const -> const value_type &
            {
                return at(key);
            }

        public:
            constexpr auto operator=(const Map &) -> Map & = default;
            constexpr auto operator=(Map &&) noexcept -> Map & = default;

        public:
            constexpr Map() = default;

            constexpr Map(const Map &)     = default;
            constexpr Map(Map &&) noexcept = default;

            constexpr Map(const std::initializer_list<const_map_elem> &args)
              : parent(args)
            {}
        };
    }// namespace gl

    template<
        typename T1, typename T2, bool MayThrow = true,
        typename Compare = less<void>, typename Alloc = std::allocator<T1>>
    class CERBLIB_TRIVIAL Map
      : public PRIVATE::RBTree<Pair<const T1, T2, BY_FIRST_VALUE>, Compare, Alloc>
    {
        using value_type = Pair<const T1, T2, BY_FIRST_VALUE>;
        using BasicTree  = PRIVATE::RBTree<value_type, Compare, Alloc>;
        using Node = typename PRIVATE::RBTree<value_type, Compare, Alloc>::Node;

        using BasicTree::deleteNode;
        using BasicTree::RBTreeEmplace;
        using BasicTree::RBTreeErase;
        using BasicTree::search;

    public:
        constexpr auto insert(value_type &&value) noexcept
        {
            this->template RBTreeEmplace<true, true>(value);
        }

        constexpr auto insert(const value_type &value) noexcept
        {
            this->template RBTreeEmplace<true, true>(value);
        }

        constexpr auto count(const T1 &key) const noexcept -> size_t
        {
            auto value = search(key);
            return value != nullptr && value->value == key;
        }

        constexpr auto erase(const T1 &key) noexcept(!MayThrow)
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

        constexpr auto operator[](const T1 &key) noexcept(!MayThrow) -> T2 &
        {
            auto value = search(key);

            if (value != nullptr && value->value.first == key) {
                return value->value.second;
            }

            auto elem = RBTreeEmplace(key);
            return elem->value.second;
        }

    public:
        constexpr auto operator=(const Map &) -> Map & = default;
        constexpr auto operator=(Map &&) noexcept -> Map & = default;

    public:
        constexpr Map() noexcept  = default;
        constexpr ~Map() noexcept = default;

        constexpr Map(const Map &)     = default;
        constexpr Map(Map &&) noexcept = default;

        constexpr Map(const std::initializer_list<value_type> &values) noexcept
        {
            CERBLIB_UNROLL_N(2)
            for (const auto &elem : values) {
                insert(elem);
            }
        }
    };
}// namespace cerb
#endif /* CERBERUS_MAP_HPP */
