#ifndef CERBERUS_HASH_HPP
#define CERBERUS_HASH_HPP

#include <atomic>
#include <cerberus/types.h>

namespace cerb::hash {
    namespace PRIVATE {
        inline std::atomic<size_t> uid{ 0 };
    }// namespace PRIVATE

    /**
     * @brief thread safe return unsigned value
     *
     * @return unique unsigned value
     */
    CERBLIB_INLINE auto uuid() noexcept -> size_t
    {
        return hash::PRIVATE::uid++;
    }
}// namespace cerb::hash

#endif /* CERBERUS_HASH_HPP */
