%{
/*
 * Module 3 — Extended Calculator Lexer
 * Supports numbers, identifiers, log(), exp(), and operators.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extended.tab.h"
%}

%option noyywrap

DIGIT   [0-9]
LETTER  [a-zA-Z_]

%%

"log"   { return LOG; }
"exp"   { return EXP_F; }

{DIGIT}+\.{DIGIT}+  { yylval.dval = atof(yytext); return NUMBER; }
{DIGIT}+            { yylval.dval = atof(yytext); return NUMBER; }

{LETTER}({LETTER}|{DIGIT})*  { strncpy(yylval.sval, yytext, 63); return ID; }

"+"   { return '+'; }
"-"   { return '-'; }
"*"   { return '*'; }
"/"   { return '/'; }
"^"   { return '^'; }
"("   { return '('; }
")"   { return ')'; }
"\n"  { return '\n'; }

[ \t\r]+  { /* skip whitespace */ }

.         { fprintf(stderr, "Unexpected character: %s\n", yytext); }

%%
