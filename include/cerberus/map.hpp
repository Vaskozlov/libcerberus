#ifndef CERBERUS_MAP_HPP
#define CERBERUS_MAP_HPP

#include <stdexcept>
#include <functional>
#include <memory>
#include <initializer_list>
#include <cerberus/types.h>
#include <cerberus/Pair.hpp>
#include <cerberus/string.hpp>
#include <cerberus/allocator.hpp>
#include <cerberus/algorithms.hpp>

namespace cerb::PRIVATE::gl {
    template<typename T, size_t Size>
    struct basic_set {
        using value_type = T;
        using const_value_type = const T;
        using storage_t = std::array<value_type, Size>;

        using iterator = typename storage_t::iterator;
        using const_iterator = typename storage_t::const_iterator;

        using reverse_iterator = typename storage_t::reverse_iterator;
        using const_reverse_iterator = typename storage_t::const_reverse_iterator;

    protected:
        size_t m_size { 0 };
        storage_t m_data {};

    public:
        constexpr auto begin() noexcept -> iterator {
            return m_data.begin();
        }

        constexpr auto end() noexcept -> iterator {
            return m_data.begin() + m_size;
        }

        constexpr auto begin() const noexcept -> const_iterator {
            return m_data.begin();
        }

        constexpr auto end() const noexcept -> const_iterator {
            return m_data.begin() + m_size;
        }

        constexpr auto rbegin() noexcept -> reverse_iterator {
            return reverse_iterator(begin());
        }

        constexpr auto rend() noexcept -> reverse_iterator {
            return reverse_iterator(end());
        }

        constexpr auto rbegin() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(begin());
        }

        constexpr auto rend() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(end());
        }

        constexpr auto cbegin() noexcept -> const_iterator {
            return m_data.cbegin();
        }

        constexpr auto cend() noexcept -> const_iterator {
            return m_data.cbegin() + m_size;
        }

        constexpr auto crbegin() noexcept -> const_reverse_iterator {
            return const_reverse_iterator(cbegin());
        }

        constexpr auto crend() noexcept -> const_reverse_iterator {
            return const_reverse_iterator(cend());
        }

    public:
        constexpr auto self() noexcept -> basic_set& {
            return *this;
        }

        constexpr auto self() const noexcept -> const basic_set& {
            return *this;
        }

    public:
        constexpr basic_set &operator=(const basic_set& other) noexcept {
            if (this == &other) {
                return *this;
            }

            m_size = other.m_size;
            cerb::memcpy(m_data, other.m_data, m_size);
            return *this;
        }

        constexpr basic_set &operator=(basic_set&& other) noexcept {
            m_size = other.m_size;
            cerb::memcpy(m_data, other.m_data, m_size);
            return *this;
        }

    public:
        constexpr basic_set() noexcept = default;

        constexpr basic_set(const basic_set &other) noexcept
        : m_size(other.m_size)
        {
            cerb::memcpy(m_data, other.m_data, m_size);
        }

        constexpr basic_set(basic_set&& other) noexcept
        : m_size(other.m_size)
        {
            cerb::memcpy(m_data, other.m_data, m_size);
        }

        constexpr basic_set(const std::initializer_list<const_value_type> &args) noexcept
        {
            for (const auto& elem: args) {
                m_data[m_size++] = std::move(elem);
            }
        }

        template<typename... Ts>
        explicit constexpr basic_set(Ts&&... args) noexcept
        {
            cerb::forEach<false>([&](const auto &elem){
                m_data[m_size++] = std::move(elem);
            }, args...);
        }

        constexpr ~basic_set() noexcept = default;
    };
}

namespace cerb::gl {
    template<typename T, size_t Size>
    class set : public cerb::PRIVATE::gl::basic_set<T, Size> {
        using value_type = T;
        using const_value_type = const T;
        using base_class = cerb::PRIVATE::gl::basic_set<T, Size>;

        using base_class::begin;
        using base_class::end;
        using base_class::m_data;
        using base_class::m_size;
        using base_class::self;

    private:
        constexpr auto search(const_value_type& key) noexcept {
            return cerb::find_if(begin(), end(), [&key](const auto &i) {
                return i == key;
            });
        }

