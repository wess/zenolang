#include "llvm_context.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Initialize LLVM code generation context
LLVMGenContext* init_llvm_codegen(const char* module_name, int debug_info) {
    LLVMGenContext* ctx = (LLVMGenContext*)malloc(sizeof(LLVMGenContext));
    if (!ctx) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    // Initialize LLVM components
    ctx->context = LLVMContextCreate();
    ctx->module = LLVMModuleCreateWithNameInContext(module_name, ctx->context);
    ctx->builder = LLVMCreateBuilderInContext(ctx->context);
    
    // Initialize symbol table
    ctx->symtab = init_symbol_table();
    
    // Initialize state
    ctx->current_function = NULL;
    ctx->entry_block = NULL;
    ctx->current_block = NULL;
    ctx->value_map = NULL;
    ctx->label_count = 0;
    ctx->debug_info = debug_info;
    
    return ctx;
}

// Clean up LLVM code generation context
void cleanup_llvm_codegen(LLVMGenContext* ctx) {
    if (!ctx) return;
    
    // Clean up LLVM components
    LLVMDisposeBuilder(ctx->builder);
    LLVMDisposeModule(ctx->module);
    LLVMContextDispose(ctx->context);
    
    // Clean up symbol table
    cleanup_symbol_table(ctx->symtab);
    
    // Free the context itself
    free(ctx);
}

// Get LLVM type from Zeno type
LLVMTypeRef llvm_get_type(LLVMGenContext* ctx, TypeInfo* type) {
    if (!type) return LLVMVoidType();
    
    if (strcmp(type->name, "int") == 0) {
        return LLVMInt32Type();
    } else if (strcmp(type->name, "float") == 0) {
        return LLVMFloatType();
    } else if (strcmp(type->name, "bool") == 0) {
        return LLVMInt1Type();
    } else if (strcmp(type->name, "string") == 0) {
        return LLVMPointerType(LLVMInt8Type(), 0);
    } else if (strcmp(type->name, "void") == 0) {
        return LLVMVoidType();
    } else if (strcmp(type->name, "array") == 0 && type->generic_type) {
        // For arrays, we need to create a pointer to the inner type
        LLVMTypeRef inner_type = llvm_get_type(ctx, type->generic_type);
        return LLVMPointerType(inner_type, 0);
    } else if (strcmp(type->name, "map") == 0) {
        // Represent map as an opaque pointer (i8*) for now
        // TODO: Define a proper runtime struct type for maps
        return LLVMPointerType(LLVMInt8Type(), 0);
    } else {
        // For custom types (structs), use opaque pointer type for now
        // TODO: Look up or create named struct types
        return LLVMPointerType(LLVMInt8Type(), 0);
    }
}

// Store value in context value map
void llvm_store_value(LLVMGenContext* ctx, const char* name, LLVMValueRef value) {
    // Real implementation would use a hash map for efficiency
    // This is just a placeholder
}

// Get value from context value map
LLVMValueRef llvm_get_value(LLVMGenContext* ctx, const char* name) {
    // Real implementation would look up in the hash map
    // This is just a placeholder
    return NULL;
}

// Write LLVM module to file
int write_llvm_module(LLVMGenContext* ctx, const char* output_path) {
    if (!ctx || !output_path) return 1;
    
    // Verify the module
    char* error = NULL;
    LLVMVerifyModule(ctx->module, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);
    
    // Write bitcode to file
    if (LLVMWriteBitcodeToFile(ctx->module, output_path) != 0) {
        fprintf(stderr, "Error writing bitcode to file\n");
        return 1;
    }
    
    return 0;
}
