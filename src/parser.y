%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// External declarations
extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

// Function declarations
void yyerror(const char* s);
AST_Node* root = NULL;  // Root of the AST
%}

%union {
    char* str;
    AST_Node* node;
    AST_Node_List* node_list;
    TypeInfo* type_info;
    StructField* field;
    StructField_List* field_list;
    ExpressionList* expr_list;
    GuardClause* guard;
    MatchCase* match_case;
    MatchCase_List* match_case_list;
}

%token FN LET CONST STRUCT WITH WHERE IF ELSE MATCH RETURN IMPORT TYPE
%token EQ NEQ GE LE AND OR CONCAT ARROW PIPE SPREAD
%token ASYNC AWAIT THEN CATCH FINALLY
%token <str> IDENTIFIER TYPE_NAME PROMISE_TYPE
%token <str> INT_LITERAL FLOAT_LITERAL STRING_LITERAL BOOL_LITERAL

%type <node> program declaration function_declaration variable_declaration struct_declaration
%type <node> type_declaration statement expression function_call
%type <node> if_statement match_statement return_statement literal struct_instantiation
%type <node> compound_statement import_declaration struct_init_field
%type <node_list> declarations statements parameter_list struct_composition_list struct_init_list
%type <type_info> type type_with_generics
%type <str> function_name
%type <field_list> struct_fields
%type <field> struct_field
%type <expr_list> argument_list array_elements
%type <guard> guard_clause
%type <match_case_list> match_cases
%type <match_case> match_case

%start program

%%

program
    : declarations { root = create_program_node($1); }
    ;

declarations
    : declaration { $$ = create_node_list($1); }
    | declarations declaration { append_node($1, $2); $$ = $1; }
    ;

declaration
    : function_declaration { $$ = $1; }
    | variable_declaration { $$ = $1; }
    | struct_declaration { $$ = $1; }
    | type_declaration { $$ = $1; }
    | import_declaration { $$ = $1; }
    ;

import_declaration
    : IMPORT STRING_LITERAL ';' { $$ = create_import_node($2); }
    ;

type_declaration
    : TYPE IDENTIFIER '=' type ';' { $$ = create_type_declaration_node($2, $4); }
    ;

variable_declaration
    : LET IDENTIFIER '=' expression ';' { $$ = create_variable_node(VAR_LET, $2, NULL, $4); }
    | LET IDENTIFIER ':' type '=' expression ';' { $$ = create_variable_node(VAR_LET, $2, $4, $6); }
    | CONST IDENTIFIER '=' expression ';' { $$ = create_variable_node(VAR_CONST, $2, NULL, $4); }
    | CONST IDENTIFIER ':' type '=' expression ';' { $$ = create_variable_node(VAR_CONST, $2, $4, $6); }
    ;

struct_declaration
    : STRUCT IDENTIFIER '{' struct_fields '}' { $$ = create_struct_node($2, NULL, $4); }
    | STRUCT IDENTIFIER WITH struct_composition_list '{' struct_fields '}' { $$ = create_struct_node($2, $4, $6); }
    ;

struct_composition_list
    : IDENTIFIER { $$ = create_node_list(create_identifier_node($1)); }
    | struct_composition_list ',' IDENTIFIER { append_node($1, create_identifier_node($3)); $$ = $1; }
    ;

struct_fields
    : struct_field { $$ = create_field_list($1); }
    | struct_fields ',' struct_field { append_field($1, $3); $$ = $1; }
    ;

struct_field
    : IDENTIFIER ':' type { $$ = create_struct_field($1, $3); }
    ;

function_declaration
    : FN function_name '(' parameter_list ')' ':' type guard_clause compound_statement {
        $$ = create_function_node($2, $4, $7, $8, $9);
    }
    | FN function_name '(' parameter_list ')' ':' type ARROW expression ';' {
        AST_Node* return_stmt = create_return_node($9);
        AST_Node_List* body = create_node_list(return_stmt);
        $$ = create_function_node($2, $4, $7, NULL, create_compound_statement_node(body));
    }
    | ASYNC FN function_name '(' parameter_list ')' ':' type guard_clause compound_statement {
        // For async functions, we set a flag in the AST node
        AST_Node* func_node = create_function_node($3, $5, $8, $9, $10);
        func_node->data.function.is_async = 1; // Mark as async
        $$ = func_node;
    }
    | ASYNC FN function_name '(' parameter_list ')' ':' type ARROW expression ';' {
        AST_Node* return_stmt = create_return_node($10);
        AST_Node_List* body = create_node_list(return_stmt);
        AST_Node* func_node = create_function_node($3, $5, $8, NULL, create_compound_statement_node(body));
        func_node->data.function.is_async = 1; // Mark as async
        $$ = func_node;
    }
    ;

