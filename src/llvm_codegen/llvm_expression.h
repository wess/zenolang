// src/llvm_codegen/llvm_expression.h
#ifndef LLVM_EXPRESSION_H
#define LLVM_EXPRESSION_H

#include "llvm_context.h"
#include "../ast.h"

// Generate LLVM IR for expressions
LLVMValueRef llvm_generate_expression(LLVMGenContext* ctx, AST_Node* node);

// Helper functions
LLVMValueRef llvm_generate_binary_op(LLVMGenContext* ctx, AST_Node* node);
LLVMValueRef llvm_generate_unary_op(LLVMGenContext* ctx, AST_Node* node);
LLVMValueRef llvm_generate_function_call(LLVMGenContext* ctx, AST_Node* node);
LLVMValueRef llvm_generate_struct_init(LLVMGenContext* ctx, AST_Node* node);
LLVMValueRef llvm_generate_anonymous_function(LLVMGenContext* ctx, AST_Node* node);
LLVMValueRef llvm_generate_pipe(LLVMGenContext* ctx, AST_Node* node);

#endif // LLVM_EXPRESSION_H

// src/llvm_codegen/llvm_expression.c
#include "llvm_expression.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Main expression generation function
LLVMValueRef llvm_generate_expression(LLVMGenContext* ctx, AST_Node* node) {
    if (!node) return NULL;
    
    switch (node->type) {
        case NODE_LITERAL_INT:
            return LLVMConstInt(LLVMInt32Type(), atoi(node->data.literal.value), 0);
            
        case NODE_LITERAL_FLOAT:
            return LLVMConstReal(LLVMFloatType(), atof(node->data.literal.value));
            
        case NODE_LITERAL_STRING:
            return LLVMBuildGlobalStringPtr(ctx->builder, node->data.literal.value, "str");
            
        case NODE_LITERAL_BOOL:
            return LLVMConstInt(LLVMInt1Type(), 
                             strcmp(node->data.literal.value, "true") == 0 ? 1 : 0, 0);
                             
        case NODE_IDENTIFIER:
            // Look up variable reference
            return LLVMBuildLoad(ctx->builder, 
                              llvm_get_value(ctx, node->data.identifier.name), 
                              node->data.identifier.name);
            
        case NODE_BINARY_OP:
            return llvm_generate_binary_op(ctx, node);
            
        case NODE_UNARY_OP:
            return llvm_generate_unary_op(ctx, node);
            
        case NODE_FUNCTION_CALL:
            return llvm_generate_function_call(ctx, node);
            
        case NODE_STRUCT_INIT:
            return llvm_generate_struct_init(ctx, node);
            
        case NODE_ANONYMOUS_FUNCTION:
            return llvm_generate_anonymous_function(ctx, node);
            
        case NODE_PIPE:
            return llvm_generate_pipe(ctx, node);
            
        case NODE_MEMBER_ACCESS: {
            // Get the struct object
            LLVMValueRef object = llvm_generate_expression(ctx, node->data.member_access.object);
            
            // Extract the member (this is simplified - would need to get actual struct type info)
            int field_index = 0; // Would need to look up the actual field index
            
            // Get a pointer to the field
            LLVMValueRef indices[2] = {
                LLVMConstInt(LLVMInt32Type(), 0, 0),
                LLVMConstInt(LLVMInt32Type(), field_index, 0)
            };
            
            LLVMValueRef field_ptr = LLVMBuildGEP(ctx->builder, object, indices, 2, 
                                               node->data.member_access.member);
                                               
            // Load the field
            return LLVMBuildLoad(ctx->builder, field_ptr, node->data.member_access.member);
        }
        
        case NODE_ASSIGNMENT: {
            // Get the value to assign
            LLVMValueRef value = llvm_generate_expression(ctx, node->data.assignment.value);
            
            // Get the variable to assign to
            LLVMValueRef var_ptr = llvm_get_value(ctx, node->data.assignment.name);
            
            // Build the store instruction
            return LLVMBuildStore(ctx->builder, value, var_ptr);
        }
        
        default:
            fprintf(stderr, "Unsupported expression node type %d\n", node->type);
            return NULL;
    }
}

