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
#include <cerberus/lex/lex.hpp>
#include <cerberus/vector.hpp>
#include <random>

using namespace cerb::literals;
using namespace std::string_view_literals;

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
        inline friend auto operator<<(T &os, const ConstructorChecker & /*checker*/)
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

CERBERUS_LEX_TEMPLATES
struct Lex : public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS

    constexpr void yield(size_t first, size_t last) override
    {
        for (; first != last; ++first) {
            auto &token = m_tokens[first];
            std::cout << std::setw(8) << std::left << token.repr << ' ' << token.type
                      << ' ' << "line: " << token.pos.line_number + 1 << ' '
                      << token.pos.char_number << std::endl;
        }
    }

    CERBERUS_LEX_INITIALIZER(Lex)
    {}
};

Lex<char, unsigned> controller{
    { { 3, "for" }, { 4, "while" }, { 5, "char" }, { 6, "int" }, { 7, "return" } },
    { { 8, "[a-zA-Z_]+" }, { 9, "[0-9]+" } },
    { { '=', '+', '-', '*', '/', '%', '(', ')', '[', ']', '{',  '}', '!', '^',
        '&', '|', '~', '>', '<', '?', ':', ';', '$', ',', '\\', '.', '#' },
      { "||", "&&", "<<", ">>", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "==",
        "!=", ">=", "<=", ">>=", "<<=" } }
};

auto main(int /*argc*/, char * /*argv*/[]) -> int
{
    controller.scan(
        R"(
    #define f(x) (x * 2)

    int main(int argc, char **argv) {
        int a = 10;
        int b = 20;

        return a + b;
    })",
        "stdio");

    puts(
        R"(int main(int argc, char **argv) {
        int a = 10;
        int b = 20;

        return a + b;
    })");

    /*
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
    */

    return 0;
}

/*
 * TODO:    1) do not rely on compiler and check builtin function manually
 *
 */