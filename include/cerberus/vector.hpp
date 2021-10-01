#ifndef CERBERUS_VECTOR_HPP
#define CERBERUS_VECTOR_HPP

#include <cerberus/private/basic_vector.hpp>

namespace cerb {
    template<typename T, typename Alloc = std::allocator<T>>
    struct Vector : private PRIVATE::BasicVector<T, Alloc>
    {
        using parent = PRIVATE::BasicVector<T, Alloc>;
        using parent::back;
        using parent::begin;
        using parent::capacity;
        using parent::cbegin;
        using parent::cend;
        using parent::clear;
        using parent::data;
        using parent::emplace_back;
        using parent::end;
        using parent::erase;
        using parent::insert;
        using parent::pop_back;
        using parent::push_back;
        using parent::rbegin;
        using parent::rend;
        using parent::size;

        using typename parent::const_reference;
        using typename parent::iterator;
        using typename parent::reference;
        using typename parent::reverse_iterator;
        using typename parent::size_type;

    public:
        constexpr friend auto operator==(const Vector &lhs, Vector &rhs)
        {
            return operator==(lhs.self(), rhs.self());
        }

        constexpr friend auto operator!=(const Vector &lhs, Vector &rhs)
        {
            return operator!=(lhs.self(), rhs.self());
        }

        constexpr friend auto operator>(const Vector &lhs, Vector &rhs)
        {
            return operator>(lhs.self(), rhs.self());
        }

        constexpr friend auto operator>=(const Vector &lhs, Vector &rhs)
        {
            return operator>=(lhs.self(), rhs.self());
        }

        constexpr friend auto operator<(const Vector &lhs, Vector &rhs)
        {
            return operator<(lhs.self(), rhs.self());
        }

        constexpr friend auto operator<=(const Vector &lhs, Vector &rhs)
        {
            return operator<=(lhs.self(), rhs.self());
        }

    public:
        constexpr auto at(size_type index) const -> reference
        {
            return data()[index];
        }

        constexpr auto operator[](size_type index) -> reference
        {
            return data()[index];
        }

        constexpr auto operator[](size_type index) const -> reference
        {
            return data()[index];
        }

    public:
        constexpr auto operator=(const Vector &) -> Vector & = default;
        constexpr auto operator=(Vector &&) noexcept -> Vector & = default;

    public:
        constexpr Vector()  = default;
        constexpr ~Vector() = default;

        constexpr Vector(const Vector &)     = default;
        constexpr Vector(Vector &&) noexcept = default;

        constexpr Vector(const std::initializer_list<T> &args) : parent(args)
        {}
    };
}// namespace cerb

#endif /* CERBERUS_VECTOR_HPP */