// Generate binary operation
LLVMValueRef llvm_generate_binary_op(LLVMGenContext* ctx, AST_Node* node) {
    LLVMValueRef left = llvm_generate_expression(ctx, node->data.binary_op.left);
    LLVMValueRef right = llvm_generate_expression(ctx, node->data.binary_op.right);
    
    if (!left || !right) return NULL;
    
    // Depending on the operation, call the appropriate LLVM builder function
    switch (node->data.binary_op.op) {
        case OP_ADD:
            if (LLVMTypeOf(left) == LLVMFloatType() || LLVMTypeOf(right) == LLVMFloatType()) {
                // For floating point addition
                if (LLVMTypeOf(left) != LLVMFloatType()) {
                    left = LLVMBuildSIToFP(ctx->builder, left, LLVMFloatType(), "cast_to_float");
                }
                if (LLVMTypeOf(right) != LLVMFloatType()) {
                    right = LLVMBuildSIToFP(ctx->builder, right, LLVMFloatType(), "cast_to_float");
                }
                
                return LLVMBuildFAdd(ctx->builder, left, right, "fadd");
            } else {
                // For integer addition
                return LLVMBuildAdd(ctx->builder, left, right, "add");
            }
            
        case OP_SUB:
            if (LLVMTypeOf(left) == LLVMFloatType() || LLVMTypeOf(right) == LLVMFloatType()) {
                // For floating point subtraction
                if (LLVMTypeOf(left) != LLVMFloatType()) {
                    left = LLVMBuildSIToFP(ctx->builder, left, LLVMFloatType(), "cast_to_float");
                }
                if (LLVMTypeOf(right) != LLVMFloatType()) {
                    right = LLVMBuildSIToFP(ctx->builder, right, LLVMFloatType(), "cast_to_float");
                }
                
                return LLVMBuildFSub(ctx->builder, left, right, "fsub");
            } else {
                // For integer subtraction
                return LLVMBuildSub(ctx->builder, left, right, "sub");
            }
            
        case OP_MUL:
            if (LLVMTypeOf(left) == LLVMFloatType() || LLVMTypeOf(right) == LLVMFloatType()) {
                // For floating point multiplication
                if (LLVMTypeOf(left) != LLVMFloatType()) {
                    left = LLVMBuildSIToFP(ctx->builder, left, LLVMFloatType(), "cast_to_float");
                }
                if (LLVMTypeOf(right) != LLVMFloatType()) {
                    right = LLVMBuildSIToFP(ctx->builder, right, LLVMFloatType(), "cast_to_float");
                }
                
                return LLVMBuildFMul(ctx->builder, left, right, "fmul");
            } else {
                // For integer multiplication
                return LLVMBuildMul(ctx->builder, left, right, "mul");
            }
            
        case OP_DIV:
            if (LLVMTypeOf(left) == LLVMFloatType() || LLVMTypeOf(right) == LLVMFloatType()) {
                // For floating point division
                if (LLVMTypeOf(left) != LLVMFloatType()) {
                    left = LLVMBuildSIToFP(ctx->builder, left, LLVMFloatType(), "cast_to_float");
                }
                if (LLVMTypeOf(right) != LLVMFloatType()) {
                    right = LLVMBuildSIToFP(ctx->builder, right, LLVMFloatType(), "cast_to_float");
                }
                
                return LLVMBuildFDiv(ctx->builder, left, right, "fdiv");
            } else {
                // For integer division
                return LLVMBuildSDiv(ctx->builder, left, right, "div");
            }
            
        case OP_MOD:
            if (LLVMTypeOf(left) == LLVMFloatType() || LLVMTypeOf(right) == LLVMFloatType()) {
                // For floating point modulo - need to use fmod function
                // This would require the math library
                fprintf(stderr, "Floating point modulo not yet implemented\n");
                return NULL;
            } else {
                // For integer modulo
                return LLVMBuildSRem(ctx->builder, left, right, "mod");
            }
            
        case OP_EQ:
            if (LLVMTypeOf(left) == LLVMFloatType() || LLVMTypeOf(right) == LLVMFloatType()) {
                // For floating point equality
                if (LLVMTypeOf(left) != LLVMFloatType()) {
                    left = LLVMBuildSIToFP(ctx->builder, left, LLVMFloatType(), "cast_to_float");
                }
                if (LLVMTypeOf(right) != LLVMFloatType()) {
                    right = LLVMBuildSIToFP(ctx->builder, right, LLVMFloatType(), "cast_to_float");
                }
                
                return LLVMBuildFCmp(ctx->builder, LLVMRealOEQ, left, right, "fcmp_eq");
            } else {
                // For integer equality
                return LLVMBuildICmp(ctx->builder, LLVMIntEQ, left, right, "icmp_eq");
            }
            
        case OP_NEQ:
            if (LLVMTypeOf(left) == LLVMFloatType() || LLVMTypeOf(right) == LLVMFloatType()) {
                // For floating point inequality
                if (LLVMTypeOf(left) != LLVMFloatType()) {
                    left = LLVMBuildSIToFP(ctx->builder, left, LLVMFloatType(), "cast_to_float");
                }
                if (LLVMTypeOf(right) != LLVMFloatType()) {
                    right = LLVMBuildSIToFP(ctx->builder, right, LLVMFloatType(), "cast_to_float");
                }
                
                return LLVMBuildFCmp(ctx->builder, LLVMRealONE, left, right, "fcmp_ne");
            } else {
                // For integer inequality
                return LLVMBuildICmp(ctx->builder, LLVMIntNE, left, right, "icmp_ne");
            }
            
        case OP_LT:
            if (LLVMTypeOf(left) == LLVMFloatType() || LLVMTypeOf(right) == LLVMFloatType()) {
                // For floating point less than
                if (LLVMTypeOf(left) != LLVMFloatType()) {
                    left = LLVMBuildSIToFP(ctx->builder, left, LLVMFloatType(), "cast_to_float");
                }
                if (LLVMTypeOf(right) != LLVMFloatType()) {
                    right = LLVMBuildSIToFP(ctx->builder, right, LLVMFloatType(), "cast_to_float");
                }
                
                return LLVMBuildFCmp(ctx->builder, LLVMRealOLT, left, right, "fcmp_lt");
            } else {
                // For integer less than
                return LLVMBuildICmp(ctx->builder, LLVMIntSLT, left, right, "icmp_lt");
            }
            
        // ... Other operations ...
        
        default:
            fprintf(stderr, "Unsupported binary operation %d\n", node->data.binary_op.op);
            return NULL;
    }
}

