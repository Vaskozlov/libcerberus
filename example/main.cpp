#include <cmath>
#include <string.h>
#include <iostream>
#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include <cerberus/address.hpp>
#include <cerberus/bitmap.hpp>
#include <chrono>
#include <set>
#include <memory_resource>

u64 globalFreeBuffer[40];
cerb::freeBitmap<u64> globalFreeBitmap(globalFreeBuffer, bitsizeof(globalFreeBuffer));

template<typename T1, typename T2>
__attribute__((noinline)) static int cmpBitmaps(T1 &&bitmap1, T2 &&bitmap2, std::set<size_t> &positions) {

    for (size_t i = 0; i < bitmap1.size(); i++) {
        if (positions.count(i) > 0) {
            if (bitmap1.at(i) == 0 || bitmap1.at(i) != bitmap2.at(i)) {
                printf("index: %zu, global: %d, local: %d\n", i, bitmap1.at(i), bitmap2.at(i));
                return 1;
            }
        } else {
            if (bitmap1.at(i) == 1 || bitmap1.at(i) != bitmap2.at(i)) {
                printf("index: %zu, global: %d, local: %d\n", i, bitmap1.at(i), bitmap2.at(i));
                return 1;
            }
        }
    }

    return 0;
}


int testOfFreeBitmap(size_t size) {
    u64 buffer[40];

    cerb::freeBitmap<u64> bitmap1(buffer, size);
    bitmap1.clear();

    for (size_t i = 0; i < bitmap1.size(); i++) {
        if (bitmap1.at(i) != 0) UNLIKELY {
            return 1;
        }
    }

    std::set<size_t> positions;

    for (size_t i = 0; i < 500; i++) {
        auto value = static_cast<size_t>(rand() % bitmap1.size());
        positions.insert(value);
        bitmap1.set<1>(value);
    }

    globalFreeBitmap = bitmap1.toAPI(); // if globalFreeBitmap.size() is smaller than bitmap1 operator= will return null bitmap

    for (auto elem: positions) {
        if (bitmap1.at(elem) != 1) {
            return 1;
        }
    }

    if (globalFreeBitmap.size() != bitmap1.size()) {
        return 1;
    }

    return cmpBitmaps(std::move(globalFreeBitmap), std::move(bitmap1), positions);
}

cerb::bitmap<u64> globalBitmap;

int testOfBitmap(size_t size) {
    cerb::bitmap<u64> bitmap1(size);
    bitmap1.clear();

    for (size_t i = 0; i < bitmap1.size(); i++) {
        if (bitmap1.at(i) != 0) UNLIKELY {
            return 1;
        }
    }

    std::set<size_t> positions{};

    for (size_t i = 0; i < 500; i++) {
        auto value = static_cast<size_t>(rand() % bitmap1.size());

        if (positions.count(value) == 0) {
            positions.insert(value);
            bitmap1.set<1>(value);
        }
    }

    globalBitmap = bitmap1.toAPI();

    for (auto elem: positions) {
        if (bitmap1.at(elem) != 1) {
            return 1;
        }
    }

    if (globalBitmap.size() != bitmap1.size()) {
        return 1;
    }

    return cmpBitmaps(std::move(globalBitmap), std::move(bitmap1), positions);
}

auto main(int argc, char *argv[]) -> int {
    printf("Bitmap test: %d\n", testOfBitmap(1024 * argc));
    printf("freeBitmap test: %i\n", testOfFreeBitmap(1024 * argc));
    return 0;
}