guard_clause
    : /* empty */ { $$ = NULL; }
    | WHERE expression { $$ = create_guard_clause($2); }
    ;

function_name
    : IDENTIFIER { $$ = $1; }
    ;

parameter_list
    : /* empty */ { $$ = create_node_list(NULL); }
    | IDENTIFIER ':' type { $$ = create_node_list(create_parameter_node($1, $3)); }
    | parameter_list ',' IDENTIFIER ':' type { 
        append_node($1, create_parameter_node($3, $5)); 
        $$ = $1; 
    }
    ;

type
    : TYPE_NAME { $$ = create_type_info($1, NULL); }
    | IDENTIFIER { $$ = create_type_info($1, NULL); }
    | type_with_generics { $$ = $1; }
    ;

type_with_generics
    : TYPE_NAME '<' type '>' { $$ = create_type_info($1, $3); }
    | IDENTIFIER '<' type '>' { $$ = create_type_info($1, $3); }
    ;

compound_statement
    : '{' statements '}' { $$ = create_compound_statement_node($2); }
    ;

statements
    : /* empty */ { $$ = create_node_list(NULL); }
    | statements statement { append_node($1, $2); $$ = $1; }
    ;

statement
    : variable_declaration { $$ = $1; }
    | expression ';' { $$ = $1; }
    | if_statement { $$ = $1; }
    | match_statement { $$ = $1; }
    | return_statement { $$ = $1; }
    | compound_statement { $$ = $1; }
    ;

if_statement
    : IF '(' expression ')' statement { $$ = create_if_node($3, $5, NULL); }
    | IF '(' expression ')' statement ELSE statement { $$ = create_if_node($3, $5, $7); }
    ;

match_statement
    : MATCH expression '{' match_cases '}' { $$ = create_match_node($2, $4); }
    ;

match_cases
    : match_case { $$ = create_match_case_list($1); }
    | match_cases match_case { append_match_case($1, $2); $$ = $1; }
    ;

match_case
    : expression ARROW statement { $$ = create_match_case($1, NULL, $3); }
    | IDENTIFIER ':' type ARROW statement { 
        AST_Node* binding = create_binding_pattern($1, $3);
        $$ = create_match_case(binding, NULL, $5); 
    }
    | expression IF expression ARROW statement { $$ = create_match_case($1, $3, $5); }
    | '_' ARROW statement { $$ = create_match_case(create_wildcard_node(), NULL, $3); }
    ;

return_statement
    : RETURN expression ';' { $$ = create_return_node($2); }
    | RETURN ';' { $$ = create_return_node(NULL); }
    ;

