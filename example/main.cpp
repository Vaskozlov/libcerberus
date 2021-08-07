#include <iostream>
#include <cerberus/address.hpp>
#include <cerberus/bit.hpp>
#include <cerberus/map.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/doubleBitmap.hpp>

#include <deque>
#include <chrono>
#include <cerberus/map.hpp>

consteval auto TEST() -> int {
    cerb::constBitMap<128> a;
    //a.set(0, 1);
    a.clear();
    a[0] = 1;

    a.at(0);
    a.isEmpty();
    a.find_if<1>();

    cerb::constBitMap<128> b;
    b = a;
    return 0;
}

auto TEST2() -> size_t {
    u64 buf[2] = {0};
    cerb::bitmap<true> a(buf, 128);
    a.set<1>(0);
    a.clear();
    a[0] = 1;

    a.at(0);
    a.isEmpty();
    return a.isEmpty();
}

constexpr auto TEST3() -> bool{
    cerb::constDoubleBitmap<128> a;
    a.clear();
    a.set<1>(0);
    a.set<1>(1);

    return a.isEmpty1();
}

auto TEST4() -> int {
    cerb::DoubleBitmap a(128);

    a.clear();
    a.set<1>(0);
    a.set<1>(1);

    return a.isEmpty1();
}

consteval auto TEST7() -> bool {
    cerb::constBitMap<256> a;

    for (int i = 0; i < 60; i++) {
        a.set<1>(i);
    }
    //a.set<1>(60);
    a.set<1>(130);

    return a.is_value_set<0>(60, 70);
}

consteval auto TEST8() -> int {
    std::array<int, 10> a {} ;
    cerb::memset(a, 0, 10);
    return 0;
}

#include <set>
#include <cerberus/container.hpp>
using namespace cerb::literals;

auto main(int argc, char *argv[]) -> int {

    {
        srand(0);
        cerb::constBitMap<64> bitmap;

        auto begin = std::chrono::high_resolution_clock::now();
        size_t result = 0;

        while (true) {
            result = bitmap.find_if<0>(5);

            if (result == std::numeric_limits<size_t>::max()) {
                break;
            }

            std::cout << result << cerb::endl;

            bitmap.set<1>(result, 5);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - begin;
        printf("u16 %e\n", elapsed.count());
    }
    std::cout << cerb::endl << cerb::endl;
    {
        srand(0);
        cerb::constBitMap<64> bitmap;

        auto begin = std::chrono::high_resolution_clock::now();
        size_t result;

        while (true) {
            result = bitmap.find_if<0>(5);

            if (result == std::numeric_limits<size_t>::max()) {
                break;
            }

            std::cout << result << cerb::endl;

            bitmap.set<1>(result, 5);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - begin;
        printf("u64 %e\n", elapsed.count());
    }

    return 0;
}
