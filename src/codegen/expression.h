#ifndef CODEGEN_EXPRESSION_H
#define CODEGEN_EXPRESSION_H

#include "context.h"
#include "../ast.h"

// Generate code for expression
void generate_expression(CodeGenContext* ctx, AST_Node* node);

#endif // CODEGEN_EXPRESSION_H
