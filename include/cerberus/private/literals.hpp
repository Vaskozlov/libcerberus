#ifndef CERBERUS_LITERALS_HPP
#define CERBERUS_LITERALS_HPP

#ifndef cerberusTypes_h
#  error Do NOT inlude cerberus/private/literals.hpp without cerberus/types.h
#endif

namespace cerb::literals {
    CERBLIB_COMPILE_TIME auto operator ""_z(unsigned long long x) -> size_t {
        return static_cast<size_t>(x);
    }

    CERBLIB_COMPILE_TIME auto operator ""_Z(unsigned long long x) -> size_t {
        return static_cast<size_t>(x);
    }

    CERBLIB_COMPILE_TIME auto operator "" _2val(const char *str, size_t len) {
        uintmax_t result = 0;
        uintmax_t modifier = 10;

        if (str[0] == 'b') {
            modifier = 2;
        } else if (str[0] == 'o') {
            modifier = 8;
        } else if (str[0] == 'x' || str[0] == 'X') {
            modifier = 16;
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
            if (str[i] == ' ' ) {
                continue;
            }

            result = result * modifier + static_cast<uintmax_t>(getValue(str[i]));
        }

        return result;
    }
} // namespace cerb::literals

#endif /* CERBERUS_LITERALS_HPP */
