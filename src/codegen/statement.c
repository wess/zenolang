#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "statement.h"
#include "expression.h"
#include "declaration.h" // Added include for generate_variable_declaration
#include "utils.h"
#include "codegen.h"

// Forward declarations for loop generation functions
void generate_c_style_for_statement(CodeGenContext* ctx, AST_Node* node);
void generate_for_in_statement(CodeGenContext* ctx, AST_Node* node);
void generate_for_map_statement(CodeGenContext* ctx, AST_Node* node);
void generate_while_statement(CodeGenContext* ctx, AST_Node* node);

// Generate code for compound statement with braces
void generate_compound_statement(CodeGenContext* ctx, AST_Node* node) {
    fprintf(ctx->output, "{\n");
    
    increase_indent(ctx);
    generate_compound_statement_contents(ctx, node);
    decrease_indent(ctx);
    
    indent(ctx);
    fprintf(ctx->output, "}\n");
}

// Generate code for C-style for statement
void generate_c_style_for_statement(CodeGenContext* ctx, AST_Node* node) {
    // Create new scope for the loop initializer if it's a declaration
    int needs_scope = node->data.c_style_for.initializer && node->data.c_style_for.initializer->type == NODE_VARIABLE;
    if (needs_scope) {
        enter_scope(ctx->symtab);
    }

    fprintf(ctx->output, "for (");

    // Generate initializer
    if (node->data.c_style_for.initializer) {
        if (node->data.c_style_for.initializer->type == NODE_VARIABLE) {
            // Generate variable declaration without trailing semicolon or newline
            // NOTE: Assuming generate_variable handles context appropriately or needs adjustment
            generate_variable(ctx, node->data.c_style_for.initializer); // Changed function name
        } else {
            // Generate expression initializer
            generate_expression(ctx, node->data.c_style_for.initializer);
        }
    }
    fprintf(ctx->output, "; ");

    // Generate condition
    if (node->data.c_style_for.condition) {
        generate_expression(ctx, node->data.c_style_for.condition);
    }
    fprintf(ctx->output, "; ");

    // Generate incrementer
    if (node->data.c_style_for.incrementer) {
        generate_expression(ctx, node->data.c_style_for.incrementer);
    }
    fprintf(ctx->output, ") {\n");
    increase_indent(ctx);

    // Generate loop body
    AST_Node* body = node->data.c_style_for.body;
    if (body->type == NODE_COMPOUND_STATEMENT) {
        // Compound statement handles its own scope
        generate_compound_statement_contents(ctx, body);
    } else {
        // For single statements, generate directly
        indent(ctx);
        generate_code(ctx, body);
    }

    decrease_indent(ctx);
    indent(ctx);
    fprintf(ctx->output, "}\n");

    // End scope if one was created for the initializer
    if (needs_scope) {
        leave_scope(ctx->symtab);
    }
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

// Generate code for for..in statement (range/array)
void generate_for_in_statement(CodeGenContext* ctx, AST_Node* node) { // Renamed function
    AST_Node* iterable = node->data.for_in.iterable; // Use for_in struct
    AST_Node* body = node->data.for_in.body;
    AST_Node* var_node = node->data.for_in.variable; // This is a NODE_VARIABLE type node used as parameter
    char* var_name = var_node->data.variable.name;
    TypeInfo* var_type = var_node->data.variable.type; // Get type info (might be NULL)

    // Create new scope for loop variable
    enter_scope(ctx->symtab);

    // Add loop variable to symbol table for the scope
    // TODO: Determine actual type from iterable if var_type is NULL
    add_symbol(ctx->symtab, var_name, SYMBOL_VARIABLE, var_type ? var_type : create_type_info("int", NULL)); // Assume int if type unknown

    if (iterable->type == NODE_RANGE) {
        // Generate code for range start and end
        char start_var[64], end_var[64];
        snprintf(start_var, sizeof(start_var), "__range_start_%d", ctx->temp_var_count++);
        snprintf(end_var, sizeof(end_var), "__range_end_%d", ctx->temp_var_count++);

        indent(ctx);
        // Assuming range variables are integers for now
        fprintf(ctx->output, "int %s = ", start_var);
        generate_expression(ctx, iterable->data.range.start); // Use iterable
        fprintf(ctx->output, ";\n");

        indent(ctx);
        fprintf(ctx->output, "int %s = ", end_var);
        generate_expression(ctx, iterable->data.range.end); // Use iterable
        fprintf(ctx->output, ";\n");

        // Generate the C for loop (exclusive end '..')
        indent(ctx);
        // Declare loop variable with its type (assuming int for range)
        fprintf(ctx->output, "for (int %s = %s; %s < %s; %s++) {\n", var_name, start_var, var_name, end_var, var_name); // Use '<' for exclusive end
        increase_indent(ctx);

        // Generate loop body
        if (body->type == NODE_COMPOUND_STATEMENT) {
            // Compound statement handles its own scope internally, but the loop var is outer
            generate_compound_statement_contents(ctx, body);
        } else {
            // For single statements, generate directly within the loop scope
            indent(ctx);
            generate_code(ctx, body);
        }

        decrease_indent(ctx);
        indent(ctx);
        fprintf(ctx->output, "}\n");

    } else if (iterable->type == NODE_IDENTIFIER || iterable->type == NODE_LITERAL_ARRAY) {
        // Handle array iteration
        char index_var[64], length_var[64], array_var[64];
        snprintf(index_var, sizeof(index_var), "__i_%d", ctx->temp_var_count++);
        snprintf(length_var, sizeof(length_var), "__len_%d", ctx->temp_var_count++);
        snprintf(array_var, sizeof(array_var), "__arr_%d", ctx->temp_var_count++);

        indent(ctx);
        // Assuming array is accessible and has a 'length' property (requires runtime support)
        // This is a simplified C representation
        fprintf(ctx->output, "/* Array iteration requires runtime support for length and access */\n");
        indent(ctx);
        fprintf(ctx->output, "int %s = 0; // Placeholder length\n", length_var); // Placeholder
        indent(ctx);
        fprintf(ctx->output, "void* %s = ", array_var); // Placeholder array pointer
        generate_expression(ctx, iterable);
        fprintf(ctx->output, "; // Placeholder array\n");

        indent(ctx);
        // Determine variable type (e.g., char* for string array)
        const char* c_type = "void*"; // Default placeholder
        if (var_type && strcmp(var_type->name, "string") == 0) {
             c_type = "char*";
        } else if (var_type && strcmp(var_type->name, "int") == 0) {
             c_type = "int";
        } // Add more types as needed

        fprintf(ctx->output, "for (int %s = 0; %s < %s; %s++) {\n", index_var, index_var, length_var, index_var);
        increase_indent(ctx);
        indent(ctx);
        // Assign array element to loop variable (requires runtime function like array_get)
        fprintf(ctx->output, "%s %s = (%s)array_get(%s, %s); // Placeholder access\n", c_type, var_name, c_type, array_var, index_var);

        // Generate loop body
        if (body->type == NODE_COMPOUND_STATEMENT) {
            generate_compound_statement_contents(ctx, body);
        } else {
            indent(ctx);
            generate_code(ctx, body);
        }

        decrease_indent(ctx);
        indent(ctx);
        fprintf(ctx->output, "}\n");

    } else {
        // Error or unsupported type
        indent(ctx);
        fprintf(ctx->output, "// ERROR: Unsupported iterable type for 'for..in' loop\n");
    }

    // End scope for loop variable
    leave_scope(ctx->symtab);
}

// Generate code for for..in statement (map)
void generate_for_map_statement(CodeGenContext* ctx, AST_Node* node) { // Renamed function
    AST_Node* map_expr = node->data.for_map.map_expr; // Use for_map struct
    (void)map_expr; // Mark as unused for now (placeholder implementation)
    AST_Node* body = node->data.for_map.body;
    AST_Node* key_var_node = node->data.for_map.key_var;
    AST_Node* value_var_node = node->data.for_map.value_var;
    char* key_name = key_var_node->data.variable.name;
    char* value_name = value_var_node->data.variable.name;
    TypeInfo* key_type = key_var_node->data.variable.type; // Assume 'any' or determine from map_expr type
    TypeInfo* value_type = value_var_node->data.variable.type; // Assume 'any' or determine from map_expr type

    // Create new scope for loop variables
    enter_scope(ctx->symtab);

    // Add loop variables to symbol table
    // TODO: Determine actual types from map map_expr type if key/value types are NULL
    add_symbol(ctx->symtab, key_name, SYMBOL_VARIABLE, key_type ? key_type : create_type_info("string", NULL)); // Assume string key if unknown
    add_symbol(ctx->symtab, value_name, SYMBOL_VARIABLE, value_type ? value_type : create_type_info("any", NULL)); // Assume any value if unknown

    indent(ctx);
    fprintf(ctx->output, "// Map iteration requires runtime support (iterator, key/value access)\n");
    indent(ctx);
    // Placeholder loop - does not actually iterate map
    fprintf(ctx->output, "/* Placeholder map iteration */\n");
    indent(ctx);
    fprintf(ctx->output, "{\n"); // Create a block to contain placeholder vars
    increase_indent(ctx);

    // Declare placeholder key/value variables inside the loop scope
    // Determine C types based on TypeInfo
    const char* c_key_type = "char*"; // Default placeholder
    if (key_type && strcmp(key_type->name, "int") == 0) c_key_type = "int";
    // Add more key types

    const char* c_value_type = "void*"; // Default placeholder
    if (value_type && strcmp(value_type->name, "int") == 0) c_value_type = "int";
    else if (value_type && strcmp(value_type->name, "string") == 0) c_value_type = "char*";
    // Add more value types

    indent(ctx);
    fprintf(ctx->output, "%s %s = (%s)map_get_key_placeholder(); // Placeholder key\n", c_key_type, key_name, c_key_type);
    indent(ctx);
    fprintf(ctx->output, "%s %s = (%s)map_get_value_placeholder(); // Placeholder value\n", c_value_type, value_name, c_value_type);

    // Generate loop body
    if (body->type == NODE_COMPOUND_STATEMENT) {
        generate_compound_statement_contents(ctx, body);
    } else {
        indent(ctx);
        generate_code(ctx, body);
    }

    decrease_indent(ctx);
    indent(ctx);
    fprintf(ctx->output, "}\n");

    // End scope
    leave_scope(ctx->symtab);
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

// Generate code for while statement
void generate_while_statement(CodeGenContext* ctx, AST_Node* node) {
    fprintf(ctx->output, "while (");
    generate_expression(ctx, node->data.while_statement.condition); // Use while_statement struct
    fprintf(ctx->output, ") {\n");
    increase_indent(ctx);

    // Generate loop body
    AST_Node* body = node->data.while_statement.body; // Use while_statement struct
    if (body->type == NODE_COMPOUND_STATEMENT) {
        // Compound statement handles its own scope
        generate_compound_statement_contents(ctx, body);
    } else {
        // For single statements, generate directly
        indent(ctx);
        generate_code(ctx, body);
    }

    decrease_indent(ctx);
    indent(ctx);
    fprintf(ctx->output, "}\n");
}
