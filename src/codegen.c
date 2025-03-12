#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

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
    
    return ctx;
}

// Generate indentation
void indent(CodeGenContext* ctx) {
    for (int i = 0; i < ctx->indentation; i++) {
        fprintf(ctx->output, "    ");
    }
}

// Increase indentation level
void increase_indent(CodeGenContext* ctx) {
    ctx->indentation++;
}

// Decrease indentation level
void decrease_indent(CodeGenContext* ctx) {
    if (ctx->indentation > 0) {
        ctx->indentation--;
    }
}

// Get a new temporary variable name
char* get_temp_var_name(CodeGenContext* ctx) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "__temp_%d", ctx->temp_var_count++);
    return strdup(buffer);
}

// Get a new label name
char* get_label_name(CodeGenContext* ctx) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "__label_%d", ctx->label_count++);
    return strdup(buffer);
}

// Convert our type to C type
char* get_c_type(TypeInfo* type) {
    if (!type) return strdup("void");
    
    if (strcmp(type->name, "int") == 0) {
        return strdup("int");
    } else if (strcmp(type->name, "float") == 0) {
        return strdup("float");
    } else if (strcmp(type->name, "bool") == 0) {
        return strdup("int"); // C doesn't have bool, use int
    } else if (strcmp(type->name, "string") == 0) {
        return strdup("char*");
    } else if (strcmp(type->name, "void") == 0) {
        return strdup("void");
    } else if (strcmp(type->name, "array") == 0 && type->generic_type) {
        // For arrays, we need to create a pointer to the generic type
        char* inner_type = get_c_type(type->generic_type);
        char* array_type = (char*)malloc(strlen(inner_type) + 3);
        sprintf(array_type, "%s*", inner_type);
        free(inner_type);
        return array_type;
    } else {
        // For custom types, use struct prefix
        char* struct_type = (char*)malloc(strlen(type->name) + 8);
        sprintf(struct_type, "struct %s", type->name);
        return struct_type;
    }
}

// Function declarations
void generate_compound_statement_contents(CodeGenContext* ctx, AST_Node* node);
void generate_expression(CodeGenContext* ctx, AST_Node* node);

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

// Generate code for program node
void generate_program(CodeGenContext* ctx, AST_Node* node) {
    // Add standard includes
    fprintf(ctx->output, "#include <stdio.h>\n");
    fprintf(ctx->output, "#include <stdlib.h>\n");
    fprintf(ctx->output, "#include <string.h>\n");
    fprintf(ctx->output, "#include <stdbool.h>\n\n");
    
    // Generate code for all declarations
    AST_Node* declaration = node->data.program.declarations->head;
    while (declaration) {
        // Process this node but don't follow its next links
        generate_code(ctx, declaration);
        declaration = declaration->next;
    }
}