// Generate function call
LLVMValueRef llvm_generate_function_call(LLVMGenContext* ctx, AST_Node* node) {
    // Get function from module
    LLVMValueRef func = LLVMGetNamedFunction(ctx->module, node->data.function_call.name);
    if (!func) {
        fprintf(stderr, "Function '%s' not found\n", node->data.function_call.name);
        return NULL;
    }
    
    // Generate arguments
    int arg_count = 0;
    ExpressionList* arg = node->data.function_call.arguments;
    
    // Count arguments
    while (arg) {
        arg_count++;
        arg = arg->next;
    }
    
    // Create an array of argument values
    LLVMValueRef* args = NULL;
    if (arg_count > 0) {
        args = (LLVMValueRef*)malloc(sizeof(LLVMValueRef) * arg_count);
        if (!args) {
            fprintf(stderr, "Memory allocation failed\n");
            return NULL;
        }
        
        // Generate code for each argument
        arg = node->data.function_call.arguments;
        for (int i = 0; i < arg_count; i++) {
            args[i] = llvm_generate_expression(ctx, arg->expression);
            arg = arg->next;
        }
    }
    
    // Call the function
    LLVMValueRef call = LLVMBuildCall(ctx->builder, func, args, arg_count, "");
    
    // Clean up
    if (args) free(args);
    
    return call;
}

