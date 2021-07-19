#include <cmath>
#include <random>
#include <string.h>
#include <iostream>
#include <cerberus/types.h>
#include <cerberus/math.hpp>
#include <cerberus/address.hpp>
#include <cerberus/bitmap.hpp>
#include <chrono>
#include <set>
#include <memory_resource>
#include <cerberus/doubleBitmap.hpp>
#include <cerberus/arrayProtocol.hpp>

u64 globalFreeBuffer[40];
cerb::freeBitmap<u64> globalFreeBitmap(globalFreeBuffer, bitsizeof(globalFreeBuffer));

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<u64> dist(0, 1024);

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
        auto value = static_cast<size_t>(dist(mt) % bitmap1.size());
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
        auto value = static_cast<size_t>(dist(mt) % bitmap1.size());

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

u64 buffer[100];
cerb::freeDoubleBitmap<u64> a(buffer, 256);


auto main(int argc, char *argv[]) -> int {

    cerb::ArrayProtocol<u64, false, 10> array{}; // u64 array[10];
    cerb::ArrayProtocol<u64, true> pointer = new u64[10]; // u64 *pointer;

    pointer[0] = 10;
    array[0] = 10;

    array.get(); // -> T * (address of array)
    pointer.get(); // -> T * (address of array)

    printf("Pointer size: %lu, array size: %lu\n", sizeof(pointer), sizeof(array));

    return 0;
}
