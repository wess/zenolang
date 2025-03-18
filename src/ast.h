#ifndef AST_H
#define AST_H

// Node types
typedef enum {
    NODE_PROGRAM,
    NODE_FUNCTION,
    NODE_VARIABLE,
    NODE_STRUCT,
    NODE_TYPE_DECLARATION,
    NODE_IMPORT,
    NODE_EXPRESSION,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_IF,
    NODE_MATCH,
    NODE_RETURN,
    NODE_ASSIGNMENT,
    NODE_FUNCTION_CALL,
    NODE_COMPOUND_STATEMENT,
    NODE_LITERAL_INT,
    NODE_LITERAL_FLOAT,
    NODE_LITERAL_STRING,
    NODE_LITERAL_BOOL,
    NODE_LITERAL_ARRAY,
    NODE_IDENTIFIER,
    NODE_MEMBER_ACCESS,
    NODE_STRUCT_INIT,
    NODE_SPREAD,
    NODE_PIPE,
    NODE_BINDING_PATTERN,
    NODE_WILDCARD,
    NODE_ANONYMOUS_FUNCTION,
    NODE_PROMISE_THEN,
    NODE_PROMISE_CATCH,
    NODE_PROMISE_FINALLY,
    NODE_PROMISE_ALL,
    NODE_AWAIT_EXPRESSION
} NodeType;

// Variable types
typedef enum {
    VAR_LET,
    VAR_CONST
} VariableType;

// Binary operation types
typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_CONCAT,
    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,
    OP_AND,
    OP_OR
} BinaryOpType;

// Unary operation types
typedef enum {
    OP_NOT,
    OP_NEG
} UnaryOpType;

// Forward declarations
typedef struct AST_Node AST_Node;
typedef struct AST_Node_List AST_Node_List;
typedef struct TypeInfo TypeInfo;
typedef struct StructField StructField;
typedef struct StructField_List StructField_List;
typedef struct ExpressionList ExpressionList;
typedef struct GuardClause GuardClause;
typedef struct MatchCase MatchCase;
typedef struct MatchCase_List MatchCase_List;

// Type information
struct TypeInfo {
    char* name;
    TypeInfo* generic_type; // For types like array<int>
};

// Struct field
struct StructField {
    char* name;
    TypeInfo* type;
    StructField* next;
};

// List of struct fields
struct StructField_List {
    StructField* head;
    StructField* tail;
};

// List of AST nodes
struct AST_Node_List {
    AST_Node* head;
    AST_Node* tail;
    AST_Node* next;
};

// List of expressions
struct ExpressionList {
    AST_Node* expression;
    ExpressionList* next;
};

// Guard clause for function declarations
struct GuardClause {
    AST_Node* condition;
};

// Match case for match statements
struct MatchCase {
    AST_Node* pattern;
    AST_Node* guard;
    AST_Node* body;
    MatchCase* next;
};

// List of match cases
struct MatchCase_List {
    MatchCase* head;
    MatchCase* tail;
};

// AST Node structure
struct AST_Node {
    NodeType type;
    AST_Node* next;

    union {
        // Program node
        struct {
            AST_Node_List* declarations;
        } program;

        // Function declaration
        struct {
            char* name;
            AST_Node_List* parameters;
            TypeInfo* return_type;
            GuardClause* guard;
            AST_Node* body;
            int is_async; // Flag to mark as async function
        } function;

        // Variable declaration
        struct {
            VariableType var_type;
            char* name;
            TypeInfo* type;
            AST_Node* initializer;
        } variable;

        // Struct declaration
        struct {
            char* name;
            AST_Node_List* composition;
            StructField_List* fields;
        } struct_decl;

        // Type declaration
        struct {
            char* name;
            TypeInfo* type;
        } type_decl;

        // Import declaration
        struct {
            char* filename;
        } import;

        // Binary operation
        struct {
            BinaryOpType op;
            AST_Node* left;
            AST_Node* right;
        } binary_op;

        // Unary operation
        struct {
            UnaryOpType op;
            AST_Node* operand;
        } unary_op;

        // If statement
        struct {
            AST_Node* condition;
            AST_Node* true_branch;
            AST_Node* false_branch;
        } if_stmt;

        // Match statement
        struct {
            AST_Node* expression;
            MatchCase_List* cases;
        } match_stmt;

        // Return statement
        struct {
            AST_Node* expression;
        } return_stmt;

        // Assignment
        struct {
            char* name;
            AST_Node* value;
        } assignment;

        // Function call
        struct {
            char* name;
            ExpressionList* arguments;
        } function_call;

        // Compound statement
        struct {
            AST_Node_List* statements;
        } compound_stmt;

        // Literals
        struct {
            char* value;
        } literal;

