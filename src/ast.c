#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// Helper function to allocate a new AST node
static AST_Node* allocate_node(NodeType type) {
    AST_Node* node = (AST_Node*)malloc(sizeof(AST_Node));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    node->type = type;
    node->next = NULL;
    return node;
}

// Create program node
AST_Node* create_program_node(AST_Node_List* declarations) {
    AST_Node* node = allocate_node(NODE_PROGRAM);
    node->data.program.declarations = declarations;
    return node;
}

// Create function declaration node
AST_Node* create_function_node(char* name, AST_Node_List* parameters, TypeInfo* return_type, GuardClause* guard, AST_Node* body) {
    AST_Node* node = allocate_node(NODE_FUNCTION);
    node->data.function.name = name;
    node->data.function.parameters = parameters;
    node->data.function.return_type = return_type;
    node->data.function.guard = guard;
    node->data.function.body = body;
    return node;
}

// Create variable declaration node
AST_Node* create_variable_node(VariableType var_type, char* name, TypeInfo* type, AST_Node* initializer) {
    AST_Node* node = allocate_node(NODE_VARIABLE);
    node->data.variable.var_type = var_type;
    node->data.variable.name = name;
    node->data.variable.type = type;
    node->data.variable.initializer = initializer;
    return node;
}

// Create struct declaration node
AST_Node* create_struct_node(char* name, AST_Node_List* composition, StructField_List* fields) {
    AST_Node* node = allocate_node(NODE_STRUCT);
    node->data.struct_decl.name = name;
    node->data.struct_decl.composition = composition;
    node->data.struct_decl.fields = fields;
    return node;
}

// Create type declaration node
AST_Node* create_type_declaration_node(char* name, TypeInfo* type) {
    AST_Node* node = allocate_node(NODE_TYPE_DECLARATION);
    node->data.type_decl.name = name;
    node->data.type_decl.type = type;
    return node;
}

// Create import node
AST_Node* create_import_node(char* filename) {
    AST_Node* node = allocate_node(NODE_IMPORT);
    node->data.import.filename = filename;
    return node;
}