expression
    : literal { $$ = $1; }
    | IDENTIFIER { $$ = create_identifier_node($1); }
    | function_call { $$ = $1; }
    | struct_instantiation { $$ = $1; }
    | expression PIPE expression { $$ = create_pipe_node($1, $3); }
    | '(' expression ')' { $$ = $2; }
    | '(' parameter_list ')' ':' type ARROW expression { 
        AST_Node* return_stmt = create_return_node($7);
        AST_Node_List* body = create_node_list(return_stmt);
        $$ = create_anonymous_function_node($2, $5, create_compound_statement_node(body));
    }
    | '(' IDENTIFIER ')' ARROW expression { 
        // Arrow function with single parameter, no type
        AST_Node_List* params = create_node_list(create_parameter_node($2, create_type_info("any", NULL)));
        AST_Node* return_stmt = create_return_node($5);
        AST_Node_List* body = create_node_list(return_stmt);
        $$ = create_anonymous_function_node(params, create_type_info("any", NULL), create_compound_statement_node(body));
    }
    | '(' ')' ARROW expression {
        // Arrow function with no parameters - create an empty parameter list, not NULL
        AST_Node_List* params = create_node_list(NULL);
        AST_Node* return_stmt = create_return_node($4);
        AST_Node_List* body = create_node_list(return_stmt);
        $$ = create_anonymous_function_node(params, create_type_info("any", NULL), create_compound_statement_node(body));
    }
    | IDENTIFIER ARROW expression { 
        // Arrow function with single parameter, no parentheses, no type
        AST_Node_List* params = create_node_list(create_parameter_node($1, create_type_info("any", NULL)));
        AST_Node* return_stmt = create_return_node($3);
        AST_Node_List* body = create_node_list(return_stmt);
        $$ = create_anonymous_function_node(params, create_type_info("any", NULL), create_compound_statement_node(body));
    }
    | AWAIT expression { $$ = create_await_expression_node($2); }
    | PROMISE_TYPE '.' IDENTIFIER '(' argument_list ')' {
        if (strcmp($3, "all") == 0) {
            $$ = create_promise_all_node($5);
        } else {
            yyerror("Invalid Promise static method");
            YYERROR;
        }
    }
    | expression '.' THEN '(' expression ')' {
        $$ = create_promise_then_node($1, $5);
    }
    | expression '.' CATCH '(' expression ')' {
        $$ = create_promise_catch_node($1, $5);
    }
    | expression '.' FINALLY '(' expression ')' {
        $$ = create_promise_finally_node($1, $5);
    }
    | expression '+' expression { $$ = create_binary_op_node(OP_ADD, $1, $3); }
    | expression '-' expression { $$ = create_binary_op_node(OP_SUB, $1, $3); }
    | expression '*' expression { $$ = create_binary_op_node(OP_MUL, $1, $3); }
    | expression '/' expression { $$ = create_binary_op_node(OP_DIV, $1, $3); }
    | expression '%' expression { $$ = create_binary_op_node(OP_MOD, $1, $3); }
    | expression CONCAT expression { $$ = create_binary_op_node(OP_CONCAT, $1, $3); }
    | expression EQ expression { $$ = create_binary_op_node(OP_EQ, $1, $3); }
    | expression NEQ expression { $$ = create_binary_op_node(OP_NEQ, $1, $3); }
    | expression '<' expression { $$ = create_binary_op_node(OP_LT, $1, $3); }
    | expression '>' expression { $$ = create_binary_op_node(OP_GT, $1, $3); }
    | expression LE expression { $$ = create_binary_op_node(OP_LE, $1, $3); }
    | expression GE expression { $$ = create_binary_op_node(OP_GE, $1, $3); }
    | expression AND expression { $$ = create_binary_op_node(OP_AND, $1, $3); }
    | expression OR expression { $$ = create_binary_op_node(OP_OR, $1, $3); }
    | '!' expression { $$ = create_unary_op_node(OP_NOT, $2); }
    | '-' expression { $$ = create_unary_op_node(OP_NEG, $2); }
    | IDENTIFIER '=' expression { $$ = create_assignment_node($1, $3); }
    | expression '.' IDENTIFIER { $$ = create_member_access_node($1, $3); }
    | SPREAD expression { $$ = create_spread_node($2); }
    ;

struct_instantiation
    : '{' struct_init_list '}' { $$ = create_struct_init_node($2); }
    ;

struct_init_list
    : struct_init_field { $$ = create_node_list($1); }
    | struct_init_list ',' struct_init_field { append_node($1, $3); $$ = $1; }
    ;

struct_init_field
    : IDENTIFIER ':' expression { $$ = create_struct_init_field_node($1, $3); }
    | SPREAD expression { $$ = create_spread_node($2); }
    ;

function_call
    : IDENTIFIER '(' argument_list ')' { $$ = create_function_call_node($1, $3); }
    ;

argument_list
    : /* empty */ { $$ = create_expression_list(NULL); }
    | expression { $$ = create_expression_list($1); }
    | argument_list ',' expression { append_expression($1, $3); $$ = $1; }
    ;

literal
    : INT_LITERAL { $$ = create_int_literal_node($1); }
    | FLOAT_LITERAL { $$ = create_float_literal_node($1); }
    | STRING_LITERAL { $$ = create_string_literal_node($1); }
    | BOOL_LITERAL { $$ = create_bool_literal_node($1); }
    | '[' array_elements ']' { $$ = create_array_literal_node($2); }
    ;

array_elements
    : /* empty */ { $$ = create_expression_list(NULL); }
    | expression { $$ = create_expression_list($1); }
    | array_elements ',' expression { append_expression($1, $3); $$ = $1; }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Parser error at line %d: %s near '%s'\n", yylineno, s, yytext);
    exit(1);
}
