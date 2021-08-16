#include <set>
#include <map>
#include <chrono>
#include <iostream>
#include <cerberus/bit.hpp>
#include <cerberus/map.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/container.hpp>
using namespace cerb::literals;

#include <cerberus/set.hpp>
#include <cerberus/redBlackTree.hpp>

auto main(int  /*argc*/, char * /*argv*/[]) -> int {
    std::map<int, int> c;

    cerb::Map<int, std::string> a({{20, "Hi!"}});
    a.insert({10, "hello"});
    a.insert({10, "Hello, World!"});

    CERBLIB_UNROLL_N(2)
    for (auto &elem : a) {
        std::cout << elem.first << ' ' << elem.second << std::endl;
    }

    cerb::Set<int> b({10, 20, 30, 40, 100, 1000});
    b.insert(10);
    b.insert(20);
    b.insert(30);

    for (const auto &elem: b) {
        std::cout << elem << std::endl;
    }

    return 0;
}