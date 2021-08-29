#include <deque>
#include <chrono>
#include <iostream>
#include <cerberus/set.hpp>
#include <cerberus/map.hpp>
#include <cerberus/address.hpp>
#include <cerberus/deque.hpp>
#include <cerberus/container.hpp>
#include <cerberus/bitmap.hpp>
#include <cerberus/lex/dot_item.hpp>
#include <cerberus/vector.hpp>

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

    cerb::lex::DotItem<int> item(
        0, "[a-zA-Z]+[0-9]?"sv,
        { { '=', '+', '-', '*', '/', '%', '(', ')', '[', ']', '{', '}',
            '!', '^', '&', '|', '~', '>', '<', '?', ':', ';', '$', ',' },
          { "||", "&&", "<<", ">>", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
            "==", "!=", ">=", "<=", ">>=", "<<=" } });

    std::string_view input = "vask ";
    item.set(input, "stdio"sv);

    cerb::lex::DotItem<int>::CheckResult elem;
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
    } while (item.get_input().size() != 0);

    return 0;
}

/*
 * TODO:    1) do not rely on compiler and check builtin function manually
 *
 */