        constexpr auto search(const_value_type& key) const noexcept {
            return cerb::find_if(begin(), end(), [&key](const auto &i) {
                return i == key;
            });
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

                for (const auto back = end(); elem != back; ++elem) {
                    *elem = *(elem + 1);
                }
            }
        }

        constexpr auto contains(const_value_type &value) const noexcept -> bool {
            return search(value) != end();
        }

    public:
        constexpr set &operator=(set&& other) noexcept = default;
        constexpr set &operator=(const set& other) noexcept = default;

    public:
        constexpr set(const set &other) noexcept
        : base_class(other.self())
        {}

        constexpr set(set&& other) noexcept
        : base_class(std::move(other.self()))
        {}

        constexpr set(const std::initializer_list<const_value_type> &args) noexcept
        : base_class(args)
        {}

        template<typename... Ts>
        explicit constexpr set(Ts&&... args) noexcept
        : base_class(args...)
        {}
    };

    template<typename T1, typename T2, size_t Size>
    class map : public cerb::PRIVATE::gl::basic_set<cerb::Pair<T1, T2>, Size> {
        using key_type = T1;
        using value_type = T2;
        using const_key_type = const T1;
        using const_value_type = const T2;
        using map_elem = cerb::Pair<key_type , value_type>;
        using const_map_elem = const cerb::Pair<key_type , value_type>;
        using parent_class = cerb::PRIVATE::gl::basic_set<cerb::Pair<T1, T2>, Size>;

        using parent_class::begin;
        using parent_class::end;
        using parent_class::m_data;
        using parent_class::m_size;
        using parent_class::self;

    private:
        constexpr auto search(const_key_type& key) noexcept {
            return cerb::find_if(begin(), end(), [&key](const auto &i) {
                return i.first == key;
            });
        }

        constexpr auto search(const_key_type& key) const noexcept {
            return cerb::find_if(begin(), end(), [&key](const auto &i) {
                return i.first == key;
            });
        }
    public:
        constexpr auto at(const_key_type& key) const noexcept -> const_value_type& {
            const auto elem = search(key);
            return elem->second;
        }

        constexpr auto operator[](const_key_type& key) const noexcept -> const_value_type& {
            return at(key);
        }

        constexpr auto operator[](const_key_type& key) noexcept -> value_type& {
            auto elem = static_cast<size_t>(search(key) - begin());

            if (elem == m_size) {
                m_data[m_size++] = map_elem {key};
            }

            return m_data[elem].second;
        }

    public:
        constexpr map &operator=(map&& other) noexcept = default;
        constexpr map &operator=(const map& other) noexcept = default;

    public:
        constexpr map(const map &other) noexcept
        : parent_class(other.self())
        {}

        constexpr map(map&& other) noexcept
        : parent_class(std::move(other.self()))
        {}

        constexpr map(const std::initializer_list<const_map_elem> &args) noexcept
        : parent_class(args)
        {}

        template<typename... Ts>
        explicit constexpr map(Ts&&... args) noexcept
        : parent_class(args...)
        {}
    };
}

namespace cerb::cx {
    template<typename T1, typename T2, typename Alloc = cerb::ConstAllocator<cerb::Pair<T1, T2>>>
    class Map {
        using key_type = T1;
        using value_type = T2;
        using const_key_type = const T1;
        using const_value_type = const T2;
        using map_elem = cerb::Pair<key_type , value_type>;
        using difference_type = ptrdiff_t;
        using iterator = map_elem*;
        using const_iterator = const map_elem*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    private:
        map_elem *m_data;
        size_t m_size { 0 };
        size_t m_capacity { 0 };
        Alloc m_allocator { };

    public:
        constexpr auto begin() noexcept -> iterator {
            return m_data;
        }

        constexpr auto end() noexcept -> iterator {
            return m_data + m_size;
        }

        constexpr auto begin() const noexcept -> const_iterator {
            return m_data;
        }

        constexpr auto end() const noexcept -> const_iterator {
            return m_data + m_size;
        }

