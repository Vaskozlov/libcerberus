#include <iostream>
#include <cerberus/address.hpp>

auto main(int argc, char *argv[]) -> int {

    cerb::address a(0x1000);
    a++;

    return 0;
}
