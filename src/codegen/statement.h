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
void generate_c_style_for_statement(CodeGenContext* ctx, AST_Node* node); // Added
void generate_for_in_statement(CodeGenContext* ctx, AST_Node* node);      // Renamed from generate_for_statement
void generate_for_map_statement(CodeGenContext* ctx, AST_Node* node);     // Renamed from generate_foreach_statement
void generate_while_statement(CodeGenContext* ctx, AST_Node* node);

#endif // CODEGEN_STATEMENT_H
