#ifndef CERBERUS_LITERALS_HPP
#define CERBERUS_LITERALS_HPP

#include <cerberus/types.h>
#include <cerberus/string.hpp>

namespace cerb::literals {
    consteval auto operator""_z(unsigned long long x) -> size_t {
        return static_cast<size_t>(x);
    }

    consteval auto operator""_Z(unsigned long long x) -> size_t {
        return static_cast<size_t>(x);
    }

    consteval auto operator"" _hash(const char *str, size_t len) -> u64 {
        switch (len) {
        case sizeof(u8):
            return cerb::bit_cast<unsigned char>(str[0]);
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
                result = result * 31UL + cerb::bit_cast<unsigned char>(str[i]);
            }
            return result;
        }
        }
    }

    consteval auto operator"" _2val(const char *str, size_t len) {
        uintmax_t result   = 0;
        uintmax_t modifier = 10;

        switch (str[0]) {
        case 'b':
            modifier = 2;
            break;

        case '0':
            modifier = 8;
            break;

        case 'x':
        case 'X':
            modifier = 16;
            break;

        default:
            break;
        }

        if (modifier != 10) {
            ++str;
            --len;
        }

        auto getValue = [](char repr) {
            if (repr <= '9') {
                return repr - '0';
            } else if (repr >= 'a' && repr <= 'z') {
                return repr - 'a' + 10;
            } else if (repr >= 'A' && repr <= 'Z') {
                return repr - 'A' + 10;
            }
            return 0;
        };

        for (size_t i = 0; i < len; i++) {
            if (str[i] == ' ') {
                continue;
            }

            result = result * modifier + static_cast<uintmax_t>(getValue(str[i]));
        }

        return result;
    }
}// namespace cerb::literals

#endif /* CERBERUS_LITERALS_HPP */
