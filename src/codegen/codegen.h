#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "../ast.h"
#include "../symtab.h"

// Forward declarations
typedef struct CodeGenContext CodeGenContext;

// Include all module headers
#include "context.h"
#include "utils.h"
#include "expression.h"
#include "statement.h"
#include "declaration.h"
#include "anon_function.h"

// Generate C code from the AST
void generate_code(CodeGenContext* ctx, AST_Node* node);

// Generate all anonymous functions at program level
void generate_all_anon_functions(FILE* output);

#endif // CODEGEN_H