// Generate pipe operation
LLVMValueRef llvm_generate_pipe(LLVMGenContext* ctx, AST_Node* node) {
    // For pipe operator (a |> f), generate f(a)
    // This is similar to function call but with the left side as first argument
    
    if (node->data.pipe.right->type == NODE_FUNCTION_CALL) {
        // Get the function
        LLVMValueRef func = LLVMGetNamedFunction(ctx->module, 
                                              node->data.pipe.right->data.function_call.name);
        if (!func) {
            fprintf(stderr, "Function '%s' not found\n", 
                    node->data.pipe.right->data.function_call.name);
            return NULL;
        }
        
        // Generate the left side (will be first argument)
        LLVMValueRef left = llvm_generate_expression(ctx, node->data.pipe.left);
        
        // Count additional arguments
        int arg_count = 1; // Start with 1 for the left side
        ExpressionList* arg = node->data.pipe.right->data.function_call.arguments;
        
        while (arg) {
            arg_count++;
            arg = arg->next;
        }
        
        // Create an array of argument values
        LLVMValueRef* args = (LLVMValueRef*)malloc(sizeof(LLVMValueRef) * arg_count);
        if (!args) {
            fprintf(stderr, "Memory allocation failed\n");
            return NULL;
        }
        
        // First argument is the left side
        args[0] = left;
        
        // Generate code for additional arguments
        arg = node->data.pipe.right->data.function_call.arguments;
        for (int i = 1; i < arg_count; i++) {
            args[i] = llvm_generate_expression(ctx, arg->expression);
            arg = arg->next;
        }
        
        // Call the function
        LLVMValueRef call = LLVMBuildCall(ctx->builder, func, args, arg_count, "");
        
        // Clean up
        free(args);
        
        return call;
    } else if (node->data.pipe.right->type == NODE_IDENTIFIER) {
        // If right side is just an identifier, treat as function name
        LLVMValueRef func = LLVMGetNamedFunction(ctx->module, 
                                              node->data.pipe.right->data.identifier.name);
        if (!func) {
            fprintf(stderr, "Function '%s' not found\n", 
                    node->data.pipe.right->data.identifier.name);
            return NULL;
        }
        
        // Generate the left side (only argument)
        LLVMValueRef left = llvm_generate_expression(ctx, node->data.pipe.left);
        
        // Call the function with one argument
        LLVMValueRef args[1] = { left };
        return LLVMBuildCall(ctx->builder, func, args, 1, "");
    } else {
        fprintf(stderr, "Unsupported pipe operation - right side must be function\n");
        return NULL;
    }
}

// src/llvm_codegen/llvm_statement.h
#ifndef LLVM_STATEMENT_H
#define LLVM_STATEMENT_H

#include "llvm_context.h"
#include "../ast.h"

// Generate LLVM IR for statements
void llvm_generate_statement(LLVMGenContext* ctx, AST_Node* node);

// Helper functions
void llvm_generate_if_statement(LLVMGenContext* ctx, AST_Node* node);
void llvm_generate_match_statement(LLVMGenContext* ctx, AST_Node* node);
void llvm_generate_return_statement(LLVMGenContext* ctx, AST_Node* node);
void llvm_generate_compound_statement(LLVMGenContext* ctx, AST_Node* node);

#endif // LLVM_STATEMENT_H