        constexpr auto rbegin() noexcept -> reverse_iterator {
            return reverse_iterator(begin());
        }

        constexpr auto rend() noexcept -> reverse_iterator {
            return reverse_iterator(end());
        }

        constexpr auto rbegin() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(begin());
        }

        constexpr auto rend() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(end());
        }

        constexpr auto cbegin() noexcept -> const_iterator {
            return m_data;
        }

        constexpr auto cend() noexcept -> const_iterator {
            return m_data + m_size;
        }

        constexpr auto crbegin() noexcept -> const_reverse_iterator {
            return const_reverse_iterator(cbegin());
        }

        constexpr auto crend() noexcept -> const_reverse_iterator {
            return const_reverse_iterator(cend());
        }

    private:
        constexpr auto realloc(size_t new_size) noexcept -> void {
            m_capacity = new_size;
            auto new_data = m_allocator.allocate(m_capacity);
            cerb::memcpy(new_data, m_data, m_size);
            delete[] m_data;
            m_data = new_data;
        }

        constexpr auto search(const_key_type& key) noexcept {
            return cerb::find_if(begin(), end(), [&key](const auto &i) {
                return i.first == key;
            });
        }

        constexpr auto search(const_key_type& key) const noexcept {
            return cerb::find_if(begin(), end(), [&key](const auto &i) {
                return i.first == key;
            });
        }

        constexpr auto push(const map_elem& new_elem) {
            const auto elem = search(new_elem.first);

            if (m_size == m_capacity) {
                realloc(m_capacity * 2U);
            }

            if (elem == end()) {
                m_data[m_size++] = new_elem;
            }
        }

    public:
        constexpr auto at(const_key_type& key) const noexcept -> const_value_type& {
            const auto elem = search(key);
            return elem->second;
        }

        constexpr auto operator[](const_key_type& key) const noexcept -> const_value_type& {
            return at(key);
        }

        constexpr auto operator[](const_key_type& key) noexcept -> value_type& {
            auto elem = static_cast<size_t>(search(key) - begin());

            if (m_size == m_capacity) {
                realloc(m_capacity * 2U);
            }

            if (elem == m_size) {
                m_data[m_size++] = map_elem {key};
            }

            return m_data[elem].second;
        }

    public:
        constexpr Map &operator=(const Map &other) noexcept {
            if (this == &other) {
                return *this;
            }

            m_allocator = other.m_allocator;

            if (m_capacity < other.m_capacity) {
                realloc(other.m_capacity);
            }

            m_size = other.m_size;
            m_capacity = other.m_capacity;

            cerb::memcpy(m_data, other.m_data);
            return *this;
        }

        constexpr Map &operator=(Map &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            m_allocator = std::move(other.m_allocator);

            if (m_capacity < other.m_capacity) {
                realloc(other.m_capacity);
            }

            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m_data = nullptr;

            return *this;
        }

    public:
        constexpr Map() noexcept = default;

        constexpr Map(const Map& other) noexcept
        : m_size(other.m_size), m_capacity(other.m_capacity), m_allocator(other.m_allocator)
        {
            m_data = m_allocator.allocate(m_capacity);
            cerb::memcpy(m_data, other.m_data, m_size);
        }

        constexpr Map(Map&& other) noexcept
        : m_size(other.m_size), m_capacity(other.m_capacity),
          m_allocator(std::move(other.m_allocator)), m_data(other.m_data)
        {
            other.m_data = nullptr;
        }

        constexpr Map(const std::initializer_list<const map_elem> &args) noexcept
        : m_data(m_allocator.allocate(args.end() - args.begin())), m_capacity(args.end() - args.begin())
        {
            for (const auto& elem: args) {
                push(elem);
            }
        }

        template<typename... Ts>
        constexpr explicit Map(Ts... args) noexcept
        : m_data(m_allocator.allocate(sizeof...(args))), m_capacity(sizeof...(args))
        {
            cerb::forEach<false>([&](const auto &elem){
                push(elem);
            }, args...);
        }

        constexpr ~Map() noexcept {
            delete[] m_data;
        }
    };
}
#endif /* CERBERUS_MAP_HPP */
