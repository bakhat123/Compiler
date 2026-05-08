%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void yyerror(const char *s);
int yylex(void);
%}

%union { double dval; }
%token <dval> NUMBER
%type  <dval> expr

/* Precedence: lowest to highest */
%left  '+' '-'
%left  '*' '/'
%right '^'
%right UMINUS

%%

program : expr_list ;

expr_list
    : expr_list expr '\n' { printf("Result: %g\n", $2); }
    | expr '\n'           { printf("Result: %g\n", $1); }
    ;

expr
    : expr '+' expr  { $$ = $1 + $3; }
    | expr '-' expr  { $$ = $1 - $3; }
    | expr '*' expr  { $$ = $1 * $3; }
    | expr '/' expr  {
        if ($3 == 0) { yyerror("Division by zero"); $$ = 0; }
        else $$ = $1 / $3;
      }
    | expr '^' expr  { $$ = pow($1, $3); }
    | '-' expr %prec UMINUS { $$ = -$2; }
    | '(' expr ')'   { $$ = $2; }
    | NUMBER          { $$ = $1; }
    ;

%%

void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }

int main(void) {
    printf("Infix Calculator (enter expressions like: 4 + 8)\n");
    return yyparse();
}