// src/llvm_codegen/llvm_statement.c
#include "llvm_statement.h"
#include "llvm_expression.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Main statement generation function
void llvm_generate_statement(LLVMGenContext* ctx, AST_Node* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_IF:
            llvm_generate_if_statement(ctx, node);
            break;
            
        case NODE_MATCH:
            llvm_generate_match_statement(ctx, node);
            break;
            
        case NODE_RETURN:
            llvm_generate_return_statement(ctx, node);
            break;
            
        case NODE_COMPOUND_STATEMENT:
            llvm_generate_compound_statement(ctx, node);
            break;
            
        default:
            // For expressions used as statements, just generate them
            // but discard the result
            llvm_generate_expression(ctx, node);
            break;
    }
}

// Generate if statement
void llvm_generate_if_statement(LLVMGenContext* ctx, AST_Node* node) {
    // Generate condition
    LLVMValueRef condition = llvm_generate_expression(ctx, node->data.if_stmt.condition);
    
    // Create the three basic blocks for the if/then/else
    LLVMBasicBlockRef then_block = LLVMAppendBasicBlock(ctx->current_function, "then");
    LLVMBasicBlockRef else_block = node->data.if_stmt.false_branch ? 
                                 LLVMAppendBasicBlock(ctx->current_function, "else") : NULL;
    LLVMBasicBlockRef merge_block = LLVMAppendBasicBlock(ctx->current_function, "merge");
    
    // Create the conditional branch
    if (else_block) {
        LLVMBuildCondBr(ctx->builder, condition, then_block, else_block);
    } else {
        LLVMBuildCondBr(ctx->builder, condition, then_block, merge_block);
    }
    
    // Generate code for 'then' block
    LLVMPositionBuilderAtEnd(ctx->builder, then_block);
    ctx->current_block = then_block;
    llvm_generate_statement(ctx, node->data.if_stmt.true_branch);
    
    // Add branch to merge block if there is no terminator
    if (!LLVMGetBasicBlockTerminator(then_block)) {
        LLVMBuildBr(ctx->builder, merge_block);
    }
    
    // Generate code for 'else' block if it exists
    if (else_block) {
        LLVMPositionBuilderAtEnd(ctx->builder, else_block);
        ctx->current_block = else_block;
        llvm_generate_statement(ctx, node->data.if_stmt.false_branch);
        
        // Add branch to merge block if there is no terminator
        if (!LLVMGetBasicBlockTerminator(else_block)) {
            LLVMBuildBr(ctx->builder, merge_block);
        }
    }
    
    // Start inserting into merge block
    LLVMPositionBuilderAtEnd(ctx->builder, merge_block);
    ctx->current_block = merge_block;
}

// Generate return statement
void llvm_generate_return_statement(LLVMGenContext* ctx, AST_Node* node) {
    if (node->data.return_stmt.expression) {
        // Generate expression and return its value
        LLVMValueRef return_value = llvm_generate_expression(ctx, node->data.return_stmt.expression);
        LLVMBuildRet(ctx->builder, return_value);
    } else {
        // Return void
        LLVMBuildRetVoid(ctx->builder);
    }
}

// Generate compound statement
void llvm_generate_compound_statement(LLVMGenContext* ctx, AST_Node* node) {
    // Create new scope
    enter_scope(ctx->symtab);
    
    // Generate code for each statement
    AST_Node* stmt = node->data.compound_stmt.statements->head;
    while (stmt) {
        llvm_generate_statement(ctx, stmt);
        
        // If this was a return or other terminator, stop generating statements
        if (LLVMGetBasicBlockTerminator(ctx->current_block)) {
            break;
        }
        
        stmt = stmt->next;
    }
    
    // End scope
    leave_scope(ctx->symtab);
}

// src/llvm_codegen/llvm_declaration.h
#ifndef LLVM_DECLARATION_H
#define LLVM_DECLARATION_H

#include "llvm_context.h"
#include "../ast.h"

// Generate LLVM IR for declarations
void llvm_generate_declaration(LLVMGenContext* ctx, AST_Node* node);

