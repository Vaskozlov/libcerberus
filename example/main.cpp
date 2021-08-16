#include <set>
#include <chrono>
#include <iostream>
#include <cerberus/bit.hpp>
#include <cerberus/map.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/container.hpp>
using namespace cerb::literals;

#include <cerberus/set.hpp>
#include <cerberus/redBlackTree.hpp>

auto main(int argc, char *argv[]) -> int {

    cerb::Map<int, std::string, cerb::Throwable{}, cerb::greater<void>> a;
    a[0] = "He";
    a[1] = "HellO";

    for (auto &elem : a) {
        std::cout << elem.first << ' ' << elem.second << std::endl;
    }

    return 0;
}