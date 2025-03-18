#ifndef LLVM_CONTEXT_H
#define LLVM_CONTEXT_H

#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Target.h>
#include "../symtab.h"
#include "../ast.h"  // Added this include for TypeInfo

// LLVM code generation context
typedef struct LLVMGenContext {
    LLVMContextRef context;       // LLVM context
    LLVMModuleRef module;         // LLVM module
    LLVMBuilderRef builder;       // LLVM IR builder
    
    SymbolTable* symtab;          // Symbol table (reused from existing code)
    
    // Function-related state
    LLVMValueRef current_function; // Current function being generated
    LLVMBasicBlockRef entry_block; // Entry block of current function
    LLVMBasicBlockRef current_block; // Current block being generated
    
    // Value caching
    LLVMValueRef* value_map;      // Map from variable names to LLVM values
    
    // Control flow state
    int label_count;              // Counter for labels
    
    // Debug info
    int debug_info;               // Whether to generate debug info
} LLVMGenContext;

// Initialize LLVM code generation context
LLVMGenContext* init_llvm_codegen(const char* module_name, int debug_info);

// Clean up LLVM code generation context
void cleanup_llvm_codegen(LLVMGenContext* ctx);

// Get LLVM type from Zeno type
LLVMTypeRef llvm_get_type(LLVMGenContext* ctx, TypeInfo* type);

// Store value in context value map
void llvm_store_value(LLVMGenContext* ctx, const char* name, LLVMValueRef value);

// Get value from context value map
LLVMValueRef llvm_get_value(LLVMGenContext* ctx, const char* name);

// Write LLVM module to file
int write_llvm_module(LLVMGenContext* ctx, const char* output_path);

#endif // LLVM_CONTEXT_H