// Generate code for function declaration
void generate_function(CodeGenContext* ctx, AST_Node* node) {
    // Get return type
    char* return_type = get_c_type(node->data.function.return_type);
    
    // Start function declaration
    fprintf(ctx->output, "%s %s(", return_type, node->data.function.name);
    
    // Add function to symbol table
    add_symbol(ctx->symtab, node->data.function.name, SYMBOL_FUNCTION);
    
    // Create new scope for parameters
    enter_scope(ctx->symtab);
    
    // Generate parameter list
    AST_Node* param = node->data.function.parameters->head;
    int first_param = 1;
    
    while (param) {
        if (!first_param) {
            fprintf(ctx->output, ", ");
        }
        
        char* param_type = get_c_type(param->data.variable.type);
        fprintf(ctx->output, "%s %s", param_type, param->data.variable.name);
        
        // Add parameter to symbol table
        add_symbol(ctx->symtab, param->data.variable.name, SYMBOL_VARIABLE);
        
        free(param_type);
        first_param = 0;
        param = param->next;
    }
    
    fprintf(ctx->output, ") {\n");
    increase_indent(ctx);
    
    // Guard clause (if present)
    if (node->data.function.guard) {
        indent(ctx);
        fprintf(ctx->output, "// Guard clause\n");
        indent(ctx);
        fprintf(ctx->output, "if (!(");
        generate_expression(ctx, node->data.function.guard->condition);
        fprintf(ctx->output, ")) {\n");
        increase_indent(ctx);
        
        indent(ctx);
        fprintf(ctx->output, "fprintf(stderr, \"Guard condition failed for function %s\\n\");\n", 
                node->data.function.name);
        indent(ctx);
        
        if (strcmp(return_type, "void") != 0) {
            // For non-void functions, return a default value
            if (strcmp(return_type, "int") == 0 || strcmp(return_type, "bool") == 0) {
                fprintf(ctx->output, "return 0;\n");
            } else if (strcmp(return_type, "float") == 0) {
                fprintf(ctx->output, "return 0.0;\n");
            } else if (strcmp(return_type, "char*") == 0) {
                fprintf(ctx->output, "return NULL;\n");
            } else {
                // For custom types, return NULL or empty struct
                fprintf(ctx->output, "return (%s){0};\n", return_type);
            }
        } else {
            fprintf(ctx->output, "return;\n");
        }
        
        decrease_indent(ctx);
        indent(ctx);
        fprintf(ctx->output, "}\n\n");
    }
    
    // Generate function body
    if (node->data.function.body) {
        if (node->data.function.body->type == NODE_COMPOUND_STATEMENT) {
            // For compound statements, we generate the contents directly
            generate_compound_statement_contents(ctx, node->data.function.body);
        } else {
            // For single statements
            indent(ctx);
            generate_code(ctx, node->data.function.body);
        }
    }
    
    // Exit function scope
    leave_scope(ctx->symtab);
    
    decrease_indent(ctx);
    indent(ctx);
    fprintf(ctx->output, "}\n\n");
    
    free(return_type);
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

// Generate code for variable declaration
void generate_variable(CodeGenContext* ctx, AST_Node* node) {
    // Determine type
    char* var_type;
    if (node->data.variable.type) {
        var_type = get_c_type(node->data.variable.type);
    } else if (node->data.variable.initializer) {
        // Type inference (simplified)
        switch (node->data.variable.initializer->type) {
            case NODE_LITERAL_INT:
                var_type = strdup("int");
                break;
            case NODE_LITERAL_FLOAT:
                var_type = strdup("float");
                break;
            case NODE_LITERAL_STRING:
                var_type = strdup("char*");
                break;
            case NODE_LITERAL_BOOL:
                var_type = strdup("int");
                break;
            default:
                var_type = strdup("void*");
                break;
        }
    } else {
        var_type = strdup("void*");
    }
    
    // For const variables, add const qualifier
    if (node->data.variable.var_type == VAR_CONST) {
        char* const_type = (char*)malloc(strlen(var_type) + 7);
        sprintf(const_type, "const %s", var_type);
        free(var_type);
        var_type = const_type;
    }
    
    // Generate declaration
    fprintf(ctx->output, "%s %s", var_type, node->data.variable.name);
    
    // Add variable to symbol table
    add_symbol(ctx->symtab, node->data.variable.name, SYMBOL_VARIABLE);
    
    // Generate initializer if present
    if (node->data.variable.initializer) {
        fprintf(ctx->output, " = ");
        generate_expression(ctx, node->data.variable.initializer);
    }
    
    fprintf(ctx->output, ";\n");
    
    free(var_type);
}

// Generate code for struct declaration
void generate_struct(CodeGenContext* ctx, AST_Node* node) {
    // First handle composition (structs we inherit from)
    if (node->data.struct_decl.composition) {
        // In C, we'll implement composition by including all fields
        // from the composed structs in our new struct
        
        // We need to gather all fields from composed structs
        // For simplicity, we'll just print a comment about composition
        fprintf(ctx->output, "// Struct %s with composition: ", node->data.struct_decl.name);
        
        AST_Node* composed = node->data.struct_decl.composition->head;
        while (composed) {
            fprintf(ctx->output, "%s ", composed->data.identifier.name);
            composed = composed->next;
        }
        fprintf(ctx->output, "\n");
    }
    
    // Generate struct declaration
    fprintf(ctx->output, "struct %s {\n", node->data.struct_decl.name);
    
    // Add struct to symbol table
    add_symbol(ctx->symtab, node->data.struct_decl.name, SYMBOL_STRUCT);
    
    increase_indent(ctx);
    
    // Add fields from composition (would require symbol table lookup)
    // For now, we'll just add a comment and the fields manually
    if (node->data.struct_decl.composition) {
        indent(ctx);
        fprintf(ctx->output, "// Fields from composed structs\n");
        
        // Add Entity fields
        AST_Node* composed = node->data.struct_decl.composition->head;
        while (composed) {
            if (strcmp(composed->data.identifier.name, "Entity") == 0) {
                indent(ctx);
                fprintf(ctx->output, "char* id;\n");
                indent(ctx);
                fprintf(ctx->output, "char* name;\n");
            } else if (strcmp(composed->data.identifier.name, "Timestamps") == 0) {
                indent(ctx);
                fprintf(ctx->output, "int created_at;\n");
                indent(ctx);
                fprintf(ctx->output, "int updated_at;\n");
            }
            composed = composed->next;
        }
    }
    
    // Generate fields
    StructField* field = node->data.struct_decl.fields->head;
    while (field) {
        indent(ctx);
        
        char* field_type = get_c_type(field->type);
        fprintf(ctx->output, "%s %s;\n", field_type, field->name);
        
        free(field_type);
        field = field->next;
    }
    
    decrease_indent(ctx);
    fprintf(ctx->output, "};\n\n");
}

// Generate code for type declaration
void generate_type_declaration(CodeGenContext* ctx, AST_Node* node) {
    char* type_name = node->data.type_decl.name;
    char* base_type = get_c_type(node->data.type_decl.type);
    
    fprintf(ctx->output, "// Type alias: %s -> %s\n", type_name, base_type);
    fprintf(ctx->output, "typedef %s %s;\n\n", base_type, type_name);
    
    // Add type to symbol table
    add_symbol(ctx->symtab, type_name, SYMBOL_TYPE);
    
    free(base_type);
}

// Generate code for import
void generate_import(CodeGenContext* ctx, AST_Node* node) {
    // Trim quotes from filename
    char* filename = node->data.import.filename;
    char* trimmed = strdup(filename + 1);
    trimmed[strlen(trimmed) - 1] = '\0';
    
    fprintf(ctx->output, "#include %s\n", filename);
    
    free(trimmed);
}

// Generate code for expression
void generate_expression(CodeGenContext* ctx, AST_Node* node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_LITERAL_INT:
        case NODE_LITERAL_FLOAT:
        case NODE_LITERAL_BOOL:
            fprintf(ctx->output, "%s", node->data.literal.value);
            break;
            
        case NODE_LITERAL_STRING:
            fprintf(ctx->output, "%s", node->data.literal.value);
            break;
            
        case NODE_IDENTIFIER:
            fprintf(ctx->output, "%s", node->data.identifier.name);
            break;
            
        case NODE_BINARY_OP:
            fprintf(ctx->output, "(");
            generate_expression(ctx, node->data.binary_op.left);
            
            switch (node->data.binary_op.op) {
                case OP_ADD:
                    fprintf(ctx->output, " + ");
                    break;
                case OP_SUB:
                    fprintf(ctx->output, " - ");
                    break;
                case OP_MUL:
                    fprintf(ctx->output, " * ");
                    break;
                case OP_DIV:
                    fprintf(ctx->output, " / ");
                    break;
                case OP_MOD:
                    fprintf(ctx->output, " %% ");
                    break;
                case OP_CONCAT:
                    // String concatenation (we'll need to use strcat)
                    fprintf(ctx->output, " /* CONCAT */ ");
                    break;
                case OP_EQ:
                    fprintf(ctx->output, " == ");
                    break;
                case OP_NEQ:
                    fprintf(ctx->output, " != ");
                    break;
                case OP_LT:
                    fprintf(ctx->output, " < ");
                    break;
                case OP_GT:
                    fprintf(ctx->output, " > ");
                    break;
                case OP_LE:
                    fprintf(ctx->output, " <= ");
                    break;
                case OP_GE:
                    fprintf(ctx->output, " >= ");
                    break;
                case OP_AND:
                    fprintf(ctx->output, " && ");
                    break;
                case OP_OR:
                    fprintf(ctx->output, " || ");
                    break;
            }
            
            generate_expression(ctx, node->data.binary_op.right);
            fprintf(ctx->output, ")");
            break;
            
        case NODE_UNARY_OP:
            fprintf(ctx->output, "(");
            
            switch (node->data.unary_op.op) {
                case OP_NOT:
                    fprintf(ctx->output, "!");
                    break;
                case OP_NEG:
                    fprintf(ctx->output, "-");
                    break;
            }
            
            generate_expression(ctx, node->data.unary_op.operand);
            fprintf(ctx->output, ")");
            break;
            
        case NODE_FUNCTION_CALL:
            fprintf(ctx->output, "%s(", node->data.function_call.name);
            
            // Generate arguments
            ExpressionList* arg = node->data.function_call.arguments;
            int first_arg = 1;
            
            while (arg && arg->expression) {
                if (!first_arg) {
                    fprintf(ctx->output, ", ");
                }
                
                generate_expression(ctx, arg->expression);
                
                first_arg = 0;
                arg = arg->next;
            }
            
            fprintf(ctx->output, ")");
            break;
            
        case NODE_MEMBER_ACCESS:
            generate_expression(ctx, node->data.member_access.object);
            fprintf(ctx->output, ".%s", node->data.member_access.member);
            break;
            
        case NODE_STRUCT_INIT:
            // Generate struct initialization with proper formatting
            fprintf(ctx->output, "{ ");
            
            // Collect all fields and spread fields
            typedef struct FieldInfo {
                char* name;
                AST_Node* value;
                struct FieldInfo* next;
            } FieldInfo;
            
            FieldInfo* field_list = NULL;
            FieldInfo* field_list_tail = NULL;
            
            // Process all fields including spread fields
            AST_Node* field = node->data.struct_init.fields->head;
            while (field) {
                if (field->type == NODE_ASSIGNMENT) {
                    // Regular field - add directly to list
                    FieldInfo* info = (FieldInfo*)malloc(sizeof(FieldInfo));
                    info->name = strdup(field->data.assignment.name);
                    info->value = field->data.assignment.value;
                    info->next = NULL;
                    
                    if (field_list == NULL) {
                        field_list = info;
                        field_list_tail = info;
                    } else {
                        field_list_tail->next = info;
                        field_list_tail = info;
                    }
                } else if (field->type == NODE_SPREAD) {
                    // For spread fields, add all inner fields to our list
                    if (field->data.spread.expression->type == NODE_STRUCT_INIT) {
                        AST_Node* spread_field = field->data.spread.expression->data.struct_init.fields->head;
                        while (spread_field) {
                            if (spread_field->type == NODE_ASSIGNMENT) {
                                FieldInfo* info = (FieldInfo*)malloc(sizeof(FieldInfo));
                                info->name = strdup(spread_field->data.assignment.name);
                                info->value = spread_field->data.assignment.value;
                                info->next = NULL;
                                
                                if (field_list == NULL) {
                                    field_list = info;
                                    field_list_tail = info;
                                } else {
                                    field_list_tail->next = info;
                                    field_list_tail = info;
                                }
                            }
                            spread_field = spread_field->next;
                        }
                    }
                }
                field = field->next;
            }
            
            // Now generate all fields
            int first_field = 1;
            FieldInfo* current = field_list;
            while (current) {
                if (!first_field) {
                    fprintf(ctx->output, ", ");
                }
                
                fprintf(ctx->output, ".%s = ", current->name);
                generate_expression(ctx, current->value);
                
                first_field = 0;
                FieldInfo* temp = current;
                current = current->next;
                
                // Free the memory
                free(temp->name);
                free(temp);
            }
            
            fprintf(ctx->output, " }");
            break;
            
        case NODE_PIPE:
            // Pipe operator becomes function call with left expression as argument
            generate_expression(ctx, node->data.pipe.right);
            fprintf(ctx->output, "(");
            generate_expression(ctx, node->data.pipe.left);
            fprintf(ctx->output, ")");
            break;
            
        case NODE_ASSIGNMENT:
            fprintf(ctx->output, "%s = ", node->data.assignment.name);
            generate_expression(ctx, node->data.assignment.value);
            break;
            
        default:
            fprintf(ctx->output, "/* Unsupported expression */");
            break;
    }
}

