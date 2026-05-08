%{
/*
 * Module 3 — Extended Grammar
 * Grammar:
 *   E  →  E + T | E - T | T
 *   T  →  T * F | T / F | F
 *   F  →  B ^ F | B          (right-associative)
 *   B  →  ( E ) | id | num | log(E) | exp(E)
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define YYDEBUG 1

void yyerror(const char *s);
int yylex(void);

int yydebug = 0;
%}

%union {
    double dval;
    char   sval[64];
}

%token <dval> NUMBER
%token <sval> ID
%token LOG EXP_F
%type  <dval> E T F B

%left  '+' '-'
%left  '*' '/'
%right POW_OP
%right UMINUS

%%

program
    : program E '\n' { printf("= %g\n\n", $2); }
    | E '\n'         { printf("= %g\n\n", $1); }
    | error '\n'     { yyerrok; fprintf(stderr, "Syntax error — please re-enter\n"); }
    ;

E
    : E '+' T  { $$ = $1 + $3; }
    | E '-' T  { $$ = $1 - $3; }
    | T        { $$ = $1; }
    ;

T
    : T '*' F  { $$ = $1 * $3; }
    | T '/' F  {
        if ($3 == 0) { yyerror("Division by zero"); $$ = 0; }
        else          $$ = $1 / $3;
      }
    | F        { $$ = $1; }
    ;

/* Right-associative exponentiation: 2^3^2 == 2^(3^2) == 512 */
F
    : B '^' F  { $$ = pow($1, $3); }
    | B        { $$ = $1; }
    ;

B
    : '(' E ')'        { $$ = $2; }
    | '-' B %prec UMINUS { $$ = -$2; }
    | LOG '(' E ')'    {
        if ($3 <= 0) { yyerror("log of non-positive number"); $$ = 0; }
        else          $$ = log($3);
      }
    | EXP_F '(' E ')'  { $$ = exp($3); }
    | NUMBER            { $$ = $1; }
    | ID                {
        fprintf(stderr, "Warning: identifier '%s' treated as 0\n", $1);
        $$ = 0;
      }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-d") == 0)
        yydebug = 1;
    printf("Extended Calculator — supports +, -, *, /, ^, log(), exp()\n");
    printf("Example: 2^3^2   =>  512\n");
    printf("         log(2.718281828)  =>  1\n\n");
    return yyparse();
}
