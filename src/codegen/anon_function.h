#ifndef CODEGEN_ANON_FUNCTION_H
#define CODEGEN_ANON_FUNCTION_H

#include <stdio.h>
#include "context.h"
#include "../ast.h"

// Anonymous function structure
typedef struct {
    char* name;
    char* return_type;
    char* param_list;
    char* param_types;
    char* body;
} AnonFunction;

// Anonymous function functions
void generate_anonymous_function(CodeGenContext* ctx, AST_Node* node);
int add_anon_function(const char* name, const char* return_type, const char* param_list, const char* param_types, const char* body);
void generate_all_anon_functions(FILE* output);

#endif // CODEGEN_ANON_FUNCTION_H
