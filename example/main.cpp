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
    cerb::ConstBitMap<u64, 128> a;
    //a.set(0, 1);
    a.clear();
    a[0] = 1;

    a.at(0);
    a.isEmpty();
    a.find_if<1>();

    cerb::ConstBitMap<u64, 128> b;
    b = a;
    return 0;
}

auto TEST2() -> size_t {
    u64 buf[2] = {0};
    cerb::Bitmap<u64, true> a(buf, 128);
    a.set<1>(0);
    a.clear();
    a[0] = 1;

    a.at(0);
    a.isEmpty();
    return a.isEmpty();
}

constexpr auto TEST3() -> bool{
    cerb::ConstDoubleBitmap<u64, 128> a;
    a.clear();
    a.set<1>(0);
    a.set<1>(1);

    return a.isEmpty1();
}

auto TEST4() -> int {
    cerb::DoubleBitmap<u64> a(128);

    a.clear();
    a.set<1>(0);
    a.set<1>(1);

    return a.isEmpty1();
}

consteval auto TEST7() -> bool {
    cerb::ConstBitMap<u64, 256> a;

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

using namespace cerb::literals;


auto main(int argc, char *argv[]) -> int {
    cerb::gl::set<int, 8> a({10, 20});

    std::cout << "10 " << a.contains(10) << std::endl;
    std::cout <<  "20 " << a.contains(20) << std::endl;
    std::cout << "30 " << a.contains(30) << std::endl;
    a.insert(30);
    std::cout << "10 " << a.contains(10) << std::endl;
    std::cout <<  "20 " << a.contains(20) << std::endl;
    std::cout << "30 " << a.contains(30) << std::endl;
    a.erase(10);
    std::cout << "10 " << a.contains(10) << std::endl;
    std::cout <<  "20 " << a.contains(20) << std::endl;
    std::cout << "30 " << a.contains(30) << std::endl;
    a.insert(10);
    std::cout << "10 " << a.contains(10) << std::endl;
    std::cout <<  "20 " << a.contains(20) << std::endl;
    std::cout << "30 " << a.contains(30) << std::endl;

    return 0;
}
