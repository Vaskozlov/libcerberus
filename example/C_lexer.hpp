#ifndef CERBERUS_LEXER_C
#define CERBERUS_LEXER_C

#include <iostream>
#include <cerberus/analyzation/lex/lex.hpp>

using namespace cerb::literals;

// clang-format off

enum struct Lex4LexParentBlock : size_t
{
    RESERVED         = 8UL,
    GENERAL          = 64UL,
    OPERATORS        = 128UL,
};

enum struct Lex4LexParentItem : size_t
{
    UNDEFINED        = static_cast<size_t>(Lex4LexParentBlock::RESERVED) + 0UL,
    EoF              = static_cast<size_t>(Lex4LexParentBlock::RESERVED) + 1UL,
    TRUE             = static_cast<size_t>(Lex4LexParentBlock::GENERAL) + 0UL,
    FALSE            = static_cast<size_t>(Lex4LexParentBlock::GENERAL) + 1UL,
    INT              = static_cast<size_t>(Lex4LexParentBlock::GENERAL) + 2UL,
    IDENTIFIER       = static_cast<size_t>(Lex4LexParentBlock::GENERAL) + 3UL,
    STRING           = static_cast<size_t>(Lex4LexParentBlock::GENERAL) + 4UL,
    CHAR             = static_cast<size_t>(Lex4LexParentBlock::GENERAL) + 5UL,
    ASSIGN           = static_cast<size_t>(Lex4LexParentBlock::OPERATORS) + 0UL,
    ANGLE_OPENING    = static_cast<size_t>(Lex4LexParentBlock::OPERATORS) + 1UL,
    ANGLE_CLOSING    = static_cast<size_t>(Lex4LexParentBlock::OPERATORS) + 2UL,
    COLON            = static_cast<size_t>(Lex4LexParentBlock::OPERATORS) + 3UL,
    WORD             = static_cast<size_t>(Lex4LexParentBlock::OPERATORS) + 4UL,
    EoR              = static_cast<size_t>(Lex4LexParentBlock::OPERATORS) + 5UL,
};

constexpr cerb::gl::Map<Lex4LexParentBlock, cerb::string_view, 3> Lex4LexParentBlockNames{
    true, {
        { Lex4LexParentBlock::RESERVED, "RESERVED"_sv },
        { Lex4LexParentBlock::GENERAL, "GENERAL"_sv },
        { Lex4LexParentBlock::OPERATORS, "OPERATORS"_sv },
    }
};

constexpr cerb::gl::Map<Lex4LexParentItem, cerb::string_view, 14> Lex4LexParentItemItemsNames{
    true, {
        { Lex4LexParentItem::UNDEFINED, "UNDEFINED"_sv },
        { Lex4LexParentItem::EoF, "EoF"_sv },
        { Lex4LexParentItem::TRUE, "TRUE"_sv },
        { Lex4LexParentItem::FALSE, "FALSE"_sv },
        { Lex4LexParentItem::INT, "INT"_sv },
        { Lex4LexParentItem::IDENTIFIER, "IDENTIFIER"_sv },
        { Lex4LexParentItem::STRING, "STRING"_sv },
        { Lex4LexParentItem::CHAR, "CHAR"_sv },
        { Lex4LexParentItem::ASSIGN, "ASSIGN"_sv },
        { Lex4LexParentItem::ANGLE_OPENING, "ANGLE_OPENING"_sv },
        { Lex4LexParentItem::ANGLE_CLOSING, "ANGLE_CLOSING"_sv },
        { Lex4LexParentItem::COLON, "COLON"_sv },
        { Lex4LexParentItem::WORD, "WORD"_sv },
        { Lex4LexParentItem::EoR, "EoR"_sv },
    }
};


#define Lex4LexParentTemplate                             \
    template<                                   \
        typename CharT = char,                       \
        typename TokenType = Lex4LexParentItem,              \
        bool MayThrow = true,                        \
        size_t UID = 0,                             \
        bool AllowStringLiterals = true,             \
        bool AllowComments = true,                   \
        size_t MaxTerminals = 64,                  \
        size_t MaxSize4Terminals = 4>

#define Lex4LexParentDefinition(CLASS) CalculatorTemplate struct CLASS : public Lex4LexParent<>

#define Lex4LexParentAccess using Lex4LexParent<>::parent;               \
        using parent::head;                                                         \
        using item_t           = typename parent::item_t;                           \
        using storage_t        = typename parent::storage_t;                        \
        using token_t          = typename parent::token_t;                          \
        using result_t         = typename parent::result_t;                         \
        using position_t       = typename parent::position_t;                       \
        using string_t         = typename parent::string_t;                         \
        using string_view_t    = typename parent::string_view_t;                    \
        using string_checker_t = typename parent::string_checker_t;                 \
        using item_initializer  = typename parent::item_initializer;                \
        using enum Lex4LexParentItem;

Lex4LexParentTemplate
struct Lex4LexParent: public CERBERUS_LEX_PARENT_CLASS
{
    CERBERUS_LEX_PARENT_CLASS_ACCESS
    using enum Lex4LexParentItem;

    constexpr Lex4LexParent()
    : parent(
        '\"',
        '\'',
        STRING,
        CHAR,
        {
            { TRUE, "true"_sv, true, 2 },
            { FALSE, "false"_sv, true, 2 },
            { INT, "[0-9]+"_sv, false, 6 },
            { IDENTIFIER, "[a-zA-Z_]+[a-zA-Z0-9_]*"_sv, false, 6 }
        },
        {
            { 
                { ASSIGN, '=' },
                { ANGLE_OPENING, '[' },
                { ANGLE_CLOSING, ']' },
                { COLON, ':' },
                { WORD, '|' },
            },
           {
                { EoR, "%%"_sv },
           }
        },
        "//",
        "/*",
        "*/"
    )
    {}
};
// clang-format on

#endif /* CERBERUS_LEXER_C */
