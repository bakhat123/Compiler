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

/* Postfix: operands come before operator */
expr
    : expr expr '+'  { $$ = $1 + $2; }
    | expr expr '-'  { $$ = $1 - $2; }
    | expr expr '*'  { $$ = $1 * $2; }
    | expr expr '/'  {
        if ($2 == 0) { yyerror("Division by zero"); $$ = 0; }
        else $$ = $1 / $2;
      }
    | NUMBER          { $$ = $1; }
    ;

%%

void yyerror(const char *s) { fprintf(stderr, "Error: %s\n", s); }

int main(void) {
    printf("Postfix Calculator (enter expressions like: 4 8 +)\n");
    return yyparse();
}
