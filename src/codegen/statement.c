#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "statement.h"
#include "expression.h"
#include "utils.h"
#include "codegen.h"

// Generate code for compound statement with braces
void generate_compound_statement(CodeGenContext* ctx, AST_Node* node) {
    fprintf(ctx->output, "{\n");
    
    increase_indent(ctx);
    generate_compound_statement_contents(ctx, node);
    decrease_indent(ctx);
    
    indent(ctx);
    fprintf(ctx->output, "}\n");
}

// Generate contents of a compound statement without the braces
void generate_compound_statement_contents(CodeGenContext* ctx, AST_Node* node) {
    // Create new scope
    enter_scope(ctx->symtab);
    
    // Generate code for all statements
    AST_Node* stmt = node->data.compound_stmt.statements->head;
    while (stmt) {
        indent(ctx);
        
        if (stmt->type == NODE_COMPOUND_STATEMENT) {
            // If we have a nested compound statement, generate it directly
            generate_compound_statement(ctx, stmt);
        } else {
            // Otherwise, generate the statement
            generate_code(ctx, stmt);
        }
        
        // If this was a return statement, don't process any more statements
        if (stmt->type == NODE_RETURN) {
            break;
        }
        
        stmt = stmt->next;
    }
    
    // End scope
    leave_scope(ctx->symtab);
}

// Generate code for if statement
void generate_if_statement(CodeGenContext* ctx, AST_Node* node) {
    fprintf(ctx->output, "if (");
    generate_expression(ctx, node->data.if_stmt.condition);
    fprintf(ctx->output, ") {\n");
    increase_indent(ctx);
    
    // Generate true branch
    if (node->data.if_stmt.true_branch->type == NODE_COMPOUND_STATEMENT) {
        // For compound statements, we generate the contents directly
        generate_compound_statement_contents(ctx, node->data.if_stmt.true_branch);
    } else {
        // For single statements
        indent(ctx);
        generate_code(ctx, node->data.if_stmt.true_branch);
    }
    
    decrease_indent(ctx);
    indent(ctx);
    
    // Generate else branch if present
    if (node->data.if_stmt.false_branch) {
        fprintf(ctx->output, "} else {\n");
        increase_indent(ctx);
        
        if (node->data.if_stmt.false_branch->type == NODE_COMPOUND_STATEMENT) {
            // For compound statements, we generate the contents directly
            generate_compound_statement_contents(ctx, node->data.if_stmt.false_branch);
        } else {
            // For single statements
            indent(ctx);
            generate_code(ctx, node->data.if_stmt.false_branch);
        }
        
        decrease_indent(ctx);
        indent(ctx);
    }
    
    fprintf(ctx->output, "}\n");
}

// Generate code for match statement
void generate_match_statement(CodeGenContext* ctx, AST_Node* node) {
    // Get the expression to match
    fprintf(ctx->output, "{\n");
    increase_indent(ctx);
    
    // Create a temporary variable to store the match expression
    indent(ctx);
    fprintf(ctx->output, "// Match statement\n");
    
    // For boolean matches like in structs.zn example
    if (node->data.match_stmt.expression->type == NODE_IDENTIFIER) {
        MatchCase* current_case = node->data.match_stmt.cases->head;
        int first_case = 1;
        
        while (current_case) {
            indent(ctx);
            
            if (!first_case) {
                fprintf(ctx->output, "else ");
            }
            
            // Handle literal patterns (true/false)
            if (current_case->pattern->type == NODE_LITERAL_BOOL) {
                fprintf(ctx->output, "if (%s == %s) {\n", 
                        node->data.match_stmt.expression->data.identifier.name,
                        current_case->pattern->data.literal.value);
            } else {
                fprintf(ctx->output, "{\n"); // Default case
            }
            
            increase_indent(ctx);
            
            // Generate case body
            if (current_case->body->type == NODE_COMPOUND_STATEMENT) {
                generate_compound_statement_contents(ctx, current_case->body);
            } else {
                indent(ctx);
                generate_code(ctx, current_case->body);
            }
            
            decrease_indent(ctx);
            indent(ctx);
            fprintf(ctx->output, "}");
            
            if (first_case) {
                fprintf(ctx->output, "\n");
            }
            
            first_case = 0;
            current_case = current_case->next;
        }
    }
    
    decrease_indent(ctx);
    indent(ctx);
    fprintf(ctx->output, "}\n");
}

// Generate code for return statement
void generate_return_statement(CodeGenContext* ctx, AST_Node* node) {
    fprintf(ctx->output, "return");
    
    if (node->data.return_stmt.expression) {
        fprintf(ctx->output, " ");
        generate_expression(ctx, node->data.return_stmt.expression);
    }
    
    fprintf(ctx->output, ";\n");
}