// Create binary operation node
AST_Node* create_binary_op_node(BinaryOpType op, AST_Node* left, AST_Node* right) {
    AST_Node* node = allocate_node(NODE_BINARY_OP);
    node->data.binary_op.op = op;
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

// Create unary operation node
AST_Node* create_unary_op_node(UnaryOpType op, AST_Node* operand) {
    AST_Node* node = allocate_node(NODE_UNARY_OP);
    node->data.unary_op.op = op;
    node->data.unary_op.operand = operand;
    return node;
}

// Create if statement node
AST_Node* create_if_node(AST_Node* condition, AST_Node* true_branch, AST_Node* false_branch) {
    AST_Node* node = allocate_node(NODE_IF);
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.true_branch = true_branch;
    node->data.if_stmt.false_branch = false_branch;
    return node;
}

// Create match statement node
AST_Node* create_match_node(AST_Node* expression, MatchCase_List* cases) {
    AST_Node* node = allocate_node(NODE_MATCH);
    node->data.match_stmt.expression = expression;
    node->data.match_stmt.cases = cases;
    return node;
}

// Create return statement node
AST_Node* create_return_node(AST_Node* expression) {
    AST_Node* node = allocate_node(NODE_RETURN);
    node->data.return_stmt.expression = expression;
    return node;
}

// Create assignment node
AST_Node* create_assignment_node(char* name, AST_Node* value) {
    AST_Node* node = allocate_node(NODE_ASSIGNMENT);
    node->data.assignment.name = name;
    node->data.assignment.value = value;
    return node;
}

// Create function call node
AST_Node* create_function_call_node(char* name, ExpressionList* arguments) {
    AST_Node* node = allocate_node(NODE_FUNCTION_CALL);
    node->data.function_call.name = name;
    node->data.function_call.arguments = arguments;
    return node;
}

// Create compound statement node
AST_Node* create_compound_statement_node(AST_Node_List* statements) {
    AST_Node* node = allocate_node(NODE_COMPOUND_STATEMENT);
    node->data.compound_stmt.statements = statements;
    return node;
}

// Create integer literal node
AST_Node* create_int_literal_node(char* value) {
    AST_Node* node = allocate_node(NODE_LITERAL_INT);
    node->data.literal.value = value;
    return node;
}

// Create float literal node
AST_Node* create_float_literal_node(char* value) {
    AST_Node* node = allocate_node(NODE_LITERAL_FLOAT);
    node->data.literal.value = value;
    return node;
}

// Create string literal node
AST_Node* create_string_literal_node(char* value) {
    AST_Node* node = allocate_node(NODE_LITERAL_STRING);
    node->data.literal.value = value;
    return node;
}

// Create boolean literal node
AST_Node* create_bool_literal_node(char* value) {
    AST_Node* node = allocate_node(NODE_LITERAL_BOOL);
    node->data.literal.value = value;
    return node;
}

// Create array literal node
AST_Node* create_array_literal_node(ExpressionList* elements) {
    AST_Node* node = allocate_node(NODE_LITERAL_ARRAY);
    node->data.function_call.arguments = elements; // Reuse the arguments field
    return node;
}

// Create identifier node
AST_Node* create_identifier_node(char* name) {
    AST_Node* node = allocate_node(NODE_IDENTIFIER);
    node->data.identifier.name = name;
    return node;
}

// Create member access node
AST_Node* create_member_access_node(AST_Node* object, char* member) {
    AST_Node* node = allocate_node(NODE_MEMBER_ACCESS);
    node->data.member_access.object = object;
    node->data.member_access.member = member;
    return node;
}

// Create struct initialization node
AST_Node* create_struct_init_node(AST_Node_List* fields) {
    AST_Node* node = allocate_node(NODE_STRUCT_INIT);
    node->data.struct_init.fields = fields;
    return node;
}

// Create struct initialization field node
AST_Node* create_struct_init_field_node(char* name, AST_Node* value) {
    AST_Node* node = allocate_node(NODE_ASSIGNMENT); // Reuse assignment node
    node->data.assignment.name = name;
    node->data.assignment.value = value;
    return node;
}

// Create spread node
AST_Node* create_spread_node(AST_Node* expression) {
    AST_Node* node = allocate_node(NODE_SPREAD);
    node->data.spread.expression = expression;
    return node;
}

// Create pipe node
AST_Node* create_pipe_node(AST_Node* left, AST_Node* right) {
    AST_Node* node = allocate_node(NODE_PIPE);
    node->data.pipe.left = left;
    node->data.pipe.right = right;
    return node;
}

// Create parameter node
AST_Node* create_parameter_node(char* name, TypeInfo* type) {
    AST_Node* node = allocate_node(NODE_VARIABLE); // Reuse variable node
    node->data.variable.name = name;
    node->data.variable.type = type;
    node->data.variable.initializer = NULL;
    return node;
}

// Create binding pattern
AST_Node* create_binding_pattern(char* name, TypeInfo* type) {
    AST_Node* node = allocate_node(NODE_BINDING_PATTERN);
    node->data.binding.name = name;
    node->data.binding.type = type;
    return node;
}

// Create wildcard node
AST_Node* create_wildcard_node() {
    return allocate_node(NODE_WILDCARD);
}

// Create node list
AST_Node_List* create_node_list(AST_Node* node) {
    AST_Node_List* list = (AST_Node_List*)malloc(sizeof(AST_Node_List));
    if (!list) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    list->head = node;
    list->tail = node;
    list->next = NULL;
    return list;
}

// Append node to list
void append_node(AST_Node_List* list, AST_Node* node) {
    if (!list || !node) return;
    
    if (!list->head) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
}

// Create struct field
StructField* create_struct_field(char* name, TypeInfo* type) {
    StructField* field = (StructField*)malloc(sizeof(StructField));
    if (!field) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    field->name = name;
    field->type = type;
    field->next = NULL;
    return field;
}

// Create field list
StructField_List* create_field_list(StructField* field) {
    StructField_List* list = (StructField_List*)malloc(sizeof(StructField_List));
    if (!list) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    list->head = field;
    list->tail = field;
    return list;
}

// Append field to list
void append_field(StructField_List* list, StructField* field) {
    if (!list || !field) return;
    
    if (!list->head) {
        list->head = field;
        list->tail = field;
    } else {
        list->tail->next = field;
        list->tail = field;
    }
}

// Create type info
TypeInfo* create_type_info(char* name, TypeInfo* generic_type) {
    TypeInfo* type = (TypeInfo*)malloc(sizeof(TypeInfo));
    if (!type) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    type->name = name;
    type->generic_type = generic_type;
    return type;
}

// Create expression list
ExpressionList* create_expression_list(AST_Node* expr) {
    ExpressionList* list = (ExpressionList*)malloc(sizeof(ExpressionList));
    if (!list) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    list->expression = expr;
    list->next = NULL;
    return list;
}

// Append expression to list
void append_expression(ExpressionList* list, AST_Node* expr) {
    if (!list || !expr) return;
    
    if (!list->expression) {
        list->expression = expr;
    } else {
        ExpressionList* current = list;
        while (current->next) {
            current = current->next;
        }
        
        ExpressionList* new_item = create_expression_list(expr);
        current->next = new_item;
    }
}

// Create guard clause
GuardClause* create_guard_clause(AST_Node* condition) {
    GuardClause* guard = (GuardClause*)malloc(sizeof(GuardClause));
    if (!guard) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    guard->condition = condition;
    return guard;
}

// Create match case
MatchCase* create_match_case(AST_Node* pattern, AST_Node* guard, AST_Node* body) {
    MatchCase* match_case = (MatchCase*)malloc(sizeof(MatchCase));
    if (!match_case) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    match_case->pattern = pattern;
    match_case->guard = guard;
    match_case->body = body;
    match_case->next = NULL;
    return match_case;
}

// Create match case list
MatchCase_List* create_match_case_list(MatchCase* case_node) {
    MatchCase_List* list = (MatchCase_List*)malloc(sizeof(MatchCase_List));
    if (!list) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    list->head = case_node;
    list->tail = case_node;
    return list;
}

// Append match case to list
void append_match_case(MatchCase_List* list, MatchCase* case_node) {
    if (!list || !case_node) return;
    
    if (!list->head) {
        list->head = case_node;
        list->tail = case_node;
    } else {
        list->tail->next = case_node;
        list->tail = case_node;
    }
}

// Free AST
void free_ast(AST_Node* node) {
    if (!node) return;
    
    // Free next node first (depth-first traversal)
    if (node->next) {
        free_ast(node->next);
    }
    
    // Free node-specific data
    switch (node->type) {
        case NODE_PROGRAM:
            if (node->data.program.declarations) {
                free_ast(node->data.program.declarations->head);
                free(node->data.program.declarations);
            }
            break;
        case NODE_FUNCTION:
            if (node->data.function.name) free(node->data.function.name);
            if (node->data.function.parameters) {
                free_ast(node->data.function.parameters->head);
                free(node->data.function.parameters);
            }
            if (node->data.function.return_type) {
                if (node->data.function.return_type->generic_type)
                    free(node->data.function.return_type->generic_type);
                if (node->data.function.return_type->name)
                    free(node->data.function.return_type->name);
                free(node->data.function.return_type);
            }
            if (node->data.function.guard) free(node->data.function.guard);
            if (node->data.function.body) free_ast(node->data.function.body);
            break;
        case NODE_VARIABLE:
            if (node->data.variable.name) free(node->data.variable.name);
            if (node->data.variable.type) {
                if (node->data.variable.type->generic_type)
                    free(node->data.variable.type->generic_type);
                if (node->data.variable.type->name)
                    free(node->data.variable.type->name);
                free(node->data.variable.type);
            }
            if (node->data.variable.initializer) free_ast(node->data.variable.initializer);
            break;
        case NODE_STRUCT:
            if (node->data.struct_decl.name) free(node->data.struct_decl.name);
            if (node->data.struct_decl.composition) {
                free_ast(node->data.struct_decl.composition->head);
                free(node->data.struct_decl.composition);
            }
            // Free struct fields
            if (node->data.struct_decl.fields) {
                StructField* field = node->data.struct_decl.fields->head;
                while (field) {
                    StructField* next = field->next;
                    if (field->name) free(field->name);
                    if (field->type) {
                        if (field->type->generic_type)
                            free(field->type->generic_type);
                        if (field->type->name)
                            free(field->type->name);
                        free(field->type);
                    }
                    free(field);
                    field = next;
                }
                free(node->data.struct_decl.fields);
            }
            break;
        case NODE_TYPE_DECLARATION:
            if (node->data.type_decl.name) free(node->data.type_decl.name);
            if (node->data.type_decl.type) {
                if (node->data.type_decl.type->generic_type)
                    free(node->data.type_decl.type->generic_type);
                if (node->data.type_decl.type->name)
                    free(node->data.type_decl.type->name);
                free(node->data.type_decl.type);
            }
            break;
        case NODE_IMPORT:
            if (node->data.import.filename) free(node->data.import.filename);
            break;
        case NODE_BINARY_OP:
            if (node->data.binary_op.left) free_ast(node->data.binary_op.left);
            if (node->data.binary_op.right) free_ast(node->data.binary_op.right);
            break;
        case NODE_UNARY_OP:
            if (node->data.unary_op.operand) free_ast(node->data.unary_op.operand);
            break;
        case NODE_IF:
            if (node->data.if_stmt.condition) free_ast(node->data.if_stmt.condition);
            if (node->data.if_stmt.true_branch) free_ast(node->data.if_stmt.true_branch);
            if (node->data.if_stmt.false_branch) free_ast(node->data.if_stmt.false_branch);
            break;
        case NODE_MATCH:
            if (node->data.match_stmt.expression) free_ast(node->data.match_stmt.expression);
            // Free match cases
            if (node->data.match_stmt.cases) {
                MatchCase* match_case = node->data.match_stmt.cases->head;
                while (match_case) {
                    MatchCase* next = match_case->next;
                    if (match_case->pattern) free_ast(match_case->pattern);
                    if (match_case->guard) free_ast(match_case->guard);
                    if (match_case->body) free_ast(match_case->body);
                    free(match_case);
                    match_case = next;
                }
                free(node->data.match_stmt.cases);
            }
            break;
        case NODE_RETURN:
            if (node->data.return_stmt.expression) free_ast(node->data.return_stmt.expression);
            break;
        case NODE_ASSIGNMENT:
            if (node->data.assignment.name) free(node->data.assignment.name);
            if (node->data.assignment.value) free_ast(node->data.assignment.value);
            break;
        case NODE_FUNCTION_CALL:
            if (node->data.function_call.name) free(node->data.function_call.name);
            // Free arguments
            if (node->data.function_call.arguments) {
                ExpressionList* arg = node->data.function_call.arguments;
                while (arg) {
                    ExpressionList* next = arg->next;
                    if (arg->expression) free_ast(arg->expression);
                    free(arg);
                    arg = next;
                }
            }
            break;
        case NODE_COMPOUND_STATEMENT:
            if (node->data.compound_stmt.statements) {
                free_ast(node->data.compound_stmt.statements->head);
                free(node->data.compound_stmt.statements);
            }
            break;
        case NODE_LITERAL_INT:
        case NODE_LITERAL_FLOAT:
        case NODE_LITERAL_STRING:
        case NODE_LITERAL_BOOL:
            if (node->data.literal.value) free(node->data.literal.value);
            break;
        case NODE_LITERAL_ARRAY:
            // Free array elements (reused function_call.arguments)
            if (node->data.function_call.arguments) {
                ExpressionList* elem = node->data.function_call.arguments;
                while (elem) {
                    ExpressionList* next = elem->next;
                    if (elem->expression) free_ast(elem->expression);
                    free(elem);
                    elem = next;
                }
            }
            break;
        case NODE_IDENTIFIER:
            if (node->data.identifier.name) free(node->data.identifier.name);
            break;
        case NODE_MEMBER_ACCESS:
            if (node->data.member_access.object) free_ast(node->data.member_access.object);
            if (node->data.member_access.member) free(node->data.member_access.member);
            break;
        case NODE_STRUCT_INIT:
            if (node->data.struct_init.fields) {
                free_ast(node->data.struct_init.fields->head);
                free(node->data.struct_init.fields);
            }
            break;
        case NODE_SPREAD:
            if (node->data.spread.expression) free_ast(node->data.spread.expression);
            break;
        case NODE_PIPE:
            if (node->data.pipe.left) free_ast(node->data.pipe.left);
            if (node->data.pipe.right) free_ast(node->data.pipe.right);
            break;
        case NODE_BINDING_PATTERN:
            if (node->data.binding.name) free(node->data.binding.name);
            if (node->data.binding.type) {
                if (node->data.binding.type->generic_type)
                    free(node->data.binding.type->generic_type);
                if (node->data.binding.type->name)
                    free(node->data.binding.type->name);
                free(node->data.binding.type);
            }
            break;
        case NODE_WILDCARD:
            // Nothing to free
            break;
        case NODE_EXPRESSION:
            // Generic expression, no special handling needed
            break;
    }
    
    // Free the node itself
    free(node);
}