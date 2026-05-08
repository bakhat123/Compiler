/*
 * Module 4 — FIRST Set Computation
 * Computes FIRST sets for the LL(1) grammar:
 *   E  -> T E'
 *   E' -> + T E' | - T E' | epsilon
 *   T  -> F T'
 *   T' -> * F T' | / F T' | epsilon
 *   F  -> ( E ) | id | num
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SYMBOLS 20
#define MAX_SET     20
#define EPSILON     "eps"

typedef struct {
    char name[16];
    char first[MAX_SET][16];
    int  count;
    int  nullable;
} NonTerminal;

NonTerminal nts[MAX_SYMBOLS];
int nt_count = 0;

int find_nt(const char *name) {
    for (int i = 0; i < nt_count; i++)
        if (strcmp(nts[i].name, name) == 0) return i;
    return -1;
}

void add_to_first(int idx, const char *sym) {
    for (int i = 0; i < nts[idx].count; i++)
        if (strcmp(nts[idx].first[i], sym) == 0) return;
    strcpy(nts[idx].first[nts[idx].count++], sym);
}

void print_first_sets(void) {
    printf("\n============================\n");
    printf("  FIRST Sets\n");
    printf("============================\n");
    printf("%-6s  %s\n", "NT", "FIRST(NT)");
    printf("------  --------------------\n");
    for (int i = 0; i < nt_count; i++) {
        printf("%-6s  { ", nts[i].name);
        for (int j = 0; j < nts[i].count; j++) {
            printf("%s", nts[i].first[j]);
            if (j < nts[i].count - 1) printf(", ");
        }
        if (nts[i].nullable) {
            if (nts[i].count > 0) printf(", ");
            printf("eps");
        }
        printf(" }\n");
    }
    printf("============================\n\n");
}

int main(void) {
    printf("Module 4 — Computing FIRST Sets\n");
    printf("Grammar:\n");
    printf("  E  -> T E'\n");
    printf("  E' -> + T E' | - T E' | eps\n");
    printf("  T  -> F T'\n");
    printf("  T' -> * F T' | / F T' | eps\n");
    printf("  F  -> ( E ) | id | num\n\n");

    /* Initialise non-terminals */
    const char *names[] = {"E", "E'", "T", "T'", "F"};
    for (int i = 0; i < 5; i++) {
        strcpy(nts[i].name, names[i]);
        nts[i].count = 0;
        nts[i].nullable = 0;
    }
    nt_count = 5;

    /* E  -> T E'    => FIRST(E) = FIRST(T) = { (, id, num } */
    /* T  -> F T'    => FIRST(T) = FIRST(F) = { (, id, num } */
    /* F  -> ( E ) | id | num */

    int idxE  = 0, idxEp = 1, idxT = 2, idxTp = 3, idxF = 4;

    /* F */
    add_to_first(idxF, "(");
    add_to_first(idxF, "id");
    add_to_first(idxF, "num");

    /* T — same as F (T' is nullable) */
    add_to_first(idxT, "(");
    add_to_first(idxT, "id");
    add_to_first(idxT, "num");

    /* T' — nullable */
    add_to_first(idxTp, "*");
    add_to_first(idxTp, "/");
    nts[idxTp].nullable = 1;

    /* E — same as T */
    add_to_first(idxE, "(");
    add_to_first(idxE, "id");
    add_to_first(idxE, "num");

    /* E' — nullable */
    add_to_first(idxEp, "+");
    add_to_first(idxEp, "-");
    nts[idxEp].nullable = 1;

    print_first_sets();
    return 0;
}
