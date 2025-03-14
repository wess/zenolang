#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast.h"
#include "symtab.h"

// Code generation context
typedef struct {
    FILE* output;           // Output file
    SymbolTable* symtab;    // Symbol table
    int indentation;        // Current indentation level
    int temp_var_count;     // Counter for temporary variables
    int label_count;        // Counter for labels
    char* buffer;           // Temporary buffer for string operations
    size_t buffer_size;     // Size of the temporary buffer
} CodeGenContext;

// Initialize code generation context
CodeGenContext* init_codegen(FILE* output);

// Generate C code from the AST
void generate_code(CodeGenContext* ctx, AST_Node* node);

// Helper functions
void generate_program(CodeGenContext* ctx, AST_Node* node);
void generate_function(CodeGenContext* ctx, AST_Node* node);
void generate_variable(CodeGenContext* ctx, AST_Node* node);
void generate_struct(CodeGenContext* ctx, AST_Node* node);
void generate_type_declaration(CodeGenContext* ctx, AST_Node* node);
void generate_import(CodeGenContext* ctx, AST_Node* node);
void generate_expression(CodeGenContext* ctx, AST_Node* node);
void generate_if_statement(CodeGenContext* ctx, AST_Node* node);
void generate_match_statement(CodeGenContext* ctx, AST_Node* node);
void generate_return_statement(CodeGenContext* ctx, AST_Node* node);
void generate_compound_statement(CodeGenContext* ctx, AST_Node* node);

// Utility functions
void indent(CodeGenContext* ctx);
void increase_indent(CodeGenContext* ctx);
void decrease_indent(CodeGenContext* ctx);
char* get_temp_var_name(CodeGenContext* ctx);
char* get_label_name(CodeGenContext* ctx);
char* get_c_type(TypeInfo* type);

// Clean up code generation context
void cleanup_codegen(CodeGenContext* ctx);

#endif // CODEGEN_H
