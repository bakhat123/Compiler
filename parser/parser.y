%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../semantic/symbol_table.h"
#include "../ir/ir_gen.h"
#include "../optimizer/optimizer.h"

extern int yylex(void);
extern int line_num;
void yyerror(const char *s);

/* Global IR list */
TACInstr *ir_head = NULL;
TACInstr *ir_tail = NULL;

static int temp_count = 0;

char *new_temp() {
    char *t = malloc(16);
    sprintf(t, "t%d", ++temp_count);
    return t;
}

static int label_count = 0;
char *new_label() {
    char *l = malloc(16);
    sprintf(l, "L%d", ++label_count);
    return l;
}

void emit(TACInstr *instr) {
    instr->next = NULL;
    if (!ir_head) { ir_head = ir_tail = instr; }
    else { ir_tail->next = instr; ir_tail = instr; }
}
%}

%union {
    int    ival;
    float  fval;
    char   sval[64];
}

/* Tokens */
%token <ival> INT_LIT
%token <fval> FLOAT_LIT
%token <sval> IDENTIFIER

%token INT FLOAT_TYPE IF ELSE WHILE RETURN FUNCTION PROCEDURE
%token BEGIN_KW END_KW THEN LOG EXP_FUNC

%token PLUS MINUS MUL DIV POW
%token ASSIGN EQ NEQ LT GT LE GE
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA

%type <sval> expr term factor base decl_type

%right ASSIGN
%left EQ NEQ
%left LT GT LE GE
%left PLUS MINUS
%left MUL DIV
%right POW
%right UMINUS

%start program

%%

program
    : stmt_list
    ;

stmt_list
    : stmt_list stmt
    | stmt
    ;

stmt
    : decl_type IDENTIFIER SEMICOLON
        {
            declare_var($2, $1);
        }
    | IDENTIFIER ASSIGN expr SEMICOLON
        {
            if (!lookup_var($1)) {
                fprintf(stderr, "Semantic error: undeclared variable '%s' at line %d\n", $1, line_num);
            } else {
                TACInstr *instr = make_instr(TAC_ASSIGN, $1, $3, "");
                emit(instr);
            }
        }
    | IF LPAREN expr RPAREN THEN BEGIN_KW stmt_list END_KW
        {
            /* simplified if — no label backpatching in this skeleton */
        }
    | WHILE LPAREN expr RPAREN BEGIN_KW stmt_list END_KW
        {
        }
    | RETURN expr SEMICOLON
        {
            TACInstr *instr = make_instr(TAC_RETURN, "", $2, "");
            emit(instr);
        }
    | error SEMICOLON { yyerrok; }
    ;

decl_type
    : INT       { strcpy($$, "int"); }
    | FLOAT_TYPE { strcpy($$, "float"); }
    ;

expr
    : expr PLUS term
        {
            char *t = new_temp();
            TACInstr *instr = make_instr(TAC_ADD, t, $1, $3);
            emit(instr);
            strcpy($$, t);
        }
    | expr MINUS term
        {
            char *t = new_temp();
            TACInstr *instr = make_instr(TAC_SUB, t, $1, $3);
            emit(instr);
            strcpy($$, t);
        }
    | term { strcpy($$, $1); }
    ;

term
    : term MUL factor
        {
            char *t = new_temp();
            TACInstr *instr = make_instr(TAC_MUL, t, $1, $3);
            emit(instr);
            strcpy($$, t);
        }
    | term DIV factor
        {
            char *t = new_temp();
            TACInstr *instr = make_instr(TAC_DIV, t, $1, $3);
            emit(instr);
            strcpy($$, t);
        }
    | factor { strcpy($$, $1); }
    ;

factor
    : base POW factor
        {
            char *t = new_temp();
            TACInstr *instr = make_instr(TAC_POW, t, $1, $3);
            emit(instr);
            strcpy($$, t);
        }
    | base { strcpy($$, $1); }
    ;

base
    : LPAREN expr RPAREN { strcpy($$, $2); }
    | MINUS base %prec UMINUS
        {
            char *t = new_temp();
            TACInstr *instr = make_instr(TAC_NEG, t, $2, "");
            emit(instr);
            strcpy($$, t);
        }
    | LOG LPAREN expr RPAREN
        {
            char *t = new_temp();
            TACInstr *instr = make_instr(TAC_LOG, t, $3, "");
            emit(instr);
            strcpy($$, t);
        }
    | EXP_FUNC LPAREN expr RPAREN
        {
            char *t = new_temp();
            TACInstr *instr = make_instr(TAC_EXP, t, $3, "");
            emit(instr);
            strcpy($$, t);
        }
    | INT_LIT
        {
            sprintf($$, "%d", $1);
        }
    | FLOAT_LIT
        {
            sprintf($$, "%.6g", $1);
        }
    | IDENTIFIER
        {
            if (!lookup_var($1)) {
                fprintf(stderr, "Semantic error: undeclared variable '%s' at line %d\n", $1, line_num);
            }
            strcpy($$, $1);
        }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error at line %d: %s\n", line_num, s);
}