// Helper functions
void llvm_generate_function(LLVMGenContext* ctx, AST_Node* node);
void llvm_generate_variable(LLVMGenContext* ctx, AST_Node* node);
void llvm_generate_struct(LLVMGenContext* ctx, AST_Node* node);
void llvm_generate_type_declaration(LLVMGenContext* ctx, AST_Node* node);
void llvm_generate_import(LLVMGenContext* ctx, AST_Node* node);

#endif // LLVM_DECLARATION_H

// src/llvm_codegen/llvm_declaration.c
#include "llvm_declaration.h"
#include "llvm_expression.h"
#include "llvm_statement.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Main declaration generation function
void llvm_generate_declaration(LLVMGenContext* ctx, AST_Node* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_FUNCTION:
            llvm_generate_function(ctx, node);
            break;
            
        case NODE_VARIABLE:
            llvm_generate_variable(ctx, node);
            break;
            
        case NODE_STRUCT:
            llvm_generate_struct(ctx, node);
            break;
            
        case NODE_TYPE_DECLARATION:
            llvm_generate_type_declaration(ctx, node);
            break;
            
        case NODE_IMPORT:
            llvm_generate_import(ctx, node);
            break;
            
        default:
            fprintf(stderr, "Unsupported declaration node type %d\n", node->type);
            break;
    }
}

// Generate function
void llvm_generate_function(LLVMGenContext* ctx, AST_Node* node) {
    // Get function name
    const char* func_name = node->data.function.name;
    
    // Get return type
    LLVMTypeRef return_type = llvm_get_type(ctx, node->data.function.return_type);
    
    // Build parameter types and names
    int param_count = 0;
    AST_Node* param = node->data.function.parameters->head;
    
    // Count parameters
    while (param) {
        param_count++;
        param = param->next;
    }
    
    // Create parameter type array
    LLVMTypeRef* param_types = NULL;
    char** param_names = NULL;
    
    if (param_count > 0) {
        param_types = (LLVMTypeRef*)malloc(param_count * sizeof(LLVMTypeRef));
        param_names = (char**)malloc(param_count * sizeof(char*));
        
        if (!param_types || !param_names) {
            fprintf(stderr, "Memory allocation failed\n");
            if (param_types) free(param_types);
            if (param_names) free(param_names);
            return;
        }
        
        // Fill parameter types and names
        param = node->data.function.parameters->head;
        for (int i = 0; i < param_count; i++) {
            param_types[i] = llvm_get_type(ctx, param->data.variable.type);
            param_names[i] = strdup(param->data.variable.name);
            param = param->next;
        }
    }
    
    // Create function type
    LLVMTypeRef func_type = LLVMFunctionType(return_type, param_types, param_count, 0);
    
    // Create function declaration
    LLVMValueRef func = LLVMAddFunction(ctx->module, func_name, func_type);
    
    // Register function in symbol table
    add_symbol(ctx->symtab, (char*)func_name, SYMBOL_FUNCTION);
    
    // Add function to context
    ctx->current_function = func;
    
    // Create entry block
    ctx->entry_block = LLVMAppendBasicBlock(func, "entry");
    ctx->current_block = ctx->entry_block;
    
    // Position builder at entry block
    LLVMPositionBuilderAtEnd(ctx->builder, ctx->entry_block);
    
    // Create new scope for parameters
    enter_scope(ctx->symtab);
    
    // Process parameters
    for (int i = 0; i < param_count; i++) {
        // Get parameter value
        LLVMValueRef param_value = LLVMGetParam(func, i);
        LLVMSetValueName(param_value, param_names[i]);
        
        // Create allocation for parameter
        LLVMValueRef param_alloca = LLVMBuildAlloca(ctx->builder, param_types[i], param_names[i]);
        
        // Store parameter value to allocation
        LLVMBuildStore(ctx->builder, param_value, param_alloca);
        
        // Add to value map
        llvm_store_value(ctx, param_names[i], param_alloca);
        
        // Add to symbol table
        add_symbol(ctx->symtab, param_names[i], SYMBOL_VARIABLE);
    }
    
    // Generate function body
    if (node->data.function.body) {
        llvm_generate_statement(ctx, node->data.function.body);
    }
    
    // If there's no return statement at the end of a void function, add one
    if (return_type == LLVMVoidType() && !LLVMGetBasicBlockTerminator(ctx->current_block)) {
        LLVMBuildRetVoid(ctx->builder);
    }
    
    // End function scope
    leave_scope(ctx->symtab);
    
    // Clean up
    if (param_types) free(param_types);
    if (param_names) {
        for (int i = 0; i < param_count; i++) {
            free(param_names[i]);
        }
        free(param_names);
    }
}

