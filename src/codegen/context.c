#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "context.h"

// Initialize code generation context
CodeGenContext* init_codegen(FILE* output) {
    CodeGenContext* ctx = (CodeGenContext*)malloc(sizeof(CodeGenContext));
    if (!ctx) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    ctx->output = output;
    ctx->symtab = init_symbol_table();
    ctx->indentation = 0;
    ctx->temp_var_count = 0;
    ctx->label_count = 0;
    ctx->buffer = NULL;
    ctx->buffer_size = 0;
    ctx->in_async_function = 0; // Initialize to not in async function
    
    return ctx;
}

// Clean up code generation context
void cleanup_codegen(CodeGenContext* ctx) {
    if (ctx) {
        if (ctx->symtab) {
            cleanup_symbol_table(ctx->symtab);
        }
        
        if (ctx->buffer) {
            free(ctx->buffer);
        }
        
        free(ctx);
    }
}
