#ifndef CODEGEN_DECLARATION_H
#define CODEGEN_DECLARATION_H

#include "context.h"
#include "../ast.h"

// Declaration generation functions
void generate_program(CodeGenContext* ctx, AST_Node* node);
void generate_function(CodeGenContext* ctx, AST_Node* node);
void generate_variable(CodeGenContext* ctx, AST_Node* node);
void generate_struct(CodeGenContext* ctx, AST_Node* node);
void generate_type_declaration(CodeGenContext* ctx, AST_Node* node);
void generate_import(CodeGenContext* ctx, AST_Node* node);

#endif // CODEGEN_DECLARATION_H
