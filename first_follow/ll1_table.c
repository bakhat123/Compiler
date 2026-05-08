/*
 * Module 4 — LL(1) Parsing Table
 * Constructs and prints the LL(1) table for the arithmetic grammar.
 */

#include <stdio.h>
#include <string.h>

/* Non-terminals */
#define NT_E   0
#define NT_EP  1
#define NT_T   2
#define NT_TP  3
#define NT_F   4
#define NT_COUNT 5

/* Terminals (column indices) */
#define T_PLUS  0
#define T_MINUS 1
#define T_MUL   2
#define T_DIV   3
#define T_LPAREN 4
#define T_RPAREN 5
#define T_ID    6
#define T_NUM   7
#define T_DOLLAR 8
#define T_COUNT  9

const char *nt_names[NT_COUNT] = { "E", "E'", "T", "T'", "F" };
const char *t_names[T_COUNT]   = { "+", "-", "*", "/", "(", ")", "id", "num", "$" };

/* Table[NT][T] = production string, or "" for error */
const char *table[NT_COUNT][T_COUNT] = {
    /* E */
    { "",        "",        "",        "",        "T E'",   "",      "T E'",   "T E'",   ""       },
    /* E' */
    { "+ T E'",  "- T E'",  "",        "",        "",       "eps",   "",       "",       "eps"    },
    /* T */
    { "",        "",        "",        "",        "F T'",   "",      "F T'",   "F T'",   ""       },
    /* T' */
    { "eps",     "eps",     "* F T'",  "/ F T'",  "",       "eps",   "",       "",       "eps"    },
    /* F */
    { "",        "",        "",        "",        "( E )",  "",      "id",     "num",    ""       },
};

void print_ll1_table(void) {
    int col_w = 10;

    printf("\n");
    printf("============================================================"
           "========================================\n");
    printf("  LL(1) Parsing Table\n");
    printf("  Grammar: E->TE' | E'->+TE'|-TE'|eps | T->FT' | T'->*FT'|/FT'|eps | F->(E)|id|num\n");
    printf("============================================================"
           "========================================\n\n");

    /* Header */
    printf("%-6s |", "NT");
    for (int t = 0; t < T_COUNT; t++)
        printf(" %-*s|", col_w, t_names[t]);
    printf("\n");

    /* Separator */
    printf("-------|");
    for (int t = 0; t < T_COUNT; t++) {
        for (int k = 0; k < col_w+1; k++) putchar('-');
        putchar('|');
    }
    printf("\n");

    /* Rows */
    for (int nt = 0; nt < NT_COUNT; nt++) {
        printf("%-6s |", nt_names[nt]);
        for (int t = 0; t < T_COUNT; t++) {
            if (table[nt][t][0] == '\0')
                printf(" %-*s|", col_w, "error");
            else
                printf(" %-*s|", col_w, table[nt][t]);
        }
        printf("\n");
    }

    printf("\n(eps = epsilon / empty production; error = syntax error)\n\n");
}

/* Demonstrate a simple parse of "id + num" */
void demo_parse(void) {
    printf("--- Demo Parse: id + num $ ---\n");
    printf("Stack        Input        Rule/Action\n");
    printf("------       --------     -----------\n");

    /* Hard-coded trace for illustration */
    const char *steps[][3] = {
        { "$ E",        "id + num $", "Start" },
        { "$ E' T",     "id + num $", "E -> T E'" },
        { "$ E' T' F",  "id + num $", "T -> F T'" },
        { "$ E' T' id", "id + num $", "F -> id" },
        { "$ E' T'",    "+ num $",    "Match id" },
        { "$ E'",       "+ num $",    "T' -> eps  (FOLLOW)" },
        { "$ E' T +",   "+ num $",    "E' -> + T E'" },
        { "$ E' T",     "num $",      "Match +" },
        { "$ E' T' F",  "num $",      "T -> F T'" },
        { "$ E' T' num","num $",      "F -> num" },
        { "$ E' T'",    "$",          "Match num" },
        { "$ E'",       "$",          "T' -> eps" },
        { "$",          "$",          "E' -> eps" },
        { "",           "$",          "Accept!" },
    };

    for (int i = 0; steps[i][2][0] != '\0' || i == 13; i++) {
        printf("%-12s %-12s %s\n", steps[i][0], steps[i][1], steps[i][2]);
        if (strcmp(steps[i][2], "Accept!") == 0) break;
    }
    printf("\n");
}

int main(void) {
    printf("Module 4 — LL(1) Parsing Table Construction\n");
    print_ll1_table();
    demo_parse();
    return 0;
}
