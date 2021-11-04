#ifndef CERBERUS_RANGE_HPP
#define CERBERUS_RANGE_HPP

#include "types.h"

#ifndef CERBLIB_RANGE
#    define CERBLIB_RANGE(x, to) for (int64_t x = 0; x < to; ++x)
#endif /* CERBLIB_RANGE */

namespace cerb {
    template<typename T = i64>
    struct range
    {
        const i64 first{}, last{}, step{};

        class iterator
        {
            i64 m_step{};
            i64 m_state{};

        public:
            constexpr auto operator++() -> iterator &
            {
                m_state += m_step;
                return *this;
            }

            constexpr auto operator--() -> iterator &
            {
                m_state -= m_step;
                return *this;
            }

            constexpr auto operator*() -> i64
            {
                return m_state;
            }

            constexpr auto operator!=(const iterator &other) const noexcept
            {
                return m_state != other.m_state;
            }

            constexpr iterator(i64 begin, i64 step) : m_step(step), m_state(begin)
            {}
        };

        constexpr auto begin() const noexcept -> iterator
        {
            return iterator(first, step);
        }

        constexpr auto end() const noexcept -> iterator
        {
            return iterator(last, step);
        }

        constexpr range(i64 last_)
          : first(0), last(last_), step(1)
        {}

        constexpr range(i64 first_, i64 last_, i64 step_ = 1)
          : first(first_), last(last_ + (last_ - first_) % step_), step(step_)
        {}

        constexpr ~range() = default;
    };
}// namespace cerb

#endif /* CERBERUS_RANGE_HPP */
