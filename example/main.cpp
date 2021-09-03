#include <deque>
#include <chrono>
#include <iostream>
#include <cerberus/set.hpp>
#include <cerberus/map.hpp>
#include <cerberus/hash.hpp>
#include <cerberus/address.hpp>
#include <cerberus/deque.hpp>
#include <cerberus/container.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/lex/dot_item.hpp>
#include <cerberus/vector.hpp>
#include <random>

using namespace cerb::literals;
using namespace std::string_view_literals;

/*
cerb::gl::Map<std::string_view , cerb::lex::DotItem<true>, 8> items (
    {
        {"NAME", {3, "[a-zA-Z]+"}},
        {"IDENTIFIER", {0, "[a-zA-Z_]+[a-zA-Z0-9_]*"}},
        {"NUMBER", {1, "[0-9]+"}},
        {"FLOAT", {2, "[0-9]+[.][0-9]+"}}
    }
);
 */

cerb::lex::experimental::DotItem<char, unsigned> item(0, 0, "", { {}, {} });

namespace cerb {
    class ConstructorChecker
    {
        u8 empty{};
        static inline std::atomic<size_t> m_constructions{ 0 };
        static inline std::atomic<size_t> m_copies{ 0 };
        static inline std::atomic<size_t> m_moves{ 0 };
        static inline std::atomic<size_t> m_copy_constructions{ 0 };
        static inline std::atomic<size_t> m_move_constructions{ 0 };
        static inline std::atomic<size_t> m_deconstructions{ 0 };
        static inline std::atomic<size_t> m_counter{ 0 };

    public:
        inline static auto counter() -> size_t
        {
            return m_counter;
        }

        inline static auto clear() -> void
        {
            m_counter            = 0;
            m_constructions      = 0;
            m_copy_constructions = 0;
            m_move_constructions = 0;
            m_copies             = 0;
            m_moves              = 0;
            m_deconstructions    = 0;
        }

        inline auto operator=(const ConstructorChecker &) -> ConstructorChecker &
        {
            ++m_copies;
            return *this;
        }
        inline auto operator=(ConstructorChecker &&) noexcept -> ConstructorChecker &
        {
            ++m_moves;
            return *this;
        }

        inline ConstructorChecker(u8 /*unused*/)
        {}

        inline ConstructorChecker()
        {
            ++m_counter;
            ++m_constructions;
        }

        inline ConstructorChecker(const ConstructorChecker & /*unused*/)
        {
            ++m_counter;
            ++m_copy_constructions;
        }

        inline ConstructorChecker(ConstructorChecker && /*unused*/) noexcept
        {
            ++m_counter;
            ++m_move_constructions;
        }

        inline ~ConstructorChecker()
        {
            --m_counter;
            ++m_deconstructions;
        }

        template<typename T>
        inline friend auto operator<<(T &os, const ConstructorChecker &checker)
            -> T &
        {
            os << "Constructions: " << cerb::ConstructorChecker::m_constructions
               << ", construction copies: "
               << cerb::ConstructorChecker::m_copy_constructions
               << ", construction moves: "
               << cerb::ConstructorChecker::m_move_constructions
               << ", copies: " << cerb::ConstructorChecker::m_copies
               << ", moves: " << cerb::ConstructorChecker::m_moves
               << ", deconstructions: "
               << cerb::ConstructorChecker::m_deconstructions
               << ", leaks: " << cerb::ConstructorChecker::m_counter;
            return os;
        }
    };
}// namespace cerb

namespace cerb::test {
    inline static std::random_device random_device{};
    inline static std::default_random_engine random_engine(random_device());
    inline static std::uniform_int_distribution<size_t> uniform_dist(10000, 1000000);
}// namespace cerb::test

using namespace std::string_literals;

