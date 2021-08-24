#ifndef CERBERUS_LEX_HPP
#define CERBERUS_LEX_HPP

#include <iostream>
#include <cerberus/set.hpp>
#include <cerberus/vector.hpp>
#include <cerberus/lex/char.hpp>

namespace cerb::lex {

    /*

     class DotItem
     {
         enum struct MaskOfValues : i8
         {
             LETTER    = 0b1,
             LAYOUT    = 0b10,
             OPERATOR  = 0b100,
             SEPARATOR = 0b1000,
             EoF       = 0b10000,
         };

         enum Types : u16
         {
             LETTER    = 256 + static_cast<int>(MaskOfValues::LETTER),
             LAYOUT    = 256 + static_cast<int>(MaskOfValues::LAYOUT),
             OPERATOR  = 256 + static_cast<int>(MaskOfValues::OPERATOR),
             SEPARATOR = 256 + static_cast<int>(MaskOfValues::SEPARATOR),
             EoF       = 256 + +static_cast<int>(MaskOfValues::EoF)
         };

         struct TokenType
         {
             PositionInFile position{};
             Types tokenClass{};
             char repr{ '\0' };
         };

         struct DotItemNode
         {
             enum Rule
             {
                 ONCE,
                 OPTIONAL,
                 ONE_OR_MORE_TIMES,
                 ZERO_OR_MORE_TIMES,
             };

             Rule rule{ONCE};
             Set<char> available_chars{};
             DotItemNode *next{ nullptr };
         };

     private:
         constexpr static std::array<char, 127> m_values{
             0x10, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
             0x2,  0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
             0x2,  0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x0,   0x0,   0x0,   0x0,   0x0,
     0x0, 0x0,    0x8, 0x8, 0x4, 0x4, 0,   0x4, 0x0,   0x0,   0x1, 0x1, 0x1, 0x1,
             0x1,  0x1, 0x1, 0x1, 0x1, 0x1, 0x0,   0x0,   0x0,   0x0,   0x0,   0x4,
     0x0, 0x1,  0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
     0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x8,  0x0,   0x8, 0x0,
     0x0,   0x0,   0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,  0x1, 0x1, 0x1, 0x1, 0x1,
     0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,  0x1, 0x1, 0x1, 0x1, 0x1, 0x0,   0x4,
     0x0,   0x0
         };
         constexpr static gl::Set<char, 8> m_operators{ { '+', '*', '?', '|' } };
         constexpr static gl::Set<char, 8> m_separators{ { '[', ']', '(', ')' } };

     private:
         size_t m_dot{ 0 };
         TokenType m_token{};
         char m_input_char{ '\0' };
         std::string_view m_input{};
         DotItemNode *m_root{ nullptr };

     private:
         constexpr auto nextChar() -> void
         {
             if (m_dot + 1 < m_input.size()) {
                 m_input_char = m_input[++m_dot];
             } else {
                 m_input_char = '\0';
             }

             if (m_input_char == '\n') {
                 ++m_token.position.line_number;
                 m_token.position.char_number = 0;
             } else {
                 ++m_token.position.char_number;
             }
         }

         constexpr auto skipLayout()
         {
             CERBLIB_UNROLL_N(1)
             while ((m_values[m_input_char & 127] &
                     static_cast<i8>(MaskOfValues::LAYOUT)) ==
                    static_cast<i8>(MaskOfValues::LAYOUT)) {
                 nextChar();
             }
         }

         constexpr auto notePosition() -> void
         {
             m_token.position.start_position = m_dot;
             m_token.position.end_position   = m_dot + 1;
         }

         constexpr auto GetNextToken() -> void
         {
             skipLayout();
             notePosition();
             m_token.tokenClass =
                 static_cast<Types>(256 + m_values[m_input_char & 127]);
             m_token.repr = m_input_char;
             nextChar();
         }

         constexpr auto allocateNewNode()
         {
             if (m_root == nullptr) {
                 m_root = new DotItemNode();
                 return m_root;
             } else {
                 DotItemNode *node = m_root;

                 CERBLIB_UNROLL_N(2)
                 while (node->next != nullptr) {
                     node = node->next;
                 }
                 node->next = new DotItemNode();
                 return node->next;
             }
         }

         constexpr auto checkClass(Types type, Types target)
         {
             return ((static_cast<u16>(type) & static_cast<u16>(target)) ==
     static_cast<u16>(target));
         }

     public:
         consteval DotItem(std::string_view input)
           : m_input_char(input[0]), m_input(input)
         {
             Deque<TokenType> m_tokens{};

             CERBLIB_UNROLL_N(1)
             while (m_token.tokenClass != EoF) {
                 GetNextToken();
                 m_tokens.push_back(m_token);
             }
         }
     };
      */
}// namespace cerb::lex

#endif /* CERBERUS_LEX_HPP */
