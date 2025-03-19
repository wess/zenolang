#ifndef LLVM_CODEGEN_H
#define LLVM_CODEGEN_H

#include "llvm_context.h"
#include "../ast.h"

// Forward declarations
LLVMValueRef llvm_generate_expression(LLVMGenContext* ctx, AST_Node* node);
void llvm_generate_statement(LLVMGenContext* ctx, AST_Node* node);
void llvm_generate_declaration(LLVMGenContext* ctx, AST_Node* node);

// Generate LLVM IR from AST
void llvm_generate_code(LLVMGenContext* ctx, AST_Node* node);

// Compile LLVM IR to object file
int llvm_compile_to_object(LLVMGenContext* ctx, const char* output_path);

// Compile LLVM IR to executable
int llvm_compile_to_executable(LLVMGenContext* ctx, const char* output_path);

#endif // LLVM_CODEGEN_H
