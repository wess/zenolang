#include "llvm_codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

// Implementation of an extremely simplified version for demonstration
// A real implementation would need to handle all AST node types

// Main code generation function
void llvm_generate_code(LLVMGenContext* ctx, AST_Node* node) {
    if (!node) return;
    
    // Basic implementation that just handles function declarations for "main"
    if (node->type == NODE_PROGRAM) {
        // Create printf declaration
        LLVMTypeRef paramTypes[] = { LLVMPointerType(LLVMInt8Type(), 0) };
        LLVMTypeRef funcType = LLVMFunctionType(LLVMInt32Type(), paramTypes, 1, 1);
        LLVMValueRef printfFunc = LLVMAddFunction(ctx->module, "printf", funcType);
        
        // Create main function
        LLVMTypeRef mainType = LLVMFunctionType(LLVMInt32Type(), NULL, 0, 0);
        LLVMValueRef mainFunc = LLVMAddFunction(ctx->module, "main", mainType);
        
        // Create entry block
        LLVMBasicBlockRef entry = LLVMAppendBasicBlock(mainFunc, "entry");
        LLVMPositionBuilderAtEnd(ctx->builder, entry);
        
        // Create a hello world string
        LLVMValueRef str = LLVMBuildGlobalStringPtr(ctx->builder, "Hello, LLVM World!\n", "hello_str");
        
        // Call printf - using LLVMBuildCall2 instead of LLVMBuildCall
        LLVMValueRef args[] = { str };
        LLVMBuildCall2(ctx->builder, funcType, printfFunc, args, 1, "");
        
        // Return 0
        LLVMBuildRet(ctx->builder, LLVMConstInt(LLVMInt32Type(), 0, 0));
    }
}

// Compile LLVM IR to object file
int llvm_compile_to_object(LLVMGenContext* ctx, const char* output_path) {
    if (!ctx || !output_path) return 1;
    
    // Initialize target
    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllTargets();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllAsmParsers();
    LLVMInitializeAllAsmPrinters();
    
    // Get the target
    char* triple = LLVMGetDefaultTargetTriple();
    LLVMTargetRef target;
    char* error = NULL;
    
    if (LLVMGetTargetFromTriple(triple, &target, &error) != 0) {
        fprintf(stderr, "Error getting target: %s\n", error);
        LLVMDisposeMessage(error);
        LLVMDisposeMessage(triple);
        return 1;
    }
    
    // Create target machine
    LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(
        target, triple, "", "", LLVMCodeGenLevelDefault,
        LLVMRelocDefault, LLVMCodeModelDefault);
    
    LLVMDisposeMessage(triple);
    
    // Set target triple in module
    LLVMSetTarget(ctx->module, LLVMGetDefaultTargetTriple());
    
    // Write object file
    if (LLVMTargetMachineEmitToFile(
            target_machine, ctx->module, (char*)output_path,
            LLVMObjectFile, &error) != 0) {
        fprintf(stderr, "Error writing object file: %s\n", error);
        LLVMDisposeMessage(error);
        LLVMDisposeTargetMachine(target_machine);
        return 1;
    }
    
    LLVMDisposeTargetMachine(target_machine);
    return 0;
}

// Compile LLVM IR to executable
int llvm_compile_to_executable(LLVMGenContext* ctx, const char* output_path) {
    if (!ctx || !output_path) return 1;
    
    // First compile to object file
    char obj_path[1024];
    snprintf(obj_path, sizeof(obj_path), "%s.o", output_path);
    
    if (llvm_compile_to_object(ctx, obj_path) != 0) {
        return 1;
    }
    
    // Then link with compiler
    char link_cmd[2048];
    snprintf(link_cmd, sizeof(link_cmd), 
             "cc %s -o %s", obj_path, output_path);
    
    int ret = system(link_cmd);
    
    // Clean up object file
    remove(obj_path);
    
    return ret;
}
