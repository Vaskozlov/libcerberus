#ifndef CERBERUS_ALLOCATOR_HPP
#define CERBERUS_ALLOCATOR_HPP

#include <cerberus/types.h>

namespace cerb {
    template<typename T>
    struct CERBLIB_TRIVIAL ConstAllocator {
        using value_type = T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

    public:
        [[nodiscard]] static constexpr
        auto allocate(size_t n) -> T * {
            return new T[n];
        }

        static constexpr
        auto deallocate(const T *p, size_t) -> void {
            delete[] p;
        }

    public:
        constexpr auto operator=(ConstAllocator&&) noexcept -> ConstAllocator& = default;
        constexpr auto operator=(const ConstAllocator&) noexcept -> ConstAllocator& = default;

    public:
        constexpr ConstAllocator() noexcept = default;
        constexpr ~ConstAllocator() noexcept = default;

        constexpr ConstAllocator(ConstAllocator&&) noexcept = default;
        constexpr ConstAllocator(const ConstAllocator&) noexcept = default;

        template<class U>
        constexpr explicit ConstAllocator(const ConstAllocator<U> &) noexcept { }
    };
}

#endif /* CERBERUS_ALLOCATOR_HPP */
