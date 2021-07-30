#ifndef CERBERUS_HASH_HPP
#define CERBERUS_HASH_HPP

#include <atomic>
#include <cerberus/types.h>

namespace cerb::hash::PRIVATE {
    inline std::atomic<size_t> uid{ 0 };
} // namespace cerb::hash::PRIVATE

namespace cerb::hash {
    /**
     * @brief thread safe return unsigned value
     *
     * @return unique unsigned value
     */
    CERBLIB_INLINE auto uuid() noexcept -> size_t {
        return cerb::hash::PRIVATE::uid++;
    }
} // namespace cerb::hash

#endif /* CERBERUS_HASH_HPP */
