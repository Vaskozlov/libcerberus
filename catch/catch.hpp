#ifndef CERBERUS_CATCH_HPP
#define CERBERUS_CATCH_HPP

#include <cerberus/types.h>

namespace cerb::test
{
    inline static std::random_device random_device{};
    inline static std::default_random_engine random_engine(r());
    inline static std::uniform_int_distribution<int> uniform_dist(1, 6);
}

#endif /* CERBERUS_CATCH_HPP */
