#ifndef CERBERUS_LITERALS_HPP
#define CERBERUS_LITERALS_HPP

#include <cerberus/types.h>
#include <cerberus/string.hpp>

namespace cerb::literals {
    consteval auto operator""_z(unsigned long long x) -> size_t
    {
        return static_cast<size_t>(x);
    }

    consteval auto operator""_Z(unsigned long long x) -> size_t
    {
        return static_cast<size_t>(x);
    }

    consteval auto operator"" _hash(const char *str, size_t len) -> u64
    {
        switch (len) {
        case sizeof(u8):
            return bit_cast<unsigned char>(str[0]);
        case sizeof(u16):
            return str2Uint<u16>(str);
        case sizeof(u32):
            return str2Uint<u32>(str);
        case sizeof(u64):
            return str2Uint<u64>(str);
        default: {
            u64 result = 0;

            CERBLIB_UNROLL_N(2)
            for (size_t i = 0; i < len; ++i) {
                result = result * 31UL + bit_cast<unsigned char>(str[i]);
            }
            return result;
        }
        }
    }
}// namespace cerb::literals

#endif /* CERBERUS_LITERALS_HPP */
