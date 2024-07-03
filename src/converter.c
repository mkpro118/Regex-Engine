#include "ast.h"
#include "nfa.h"
#include "nfa_state.h"
#include "converter.h"

// Convert AST to NFA
NFA* convert_ast_to_nfa(ASTNode* node);
