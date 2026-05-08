%{
/*
 * Module 1 — Standalone Lexer
 * Prints each token to stdout for testing/screenshots.
 * Build: flex lexer_standalone.l && gcc -o lexer_only lex.yy.c -lfl
 * Run:   ./lexer_only < input.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int line_num = 1;
%}

%option noyywrap

DIGIT       [0-9]
LETTER      [a-zA-Z_]
INTEGER     {DIGIT}+
FLOAT       {DIGIT}+\.{DIGIT}+
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*

%%

"int"       { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"float"     { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"if"        { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"else"      { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"while"     { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"return"    { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"function"  { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"procedure" { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"begin"     { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"end"       { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"then"      { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"log"       { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }
"exp"       { printf("[Line %2d]  KEYWORD       : %s\n", line_num, yytext); }

{FLOAT}     { printf("[Line %2d]  FLOAT_LIT     : %s\n", line_num, yytext); }
{INTEGER}   { printf("[Line %2d]  INT_LIT       : %s\n", line_num, yytext); }
{IDENTIFIER}{ printf("[Line %2d]  IDENTIFIER    : %s\n", line_num, yytext); }

"+"  { printf("[Line %2d]  OP_PLUS       : +\n",  line_num); }
"-"  { printf("[Line %2d]  OP_MINUS      : -\n",  line_num); }
"*"  { printf("[Line %2d]  OP_MUL        : *\n",  line_num); }
"/"  { printf("[Line %2d]  OP_DIV        : /\n",  line_num); }
"^"  { printf("[Line %2d]  OP_POW        : ^\n",  line_num); }
"==" { printf("[Line %2d]  OP_EQ         : ==\n", line_num); }
"!=" { printf("[Line %2d]  OP_NEQ        : !=\n", line_num); }
"<=" { printf("[Line %2d]  OP_LE         : <=\n", line_num); }
">=" { printf("[Line %2d]  OP_GE         : >=\n", line_num); }
"<"  { printf("[Line %2d]  OP_LT         : <\n",  line_num); }
">"  { printf("[Line %2d]  OP_GT         : >\n",  line_num); }
"="  { printf("[Line %2d]  OP_ASSIGN     : =\n",  line_num); }

"("  { printf("[Line %2d]  LPAREN        : (\n",  line_num); }
")"  { printf("[Line %2d]  RPAREN        : )\n",  line_num); }
"{"  { printf("[Line %2d]  LBRACE        : {\n",  line_num); }
"}"  { printf("[Line %2d]  RBRACE        : }\n",  line_num); }
";"  { printf("[Line %2d]  SEMICOLON     : ;\n",  line_num); }
","  { printf("[Line %2d]  COMMA         : ,\n",  line_num); }

"//"[^\n]*              { /* skip single-line comment */ }
"/*"([^*]|\*+[^*/])*\*+"/" {
    for (char *p = yytext; *p; p++) if (*p == '\n') line_num++;
}

\n              { line_num++; }
[ \t\r]+        { /* skip whitespace */ }

.   { fprintf(stderr, "Lexer error: unrecognised character '%s' at line %d\n",
              yytext, line_num); }

%%

int main(void) {
    printf("========================================\n");
    printf("  Module 1 — Lexer Token Stream\n");
    printf("========================================\n\n");
    printf("%-12s  %-14s  %s\n", "[Line]", "Token Type", "Value");
    printf("%-12s  %-14s  %s\n", "------", "----------", "-----");
    yylex();
    printf("\n[Lexer] Done.\n");
    return 0;
}
