#ifndef CODEGEN_STATEMENT_H
#define CODEGEN_STATEMENT_H

#include "context.h"
#include "../ast.h"

// Statement generation functions
void generate_if_statement(CodeGenContext* ctx, AST_Node* node);
void generate_match_statement(CodeGenContext* ctx, AST_Node* node);
void generate_return_statement(CodeGenContext* ctx, AST_Node* node);
void generate_compound_statement(CodeGenContext* ctx, AST_Node* node);
void generate_compound_statement_contents(CodeGenContext* ctx, AST_Node* node);

#endif // CODEGEN_STATEMENT_H
