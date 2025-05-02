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

%token FN LET CONST STRUCT WITH WHERE IF ELSE MATCH RETURN IMPORT TYPE WHILE
%token EQ NEQ GE LE AND OR CONCAT ARROW PIPE SPREAD RANGE
%token ASYNC AWAIT THEN CATCH FINALLY
%token FOR IN PRINT /* Removed FOREACH */
%token <str> IDENTIFIER TYPE_NAME PROMISE_TYPE
%token <str> INT_LITERAL FLOAT_LITERAL STRING_LITERAL BOOL_LITERAL

%type <node> program declaration function_declaration variable_declaration_body struct_declaration /* variable_declaration type removed */
%type <node> type_declaration statement expression function_call simple_statement control_statement return_statement_body /* Added categories */
%type <node> if_statement match_statement /* return_statement removed */ literal struct_instantiation
%type <node> compound_statement import_declaration struct_init_field
%type <node> c_style_for_statement for_initializer expression_opt for_statement foreach_statement range_expression map_literal map_entry while_statement
%type <node_list> map_entries /* Changed from expr_list to node_list */
%type <node_list> declarations statements parameter_list struct_composition_list struct_init_list
%type <type_info> type type_with_generics
%type <str> function_name
%type <field_list> struct_fields
%type <field> struct_field
%type <expr_list> argument_list array_elements
%type <guard> guard_clause
%type <match_case_list> match_cases
%type <match_case> match_case

// Precedence and Associativity Rules (Lowest to Highest)
%right '=' // Assignment
%left OR // Logical OR
%left AND // Logical AND
%left EQ NEQ // Equality
%left '<' '>' LE GE // Comparison
%left '+' '-' CONCAT // Addition, Subtraction, Concatenation
%left '*' '/' '%' // Multiplication, Division, Modulo
%right '!' // Logical NOT (Unary)
%right UMINUS // Unary minus (defined below)
%left '.' // Member access
%left '(' ')' '[' ']' // Function call, array access (high precedence)
%left PIPE // Pipe operator
%left ARROW // Arrow function / Match case arrow
%nonassoc IF // Non-associative IF to prevent dangling else
%nonassoc ELSE // Non-associative ELSE
%nonassoc WHILE // Added precedence for WHILE back
%nonassoc FOR   // Added precedence for FOR

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
    | variable_declaration_body ';' { $$ = $1; } /* Use body + semicolon */
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

/* variable_declaration rule definition removed */

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
    | TYPE_NAME '<' type ',' type '>' {
        // Check if the type name is "map"
        if (strcmp($1, "map") == 0) {
            $$ = create_map_type_info($3, $5); // Specific function for map types
        } else {
            char err_msg[100];
            snprintf(err_msg, sizeof(err_msg), "Type '%s' does not support two generic arguments", $1);
            yyerror(err_msg);
            YYERROR; // Trigger parser error recovery
        }
    }
    ;

compound_statement
    : '{' statements '}' { $$ = create_compound_statement_node($2); }
    ;

statements
    : /* empty */ { $$ = create_node_list(NULL); }
    | statements statement { append_node($1, $2); $$ = $1; }
    ;

statement
    : simple_statement    { $$ = $1; }
    | control_statement   { $$ = $1; }
    ;

/* Statements that must end with a semicolon */
simple_statement
    : variable_declaration_body ';' { $$ = $1; } /* Use body + semicolon */
    | return_statement_body ';' { $$ = $1; }
    | expression ';'       { $$ = $1; }
    ;

/* Control flow and block statements */
control_statement
    : if_statement          { $$ = $1; }
    | while_statement       { $$ = $1; }
    | c_style_for_statement { $$ = $1; }
    | for_statement         { $$ = $1; } /* For range/array */
    | foreach_statement     { $$ = $1; } /* For map */
    | match_statement       { $$ = $1; }
    | compound_statement    { $$ = $1; }
    ;

/* Reverted C-style for loop rule (again) */
c_style_for_statement
    : FOR '(' for_initializer expression_opt ';' expression_opt ')' statement {
        /* Note: $3 (for_initializer) now includes the semicolon if it's a var_decl or expr */
        $$ = create_c_style_for_node($3, $4, $6, $8);
    }
    ;

