/**
 * Lists the types of AST Nodes
 */
typedef enum ASTNodeType {
    CHAR_NODE,
    STAR_NODE,
    PLUS_NODE,
    QUESTION_NODE,
    OR_NODE,
    CONCAT_NODE,
} ASTNodeType;


/**
 * Represents a node in the Abstract Syntax Tree
 *
 * Members
 *     type: The type of AST Node this is.
 *     child1: A pointer to the current node's child node
 *     extra: This could be one of two values
 *         - If type is CHAR_NODE, this stores a char for the character
 *         - If type is OR_NODE or CONCAT_NODE, a pointer to the second child.
 */
typedef struct ASTNode {
    /* This member is used to determine whether the other members have a
       meaningful value or not */
    ASTNodeType type;

    /* For nodes with only one child, this is that child
       For nodes with two children, this is the left child
       For CHAR_NODE, this does not contain a meaningful value */
    struct ASTNode* child1;

    /* STAR, PLUS and QUESTION nodes have no meaningful value for this member.
       Inner member character is used by CHAR_NODE
       Inner member child2 is used by OR_NODE and CONCAT_NODE */
    union {
        // Value of the character from a CHAR_NODE
        char character;

        // For nodes with two children, this is the right child
        struct ASTNode* child2;
    } extra;
} ASTNode;
