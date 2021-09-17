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
#include <cerberus/string_view.hpp>

using namespace cerb::literals;
using namespace std::string_view_literals;

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

using namespace cerb;
using namespace cerb::lex;

template<
    typename CharT,
    typename TokenType,
    size_t MaxTerminals       = 128,
    size_t MaxLength4Terminal = 8,
    bool MayThrow             = true>
class TerminalContainer
{
    constexpr static size_t MaxChars = (1ULL << bitsizeof(CharT)) - 1;
    using string_view_t              = basic_string_view<CharT>;
    using bitmap_t                   = ConstBitmap<1, MaxChars>;
    using storage_t                  = std::array<bitmap_t, MaxLength4Terminal>;
    using map_t                      = gl::Map<uintmax_t, TokenType, MaxTerminals>;

public:
    [[nodiscard]] constexpr auto check(CharT elem) const -> Pair<bool, TokenType>
    {
        auto hash = to_unsigned(elem);
        return { static_cast<bool>(m_bitmaps[0].template at<0>(hash)),
                 m_map.search(hash)->second };
    }

    [[nodiscard]] constexpr auto check(size_t offset, const string_view_t &str) const
        -> Pair<string_view_t, TokenType>
    {
        size_t i    = offset;
        size_t hash = 0;

        CERBLIB_UNROLL_N(2)
        for (; i < str.size(); ++i) {
            auto local_hash = to_unsigned(str[i]);

            hash = hash * 31U + local_hash;

            if (m_bitmaps[0].template at<0>(local_hash) == 0) {
                break;
            }
        }
        return { { str.begin() + offset, str.begin() + i },
                 m_map.search(hash)->second };
    }

    consteval TerminalContainer() = default;
    consteval TerminalContainer(
        const std::initializer_list<Pair<TokenType, CharT>> &chars,
        const std::initializer_list<Pair<TokenType, const string_view_t>> &strings)
    {
        if constexpr (MayThrow) {
            if (chars.size() + strings.size() > MaxTerminals) {
                throw std::runtime_error(
                    "Terminal container can't hold so much terminals!");
            }
        }

        CERBLIB_UNROLL_N(4)
        for (const auto &elem : chars) {
            auto hash = to_unsigned(elem.second);
            m_map.emplace(hash, elem.first);
            m_bitmaps[0].template set<1, 0>(hash);
        }

        CERBLIB_UNROLL_N(2)
        for (const auto &elem : strings) {
            size_t hash    = 0;
            size_t counter = 0;

            if constexpr (MayThrow) {
                if (MaxLength4Terminal <= elem.second.size()) {
                    throw std::out_of_range(
                        "String checker can't hold such a long string!");
                }
            }

            CERBLIB_UNROLL_N(4)
            for (const auto &chr : elem.second) {
                auto local_hash = to_unsigned(chr);
                hash            = hash * 31U + local_hash;
                m_bitmaps[0].template set<1, 0>(local_hash);
            }

            m_map.emplace(hash, elem.first);
        }
    }

private:
    map_t m_map{};
    storage_t m_bitmaps{};
};

auto main(int /*argc*/, char * /*argv*/[]) -> int
{
    TerminalContainer<char, unsigned> terminals(
        { { 1, '+' }, { 2, '-' } }, { { 4, "+=" }, { 6, "<<=" } });

    auto elem = terminals.check(0, "<<=");
    std::cout << elem.first << ' ' << elem.second << endl;

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