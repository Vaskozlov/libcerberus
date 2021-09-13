#ifndef CERBERUS_CATCH_HPP
#define CERBERUS_CATCH_HPP

#include <cerberus/types.h>

namespace cerb::test {
    inline static std::random_device random_device{};
    inline static std::default_random_engine random_engine(random_device());
    inline static std::uniform_int_distribution<size_t> uniform_dist(10000, 1000000);
}// namespace cerb::test

using namespace std::string_literals;

#endif /* CERBERUS_CATCH_HPP */
