%{
#include <stdio.h>
#include <stdlib.h>

void yyerror(const char *s);
int yylex(void);
%}

%union { double dval; }
%token <dval> NUMBER
%type  <dval> expr

%%

program : expr_list ;

expr_list
    : expr_list expr '\n' { printf("Result: %g\n", $2); }
    | expr '\n'           { printf("Result: %g\n", $1); }
    ;

/* Prefix: operator comes before operands */
expr
    : '+' expr expr  { $$ = $2 + $3; }
    | '-' expr expr  { $$ = $2 - $3; }
    | '*' expr expr  { $$ = $2 * $3; }
    | '/' expr expr  {
        if ($3 == 0) { yyerror("Division by zero"); $$ = 0; }
        else $$ = $2 / $3;
      }
    | NUMBER          { $$ = $1; }
    ;

%%

void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }

int main(void) {
    printf("Prefix Calculator (enter expressions like: + 4 8)\n");
    return yyparse();
}
