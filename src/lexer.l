%{
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

void yyerror(const char* s);
%}

%option noyywrap
%option yylineno

%%

[ \t\n]+                  { /* Skip whitespace */ }
"//".*                    { /* Skip single-line comments */ }

"/*"                      { /* Skip multi-line comments */
                            int c;
                            while((c = input()) != 0) {
                                if(c == '*') {
                                    if((c = input()) == '/')
                                        break;
                                    else
                                        unput(c);
                                }
                            }
                          }

"fn"                      { return FN; }
"let"                     { return LET; }
"const"                   { return CONST; }
"struct"                  { return STRUCT; }
"with"                    { return WITH; }
"where"                   { return WHERE; }
"if"                      { return IF; }
"else"                    { return ELSE; }
"match"                   { return MATCH; }
"return"                  { return RETURN; }
"import"                  { return IMPORT; }
"type"                    { return TYPE; }
"while"                   { return WHILE; }
"async"                   { return ASYNC; }
"await"                   { return AWAIT; }
"then"                    { return THEN; }
"catch"                   { return CATCH; }
"finally"                 { return FINALLY; }
"Promise"                 { yylval.str = strdup(yytext); return PROMISE_TYPE; }
"for"                     { return FOR; }
"in"                      { return IN; }
"print"                   { return PRINT; }

"int"                     { yylval.str = strdup(yytext); return TYPE_NAME; }
"float"                   { yylval.str = strdup(yytext); return TYPE_NAME; }
"bool"                    { yylval.str = strdup(yytext); return TYPE_NAME; }
"string"                  { yylval.str = strdup(yytext); return TYPE_NAME; }
"array"                   { yylval.str = strdup(yytext); return TYPE_NAME; }
"map"                     { yylval.str = strdup(yytext); return TYPE_NAME; }
"any"                     { yylval.str = strdup(yytext); return TYPE_NAME; }

"true"|"false"            { yylval.str = strdup(yytext); return BOOL_LITERAL; }

[0-9]+                    { yylval.str = strdup(yytext); return INT_LITERAL; }
[0-9]+"."[0-9]+           { yylval.str = strdup(yytext); return FLOAT_LITERAL; }
\"(\\.|[^"\\])*\"         { yylval.str = strdup(yytext); return STRING_LITERAL; }

"=="                      { return EQ; }
"!="                      { return NEQ; }
">="                      { return GE; }
"<="                      { return LE; }
"&&"                      { return AND; }
"||"                      { return OR; }
"++"                      { return CONCAT; }
"=>"                      { return ARROW; }
"|>"                      { return PIPE; }
"..."                     { return SPREAD; }
".."                      { return RANGE; }

[a-zA-Z_][a-zA-Z0-9_]*    { yylval.str = strdup(yytext); return IDENTIFIER; }

"("                       { return '('; }
")"                       { return ')'; }
"{"                       { return '{'; }
"}"                       { return '}'; }
"["                       { return '['; }
"]"                       { return ']'; }
"<"                       { return '<'; }
">"                       { return '>'; }
";"                       { return ';'; }
":"                       { return ':'; }
","                       { return ','; }
"."                       { return '.'; }
"="                       { return '='; }
"+"                       { return '+'; }
"-"                       { return '-'; }
"*"                       { return '*'; }
"/"                       { return '/'; }
"%"                       { return '%'; }
"!"                       { return '!'; }
"_"                       { return '_'; }

.                         { yyerror("Unexpected character"); }

%%