// Generate code for if statement
void generate_if_statement(CodeGenContext* ctx, AST_Node* node) {
    fprintf(ctx->output, "if (");
    generate_expression(ctx, node->data.if_stmt.condition);
    fprintf(ctx->output, ") ");
    
    // Generate true branch
    if (node->data.if_stmt.true_branch->type == NODE_COMPOUND_STATEMENT) {
        generate_compound_statement(ctx, node->data.if_stmt.true_branch);
    } else {
        fprintf(ctx->output, "{\n");
        increase_indent(ctx);
        
        indent(ctx);
        generate_code(ctx, node->data.if_stmt.true_branch);
        
        decrease_indent(ctx);
        indent(ctx);
        fprintf(ctx->output, "}\n");
    }
    
    // Generate false branch if present
    if (node->data.if_stmt.false_branch) {
        indent(ctx);
        fprintf(ctx->output, "else ");
        
        if (node->data.if_stmt.false_branch->type == NODE_COMPOUND_STATEMENT) {
            generate_compound_statement(ctx, node->data.if_stmt.false_branch);
        } else {
            fprintf(ctx->output, "{\n");
            increase_indent(ctx);
            
            indent(ctx);
            generate_code(ctx, node->data.if_stmt.false_branch);
            
            decrease_indent(ctx);
            indent(ctx);
            fprintf(ctx->output, "}\n");
        }
    }
}