        // Identifier
        struct {
            char* name;
        } identifier;

        // Member access
        struct {
            AST_Node* object;
            char* member;
        } member_access;

        // Struct initialization
        struct {
            AST_Node_List* fields;
        } struct_init;

        // Spread operator
        struct {
            AST_Node* expression;
        } spread;

        // Pipe operator
        struct {
            AST_Node* left;
            AST_Node* right;
        } pipe;

        // Binding pattern (for match)
        struct {
            char* name;
            TypeInfo* type;
        } binding;
        
        // Anonymous function
        struct {
            AST_Node_List* parameters;
            TypeInfo* return_type;
            AST_Node* body;
        } anon_function;

        // Promise then
        struct {
            AST_Node* promise;
            AST_Node* handler;  // Anonymous function for then handler
        } promise_then;

        // Promise catch
        struct {
            AST_Node* promise;
            AST_Node* handler;  // Anonymous function for catch handler
        } promise_catch;

        // Promise finally
        struct {
            AST_Node* promise;
            AST_Node* handler;  // Anonymous function for finally handler
        } promise_finally;

        // Promise.all
        struct {
            ExpressionList* promises;  // List of promises
        } promise_all;

        // Await expression
        struct {
            AST_Node* promise;
        } await_expr;
    } data;
};

// AST creation functions
AST_Node* create_program_node(AST_Node_List* declarations);
AST_Node* create_function_node(char* name, AST_Node_List* parameters, TypeInfo* return_type, GuardClause* guard, AST_Node* body);
AST_Node* create_variable_node(VariableType var_type, char* name, TypeInfo* type, AST_Node* initializer);
AST_Node* create_struct_node(char* name, AST_Node_List* composition, StructField_List* fields);
AST_Node* create_type_declaration_node(char* name, TypeInfo* type);
AST_Node* create_import_node(char* filename);
AST_Node* create_binary_op_node(BinaryOpType op, AST_Node* left, AST_Node* right);
AST_Node* create_unary_op_node(UnaryOpType op, AST_Node* operand);
AST_Node* create_if_node(AST_Node* condition, AST_Node* true_branch, AST_Node* false_branch);
AST_Node* create_match_node(AST_Node* expression, MatchCase_List* cases);
AST_Node* create_return_node(AST_Node* expression);
AST_Node* create_assignment_node(char* name, AST_Node* value);
AST_Node* create_function_call_node(char* name, ExpressionList* arguments);
AST_Node* create_compound_statement_node(AST_Node_List* statements);
AST_Node* create_int_literal_node(char* value);
AST_Node* create_float_literal_node(char* value);
AST_Node* create_string_literal_node(char* value);
AST_Node* create_bool_literal_node(char* value);
AST_Node* create_array_literal_node(ExpressionList* elements);
AST_Node* create_identifier_node(char* name);
AST_Node* create_member_access_node(AST_Node* object, char* member);
AST_Node* create_struct_init_node(AST_Node_List* fields);
AST_Node* create_struct_init_field_node(char* name, AST_Node* value);
AST_Node* create_spread_node(AST_Node* expression);
AST_Node* create_pipe_node(AST_Node* left, AST_Node* right);
AST_Node* create_parameter_node(char* name, TypeInfo* type);
AST_Node* create_binding_pattern(char* name, TypeInfo* type);
AST_Node* create_wildcard_node();
AST_Node* create_anonymous_function_node(AST_Node_List* parameters, TypeInfo* return_type, AST_Node* body);

// Promise-related node creation
AST_Node* create_promise_then_node(AST_Node* promise, AST_Node* handler);
AST_Node* create_promise_catch_node(AST_Node* promise, AST_Node* handler);
AST_Node* create_promise_finally_node(AST_Node* promise, AST_Node* handler);
AST_Node* create_promise_all_node(ExpressionList* promises);
AST_Node* create_await_expression_node(AST_Node* promise);

// Helper functions
AST_Node_List* create_node_list(AST_Node* node);
void append_node(AST_Node_List* list, AST_Node* node);
StructField* create_struct_field(char* name, TypeInfo* type);
StructField_List* create_field_list(StructField* field);
void append_field(StructField_List* list, StructField* field);
TypeInfo* create_type_info(char* name, TypeInfo* generic_type);
ExpressionList* create_expression_list(AST_Node* expr);
void append_expression(ExpressionList* list, AST_Node* expr);
GuardClause* create_guard_clause(AST_Node* condition);
MatchCase* create_match_case(AST_Node* pattern, AST_Node* guard, AST_Node* body);
MatchCase_List* create_match_case_list(MatchCase* case_node);
void append_match_case(MatchCase_List* list, MatchCase* case_node);

// AST traversal and code generation declarations
void free_ast(AST_Node* node);

#endif // AST_H
