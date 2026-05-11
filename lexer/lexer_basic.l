%{
/*
 * Module 1 — Task 1.1: Basic Lexer (Lab 05 Baseline)
 * Matches integer/float literals, identifiers (lowercase start),
 * keywords, operators, and skips whitespace.
 *
 * Build:  flex -o lexer_basic.yy.c lexer_basic.l && gcc -o lexer_basic lexer_basic.yy.c -lfl
 * Run:    ./lexer_basic < input.txt
 */
#include <stdio.h>
int line_num = 1;
%}

%option noyywrap

DIGIT       [0-9]
INTEGER     {DIGIT}+
FLOAT       {DIGIT}+\.{DIGIT}+
IDENTIFIER  [a-z][a-zA-Z0-9]*

%%

"if"         { printf("[Line %2d]  KEYWORD    : %s\n", line_num, yytext); }
"then"       { printf("[Line %2d]  KEYWORD    : %s\n", line_num, yytext); }
"begin"      { printf("[Line %2d]  KEYWORD    : %s\n", line_num, yytext); }
"end"        { printf("[Line %2d]  KEYWORD    : %s\n", line_num, yytext); }
"procedure"  { printf("[Line %2d]  KEYWORD    : %s\n", line_num, yytext); }
"function"   { printf("[Line %2d]  KEYWORD    : %s\n", line_num, yytext); }

{FLOAT}      { printf("[Line %2d]  FLOAT      : %s\n", line_num, yytext); }
{INTEGER}    { printf("[Line %2d]  INTEGER    : %s\n", line_num, yytext); }
{IDENTIFIER} { printf("[Line %2d]  IDENTIFIER : %s\n", line_num, yytext); }

"+"          { printf("[Line %2d]  OPERATOR   : +\n",  line_num); }
"-"          { printf("[Line %2d]  OPERATOR   : -\n",  line_num); }
"*"          { printf("[Line %2d]  OPERATOR   : *\n",  line_num); }
"/"          { printf("[Line %2d]  OPERATOR   : /\n",  line_num); }

\n           { line_num++; }
[ \t\r]+     { /* skip whitespace */ }

.            { fprintf(stderr, "Error: unrecognised character '%s' at line %d\n", yytext, line_num); }

%%

int main(void) {
    printf("=== Basic Lexer (Lab 05) ===\n\n");
    yylex();
    printf("\n[Lexer] Done.\n");
    return 0;
}