// Generate code for match statement
void generate_match_statement(CodeGenContext* ctx, AST_Node* node) {
    // In C, we'll translate match to a series of if-else statements
    char* expr_var = get_temp_var_name(ctx);
    
    fprintf(ctx->output, "{\n");
    increase_indent(ctx);
    
    // Store the match expression in a temporary variable
    indent(ctx);
    fprintf(ctx->output, "// Match statement\n");
    indent(ctx);
    fprintf(ctx->output, "int %s = ", expr_var);
    generate_expression(ctx, node->data.match_stmt.expression);
    fprintf(ctx->output, ";\n");
    
    // Generate if-else chain for each case
    MatchCase* match_case = node->data.match_stmt.cases->head;
    int first_case = 1;
    
    while (match_case) {
        indent(ctx);
        
        if (!first_case) {
            fprintf(ctx->output, "else ");
        }
        
        if (match_case->pattern->type == NODE_WILDCARD) {
            // Wildcard pattern always matches
            fprintf(ctx->output, "/* Default case */ ");
        } else {
            fprintf(ctx->output, "if (");
            
            if (match_case->pattern->type == NODE_BINDING_PATTERN) {
                // Binding pattern with type check
                fprintf(ctx->output, "/* Binding pattern: %s */ 1", 
                        match_case->pattern->data.binding.name);
                
                // Declare a variable for the binding
                indent(ctx);
                char* bind_type = get_c_type(match_case->pattern->data.binding.type);
                fprintf(ctx->output, "%s %s = %s;\n", bind_type, 
                        match_case->pattern->data.binding.name,
                        expr_var);
                free(bind_type);
            } else {
                // Regular pattern
                fprintf(ctx->output, "%s == ", expr_var);
                generate_expression(ctx, match_case->pattern);
            }
            
            // Add guard if present
            if (match_case->guard) {
                fprintf(ctx->output, " && (");
                generate_expression(ctx, match_case->guard);
                fprintf(ctx->output, ")");
            }
            
            fprintf(ctx->output, ") ");
        }
        
        // Generate case body
        if (match_case->body->type == NODE_COMPOUND_STATEMENT) {
            generate_compound_statement(ctx, match_case->body);
        } else {
            fprintf(ctx->output, "{\n");
            increase_indent(ctx);
            
            indent(ctx);
            generate_code(ctx, match_case->body);
            
            decrease_indent(ctx);
            indent(ctx);
            fprintf(ctx->output, "}\n");
        }
        
        first_case = 0;
        match_case = match_case->next;
    }
    
    decrease_indent(ctx);
    indent(ctx);
    fprintf(ctx->output, "}\n");
    
    free(expr_var);
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

// Generate code for compound statement
void generate_compound_statement(CodeGenContext* ctx, AST_Node* node) {
    fprintf(ctx->output, "{\n");
    
    // Generate the contents
    increase_indent(ctx);
    generate_compound_statement_contents(ctx, node);
    decrease_indent(ctx);
    
    indent(ctx);
    fprintf(ctx->output, "}\n");
}

// Clean up code generation context
void cleanup_codegen(CodeGenContext* ctx) {
    if (!ctx) return;
    
    cleanup_symbol_table(ctx->symtab);
    free(ctx);
}