auto main(int /*argc*/, char * /*argv*/[]) -> int
{
    /*
    std::string_view input = "vask 4.5 51";

    cerb::lex::ItemState state = cerb::lex::UNINITIALIZED;
    cerb::Vector<cerb::Pair<u32, std::string_view>> vec;

     items.begin()->second.set(input);

    for (auto &elem : items) {
        elem.second.rebind();
        do {
            state = elem.second.check(vec);
        } while (state != cerb::lex::OUT_OF_ELEMS);
    }

    for (const auto &elem: vec) {
        std::cout << elem.first << ' ' << elem.second << std::endl;
    }
    */
    /*
    cerb::lex::DotItem2<int> item(
        0, "[a-zA-Z]+[0-9]?"sv,
        { { '=', '+', '-', '*', '/', '%', '(', ')', '[', ']', '{', '}',
            '!', '^', '&', '|', '~', '>', '<', '?', ':', ';', '$', ',', '\\' },
          { "||", "&&", "<<", ">>", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
            "==", "!=", ">=", "<=", ">>=", "<<=" } });

    std::string_view input = "vask ";
    item.set(input, "stdio"sv);

    cerb::lex::DotItem2<int>::ResultType elem;
    do {
        elem = item.check();
        std::cout << cerb::lex::ItemStateRepr.at(elem.first) << std::endl;

        if (elem.first == cerb::lex::SCAN_FINISHED) {
            for (const auto &i : elem.second) {
                std::cout << "File: " << i.pos.filename
                          << ", line: " << i.pos.line_number
                          << ", char : " << i.pos.char_number << ", repr: " << i.repr
                          << std::endl;
            }
        }
    } while (!item.get_input().empty() && elem.first != cerb::lex::UNABLE_TO_MATCH);
    */

    size_t Iterations = cerb::test::uniform_dist(cerb::test::random_engine);
    std::vector<cerb::ConstructorChecker> std_vector{};
    cerb::Vector<cerb::ConstructorChecker> cerb_vector{};

    {
        auto begin = std::chrono::high_resolution_clock::now();
        CERBLIB_UNROLL_N(16)
        for (size_t i = 0; i < Iterations; ++i) {
            std_vector.push_back({});
            std_vector.push_back({});
            std_vector.emplace_back();
            std_vector.pop_back();
        }
        std_vector.insert(
            std_vector.begin() + Iterations % 1551, { {}, {}, {}, {} });
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - begin;
        printf("Std vector: %e\n", elapsed.count());
        std_vector.clear();
    }

    if (cerb::ConstructorChecker::counter() != 0) {
        throw std::runtime_error(
            "Not all elements were freed ("s +
            std::to_string(cerb::ConstructorChecker::counter()) + ")! at "s +
            std::string(__FILE__) + " "s + std::to_string(__LINE__));
    }

    std::cout << "Iterations: " << Iterations << ", "
              << cerb::ConstructorChecker{ 1 } << std::endl;
    cerb::ConstructorChecker::clear();

    {
        auto begin = std::chrono::high_resolution_clock::now();
        CERBLIB_UNROLL_N(16)
        for (size_t i = 0; i < Iterations; ++i) {
            cerb_vector.push_back({});
            cerb_vector.push_back({});
            cerb_vector.emplace_back();
            cerb_vector.pop_back();
        }
        // fix insert
        //cerb_vector.insert(Iterations % 1551, { {}, {}, {}, {} });
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - begin;
        printf("Cerb vector: %e\n", elapsed.count());
        cerb_vector.clear();
    }

    if (cerb::ConstructorChecker::counter() != 0) {
        throw std::runtime_error(
            "Not all elements were freed ("s +
            std::to_string(cerb::ConstructorChecker::counter()) + ")! at "s +
            std::string(__FILE__) + " "s + std::to_string(__LINE__));
    }

    std::cout << "Iterations: " << Iterations << ", "
              << cerb::ConstructorChecker{ 1 } << std::endl;
    cerb::ConstructorChecker::clear();

    return 0;
}

/*
 * TODO:    1) do not rely on compiler and check builtin function manually
 *
 */