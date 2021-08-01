#include <iostream>
#include <cerberus/address.hpp>
#include <cerberus/bit.hpp>
#include <cerberus/map.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/doubleBitmap.hpp>


constexpr auto TEST() {
    cerb::ConstBitMap<u64, 128> a;
    //a.set(0, 1);
    a.clear();
    a[0] = 1;

    a.at(0);
    a.isEmpty();
    a.find_if<1>();

    cerb::ConstBitMap<u64, 128> b;
    b = a;
    return b;
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

namespace cerb {
    template<typename T>
    class BitmapAllocator {
        cerb::ConstBitMap<u64, 128> m_bitmap;

    protected:
        std::array<T, 128> m_data{};

    public:
        using value_type = T;

    public:
        [[nodiscard]] constexpr
        auto allocate(size_t n = 1) -> T *{
            if (n > 1) {
                return nullptr;
            }

            size_t index = m_bitmap.find_if<0>();
            if (index == std::numeric_limits<size_t>::max()) {
                return nullptr;
            }

            m_bitmap.template set<1>(index);
            return &m_data[index];
        }

        constexpr auto deallocate(const T *p, size_t n = 1) noexcept -> void {
            ptrdiff_t index = p - m_data.data();
            m_bitmap.template set<0>(static_cast<size_t>(index));
        }

    public:
        constexpr BitmapAllocator() noexcept = default;
    };
}

constexpr int TEST5() {
    cerb::BitmapAllocator<u64> a{};

    return 0;
}

#include <set>
#include <chrono>

consteval auto TEST7() -> bool {
    cerb::ConstBitMap<u64, 256> a;

    for (int i = 0; i < 60; i++) {
        a.set<1>(i);
    }
    //a.set<1>(60);
    a.set<1>(130);

    return a.is_value_set<0>(60, 70);
}

auto main(int argc, char *argv[]) -> int {
    /*
    std::vector<size_t> answers1;
    std::vector<size_t> answers2;

    {
        cerb::ConstBitMap<u64, 2048> a;
        srand(0);

        for (size_t i = 0; i < a.sizeOfArray(); i++) {
            a.data()[i] = rand();
        }

        size_t value;
        auto begin = std::chrono::high_resolution_clock::now();

        do {
            value = a.find_if<0>(10);
            answers1.emplace_back(value);
            if (value != std::numeric_limits<size_t>::max()) {
                for (size_t j = 0; j < 10; ++j) {
                    if (a.at(value + j) == 1) {
                        //throw std::runtime_error("1 found at " + std::to_string(value + j));
                    }

                    a.set<1>(value + j);
                }
            }
        } while (value != std::numeric_limits<size_t>::max());

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - begin;
        printf("First: %e\n", elapsed.count());
    }

    {
        cerb::ConstBitMap<u64, 2048> a;
        srand(0);

        for (size_t i = 0; i < a.sizeOfArray(); i++) {
            a.data()[i] = rand();
        }

        size_t value;
        auto begin = std::chrono::high_resolution_clock::now();

        do {
            value = a.find_if2<0>(10);
            answers2.emplace_back(value);
            if (value != std::numeric_limits<size_t>::max()) {
                for (size_t j = 0; j < 10; ++j) {
                    if (a.at(value + j) == 1) {
                        throw std::runtime_error("1 found");
                    }

                    a.set<1>(value + j);
                }
            }
        } while (value != std::numeric_limits<size_t>::max());

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - begin;
        printf("Second: %e\n", elapsed.count());
    }

    for (size_t i = 0; i < cerb::min(answers1.size(), answers2.size()); ++i) {
        printf("%5zu %5zu\n", answers1[i], answers2[i]);
    }

    std::cout << (answers1 == answers2) << std::endl;
     */
    std::cout << TEST7() << std::endl;
    return 0;
}
