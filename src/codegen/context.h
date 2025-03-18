#ifndef CODEGEN_CONTEXT_H
#define CODEGEN_CONTEXT_H

#include <stdio.h>
#include "../symtab.h"

// Code generation context
typedef struct CodeGenContext {
    FILE* output;           // Output file
    SymbolTable* symtab;    // Symbol table
    int indentation;        // Current indentation level
    int temp_var_count;     // Counter for temporary variables
    int label_count;        // Counter for labels
    char* buffer;           // Temporary buffer for string operations
    size_t buffer_size;     // Size of the temporary buffer
    int in_async_function;  // Flag to indicate we're inside an async function
} CodeGenContext;

// Initialize code generation context
CodeGenContext* init_codegen(FILE* output);

// Clean up code generation context
void cleanup_codegen(CodeGenContext* ctx);

#endif // CODEGEN_CONTEXT_H