/* Initializer for C-style for loop */
for_initializer
    : variable_declaration_body ';' { $$ = $1; } /* Use body + semicolon */
    | expression_opt ';'            { $$ = $1; }
    ;

/* Optional expression (used for condition and incrementer) */
expression_opt
    : expression           { $$ = $1; }
    | /* empty */          { $$ = NULL; }
    ;

if_statement
    : IF '(' expression ')' statement %prec IF { $$ = create_if_node($3, $5, NULL); } // Apply IF precedence
    | IF '(' expression ')' statement ELSE statement { $$ = create_if_node($3, $5, $7); } // ELSE has higher precedence implicitly
    ;

while_statement
    : WHILE '(' expression ')' statement { $$ = create_while_node($3, $5); }
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

/* Renamed from return_statement, removed trailing ';' */
return_statement_body
    : RETURN expression { $$ = create_return_node($2); }
    | RETURN            { $$ = create_return_node(NULL); }
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
    | '-' expression %prec UMINUS { $$ = create_unary_op_node(OP_NEG, $2); } // Apply UMINUS precedence
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
    | PRINT '(' argument_list ')' { $$ = create_function_call_node("print", $3); }
    ;

argument_list
    : /* empty */ { $$ = create_expression_list(NULL); }
    | expression { $$ = create_expression_list($1); }
    | argument_list ',' expression { append_expression($1, $3); $$ = $1; }
    ;

/* Renamed to for_in_statement internally, handles range/array iteration */
for_statement
    : FOR '(' IDENTIFIER IN expression ')' statement { /* Optional type */
        AST_Node* var_decl = create_parameter_node($3, NULL); /* Type is NULL initially */
        $$ = create_for_in_node(var_decl, $5, $7);
    }
    | FOR '(' IDENTIFIER ':' type IN expression ')' statement { /* Explicit type */
        AST_Node* var_decl = create_parameter_node($3, $5);
        $$ = create_for_in_node(var_decl, $7, $9);
    }
    | FOR '(' IDENTIFIER IN range_expression ')' statement { /* Optional type for range */
        AST_Node* var_decl = create_parameter_node($3, NULL);
        $$ = create_for_in_node(var_decl, $5, $7);
    }
    | FOR '(' IDENTIFIER ':' type IN range_expression ')' statement { /* Explicit type for range */
        AST_Node* var_decl = create_parameter_node($3, $5);
        $$ = create_for_in_node(var_decl, $7, $9);
    }
    ;

/* Renamed to for_map_statement internally, handles map iteration */
foreach_statement
    : FOR '(' IDENTIFIER ',' IDENTIFIER IN expression ')' statement { /* Changed FOREACH to FOR */
        AST_Node* key_var = create_parameter_node($3, NULL); /* Type is NULL initially */
        AST_Node* value_var = create_parameter_node($5, NULL); /* Type is NULL initially */
        $$ = create_for_map_node(key_var, value_var, $7, $9);
    }
    ;

range_expression
    : expression RANGE expression { $$ = create_range_node($1, $3); }
    ;

literal
    : INT_LITERAL { $$ = create_int_literal_node($1); }
    | FLOAT_LITERAL { $$ = create_float_literal_node($1); }
    | STRING_LITERAL { $$ = create_string_literal_node($1); }
    | BOOL_LITERAL { $$ = create_bool_literal_node($1); }
    | '[' array_elements ']' { $$ = create_array_literal_node($2); }
    | map_literal { $$ = $1; }
    ;

map_literal
    : '{' map_entries '}' { $$ = create_map_literal_node($2); } /* $2 is now node_list */
    ;

map_entries
    : map_entry { $$ = create_node_list($1); } /* Changed create_expression_list to create_node_list */
    | map_entries ',' map_entry { append_node($1, $3); $$ = $1; } /* Changed append_expression to append_node */
    /* | / * empty * / { $$ = create_node_list(NULL); } */  /* Disallow completely empty map {} to resolve r/r conflict with empty block */
    ;

map_entry
    : expression ':' expression { /* Changed key from STRING_LITERAL to expression */
        $$ = create_map_entry_node($1, $3);
    }
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
