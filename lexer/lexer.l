%{
#include "../parser/parser.tab.h"
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

"int"           { return INT; }
"float"         { return FLOAT_TYPE; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"return"        { return RETURN; }
"function"      { return FUNCTION; }
"procedure"     { return PROCEDURE; }
"begin"         { return BEGIN_KW; }
"end"           { return END_KW; }
"then"          { return THEN; }
"log"           { return LOG; }
"exp"           { return EXP_FUNC; }

{FLOAT}         { yylval.fval = atof(yytext); return FLOAT_LIT; }
{INTEGER}       { yylval.ival = atoi(yytext); return INT_LIT; }
{IDENTIFIER}    { strncpy(yylval.sval, yytext, 63); return IDENTIFIER; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MUL; }
"/"             { return DIV; }
"^"             { return POW; }
"="             { return ASSIGN; }
"=="            { return EQ; }
"!="            { return NEQ; }
"<"             { return LT; }
">"             { return GT; }
"<="            { return LE; }
">="            { return GE; }

"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
";"             { return SEMICOLON; }
","             { return COMMA; }

"//"[^\n]*      { /* skip single-line comment */ }
"/*"([^*]|\*+[^*/])*\*+"/" { 
    /* skip multi-line comment, count newlines */
    char *p;
    for(p = yytext; *p; p++) if(*p == '\n') line_num++;
}

\n              { line_num++; }
[ \t\r]+        { /* skip whitespace */ }

.               { fprintf(stderr, "Lexer error: unrecognised character '%s' at line %d\n", yytext, line_num); }

%%
