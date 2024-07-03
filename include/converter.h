#ifndef REGEX_CONVERTER_H
#define REGEX_CONVERTER_H

#include "nfa.h"
#include "ast.h"

/**
 * Convert an AST rooted at root to a NFA
 * AST = Abstract Syntax Tree
 * NFA = Non-deterministic Finite Automata
 *
 * @param  root The root of the abstract syntax tree
 * 
 * @return An NFA equivalent to the given AST
 */
NFA* convert_ast_to_nfa(ASTNode* root);

#endif // REGEX_CONVERTER_H
