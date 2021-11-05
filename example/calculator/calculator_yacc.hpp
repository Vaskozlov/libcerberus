#ifndef CERBERUS_CALCULATOR_YACC_HPP
#define CERBERUS_CALCULATOR_YACC_HPP

#include "calculator_imp_yacc.hpp"
#include <cerberus/analyzation/parser/tree.hpp>

namespace cap = cerb::analysis::parser;
using token_t = typename CalculatorImpYacc<>::token_t;
using TreeNode = cap::TreeNode<token_t>;
using ParserNode = cap::ParserNode<token_t>;

#endif /* CERBERUS_CALCULATOR_YACC_HPP */
