#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "expression.h"
#include "statement.h"
#include "declaration.h"
#include "anon_function.h"
#include "utils.h"

// Main code generation function
void generate_code(CodeGenContext* ctx, AST_Node* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_PROGRAM:
            generate_program(ctx, node);
            break;
        case NODE_FUNCTION:
            generate_function(ctx, node);
            break;
        case NODE_VARIABLE:
            generate_variable(ctx, node);
            break;
        case NODE_STRUCT:
            generate_struct(ctx, node);
            break;
        case NODE_TYPE_DECLARATION:
            generate_type_declaration(ctx, node);
            break;
        case NODE_IMPORT:
            generate_import(ctx, node);
            break;
        case NODE_IF:
            generate_if_statement(ctx, node);
            break;
        case NODE_MATCH:
            generate_match_statement(ctx, node);
            break;
        case NODE_RETURN:
            generate_return_statement(ctx, node);
            break;
        case NODE_COMPOUND_STATEMENT:
            generate_compound_statement(ctx, node);
            break;
        default:
            generate_expression(ctx, node);
            fprintf(ctx->output, ";\n");
            break;
    }
}
