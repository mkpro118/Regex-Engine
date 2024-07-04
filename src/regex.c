#include "regex.h"

// Create a heap allocated and initialized regex buffer.
Regex* regex_create(char* pattern) {
    Regex* buf = malloc(sizeof(Regex));

    if (regex_init(buf, pattern) < 0) {
        free(buf);
        return NULL;
    }

    return buf;
}

// Initialize a given regex buffer.
int regex_init(Regex* regex_buf, char* pattern) {
    if (regex_buf == NULL) {
        return -1;
    }

    *regex_buf = (Regex) {
        .nfa = NULL,
        .is_compiled = false,
        .pattern = pattern,
    };

    if (pattern == NULL) {
        return 0;
    }

    if (regex_compile(regex_buf, pattern) < 0) {
        regex_free(regex_buf);
        return -1;
    }

    return 0;
}

// Compile a given regex pattern.
int regex_compile(Regex* regex_buf, char* pattern) {
    if (regex_buf == NULL || pattern == NULL) {
        return -1;
    }

    if (regex_buf->is_compiled) {
        // Skip compiling if already compiled with the same pattern
        if (strcmp(regex_buf->pattern, pattern) == 0) {
            return 1;
        }

        // Otherwise, reset and re-initialize
        regex_free(regex_buf);
        regex_init(regex_buf, pattern);
    }

    // Initialize a lexer with the pattern
    Lexer lexer;
    if (lexer_init(&lexer, pattern) < 0) {
        return -1;
    }

    // Initialize a parser with the lexer
    Parser parser;
    if (parser_init(&parser, &lexer) < 0) {
        return -1;
    }

    // Parse the given pattern to build a regex AST
    ASTNode* root = parse(&parser);

    // Release resources
    parser_free(&parser);
    lexer_free(&lexer);

    if (root == NULL) {
        return -1;
    }

    // Create a NFA with the AST
    NFA* nfa = convert_ast_to_nfa(root);

    // Release resources
    ast_node_free(root);
    free(root);

    if (nfa == NULL) {
        return -1;
    }

    // Initialize a compiled regex
    *regex_buf = (Regex) {
        .nfa = nfa,
        .is_compiled = true,
        .pattern = pattern,
    };

    return 0;
}

// Test whether the given string matches the given regex.
bool regex_match(Regex* regex_buf, char* string) {
    if (regex_buf == NULL || string == NULL) {
        return false;
    }

    if (!regex_buf->is_compiled || regex_buf->nfa == NULL) {
        return false;
    }

    return nfa_match(regex_buf->nfa, string);
}

// Release the memory used by the given regex structure
void regex_free(Regex* regex_buf) {
    if (regex_buf == NULL) {
        return;
    }

    nfa_free(regex_buf->nfa);
}