// Generate variable declaration
void llvm_generate_variable(LLVMGenContext* ctx, AST_Node* node) {
    // Get variable name
    const char* var_name = node->data.variable.name;
    
    // Get variable type
    LLVMTypeRef var_type;
    if (node->data.variable.type) {
        var_type = llvm_get_type(ctx, node->data.variable.type);
    } else if (node->data.variable.initializer) {
        // For type inference, we need to look at the initializer
        // This is a simplification - in a real implementation, we'd do proper type inference
        LLVMValueRef init_val = llvm_generate_expression(ctx, node->data.variable.initializer);
        var_type = LLVMTypeOf(init_val);
    } else {
        // Default to i8* (char*)
        var_type = LLVMPointerType(LLVMInt8Type(), 0);
    }
    
    // Create an allocation for the variable
    LLVMValueRef var_alloca;
    
    // Global or local variable
    if (!ctx->current_function) {
        // Global variable
        var_alloca = LLVMAddGlobal(ctx->module, var_type, var_name);
        LLVMSetLinkage(var_alloca, LLVMExternalLinkage);
        
        // Initialize if needed
        if (node->data.variable.initializer) {
            LLVMValueRef init_val = llvm_generate_expression(ctx, node->data.variable.initializer);
            LLVMSetInitializer(var_alloca, init_val);
        } else {
            // Zero initialize
            LLVMSetInitializer(var_alloca, LLVMConstNull(var_type));
        }
    } else {
        // Local variable
        var_alloca = LLVMBuildAlloca(ctx->builder, var_type, var_name);
        
        // Initialize if needed
        if (node->data.variable.initializer) {
            LLVMValueRef init_val = llvm_generate_expression(ctx, node->data.variable.initializer);
            LLVMBuildStore(ctx->builder, init_val, var_alloca);
        }
    }
    
    // Add to value map
    llvm_store_value(ctx, var_name, var_alloca);
    
    // Add to symbol table
    add_symbol(ctx->symtab, (char*)var_name, SYMBOL_VARIABLE);
}

// Generate struct declaration
void llvm_generate_struct(LLVMGenContext* ctx, AST_Node* node) {
    // Get struct name
    const char* struct_name = node->data.struct_decl.name;
    
    // Count fields
    int field_count = 0;
    StructField* field = node->data.struct_decl.fields->head;
    
    while (field) {
        field_count++;
        field = field->next;
    }
    
    // Create field types array
    LLVMTypeRef* field_types = NULL;
    
    if (field_count > 0) {
        field_types = (LLVMTypeRef*)malloc(field_count * sizeof(LLVMTypeRef));
        if (!field_types) {
            fprintf(stderr, "Memory allocation failed\n");
            return;
        }
        
        // Fill field types
        field = node->data.struct_decl.fields->head;
        for (int i = 0; i < field_count; i++) {
            field_types[i] = llvm_get_type(ctx, field->type);
            field = field->next;
        }
    }
    
    // Create struct type
    LLVMTypeRef struct_type = LLVMStructCreateNamed(ctx->context, struct_name);
    LLVMStructSetBody(struct_type, field_types, field_count, 0);
    
    // Register struct in symbol table
    add_symbol(ctx->symtab, (char*)struct_name, SYMBOL_STRUCT);
    
    // Clean up
    if (field_types) free(field_types);
}