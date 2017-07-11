#include "astree.h"

/* Does a semantic check on `tree`. */
void ast_semantic_check(struct astree* tree);

/* Compute the type of the expressions*/
int resolve_expr_type(struct astree *tree);
