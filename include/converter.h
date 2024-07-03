#ifndef REGEX_CONVERTER_H
#define REGEX_CONVERTER_H

#include "nfa.h"
#include "ast.h"

NFA* convert_ast_to_nfa(ASTNode* root);

#endif // REGEX_CONVERTER_H
