%{
/*
 * Module 2 — Shared lexer for postfix, prefix, and infix calculators
 * Returns NUMBER tokens (doubles) and single-char operators.
 *
 * This file is used by all three calculator parsers.
 * The correct .tab.h is selected via -D flags at compile time:
 *   -DPOSTFIX_CALC  => postfix.tab.h
 *   -DPREFIX_CALC   => prefix.tab.h
 *   (default)       => infix.tab.h
 */
#include <stdio.h>
#include <stdlib.h>

#if defined(POSTFIX_CALC)
  #include "postfix.tab.h"
#elif defined(PREFIX_CALC)
  #include "prefix.tab.h"
#else
  #include "infix.tab.h"
#endif

%}

%option noyywrap

DIGIT   [0-9]

%%

{DIGIT}+\.{DIGIT}+  { yylval.dval = atof(yytext); return NUMBER; }
{DIGIT}+            { yylval.dval = atof(yytext); return NUMBER; }

"+"   { return '+'; }
"-"   { return '-'; }
"*"   { return '*'; }
"/"   { return '/'; }
"^"   { return '^'; }
"("   { return '('; }
")"   { return ')'; }
"\n"  { return '\n'; }

[ \t\r]+  { /* skip whitespace */ }

.         { /* pass through any other char */ return yytext[0]; }

%%
