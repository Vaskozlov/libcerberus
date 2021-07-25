#include <iostream>
#include "testOFoperators.hpp"

auto main(int argc, char *argv[]) -> int {

    operators_test a{10, 10.0f};
    operators_test b{0, 0.0f};

    b = a + operators_test{10, 10.0f};
    b *= a;

    std::cout << b.i << ' ' << b.f << std::endl;

    return 0;
}
