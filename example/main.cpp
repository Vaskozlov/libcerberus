#include <iostream>
#include <cerberus/address.hpp>
#include <cerberus/bit.hpp>
#include <cerberus/map.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/doubleBitmap.hpp>

namespace cerb {

}

constexpr auto TEST() {
    cerb::ConstBitMap<u64, 128> a;
    a.set(0, 1);
    a.clear();
    a[0] = 1;

    a.at(0);
    a.isEmpty();
    a.find_if<1>();

    cerb::ConstBitMap<u64, 128> b;
    b = a;
    return b;
}

consteval auto TEST2() -> size_t {
    u64 buf[2] = {0};
    cerb::Bitmap<u64, true> a(buf, 128);
    a.set(0, 1);
    a.clear();
    a[0] = 1;

    a.at(0);
    a.isEmpty();
    return a.isEmpty();
}

consteval auto TEST3() -> bool{
    cerb::ConstDoubleBitmap<u64, 128> a;
    a.clear();
    a.set<1>(0);
    a.set<1>(1);

    return a.isEmpty1();
}

constexpr auto TEST4() -> int {
    cerb::DoubleBitmap<u64> a(128);

    a.clear();
    a.set<1>(0);
    a.set<1>(1);

    return a.isEmpty1();
}

auto main(int argc, char *argv[]) -> int {


    return 0;
}
