#include <cerberus/string.hpp>

namespace cerb {
    size_t strlen(const char *__restrict str) {
        size_t index;
        
#if defined(__x86_64__)
        index = 0;

        __asm__(
            " mov $-16, %0                      \n"
            " pxor %%xmm0, %%xmm0               \n"
            "1:                                 \n"
            " add $16, %0                       \n"
            " pcmpistri $0x08, (%0,%1), %%xmm0  \n"
            " jnz 1b                            \n"
            " add %2, %0                        \n"
            : "=a"(index)
            : "d"(reinterpret_cast<size_t>(str)), "c"(reinterpret_cast<size_t>(str))
            : "memory");

#else
        while (*str != '\0') {
            str++;
            index++;
        }

#endif /* ARCH */
        return index;
    }
}// namespace cerb
