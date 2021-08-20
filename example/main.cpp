#include <iostream>
#include <cerberus/set.hpp>
#include <cerberus/map.hpp>
#include <cerberus/address.hpp>

using namespace cerb::literals;

auto main(int  /*argc*/, char * /*argv*/[]) -> int {
    cerb::Address x(0x1000);
    x += 0x1000;

    auto c = cerb::move(x);

    std::cout << c.raw() << std::endl;

    return 0;
}