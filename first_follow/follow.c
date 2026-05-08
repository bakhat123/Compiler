/*
 * Module 4 — FOLLOW Set Computation
 * Computes FOLLOW sets for the LL(1) grammar.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SET 20

typedef struct {
    char name[16];
    char follow[MAX_SET][16];
    int  count;
} NonTerminal;

NonTerminal nts[10];
int nt_count = 0;

void add_to_follow(int idx, const char *sym) {
    for (int i = 0; i < nts[idx].count; i++)
        if (strcmp(nts[idx].follow[i], sym) == 0) return;
    strcpy(nts[idx].follow[nts[idx].count++], sym);
}

void print_follow_sets(void) {
    printf("\n============================\n");
    printf("  FOLLOW Sets\n");
    printf("============================\n");
    printf("%-6s  %s\n", "NT", "FOLLOW(NT)");
    printf("------  --------------------\n");
    for (int i = 0; i < nt_count; i++) {
        printf("%-6s  { ", nts[i].name);
        for (int j = 0; j < nts[i].count; j++) {
            printf("%s", nts[i].follow[j]);
            if (j < nts[i].count - 1) printf(", ");
        }
        printf(" }\n");
    }
    printf("============================\n\n");
}

int main(void) {
    printf("Module 4 — Computing FOLLOW Sets\n\n");

    const char *names[] = {"E", "E'", "T", "T'", "F"};
    for (int i = 0; i < 5; i++) {
        strcpy(nts[i].name, names[i]);
        nts[i].count = 0;
    }
    nt_count = 5;

    int idxE=0, idxEp=1, idxT=2, idxTp=3, idxF=4;

    /*
     * Rules:
     *   E  -> T E'
     *   E' -> + T E' | - T E' | eps
     *   T  -> F T'
     *   T' -> * F T' | / F T' | eps
     *   F  -> ( E ) | id | num
     *
     * FOLLOW(E)  = { $, ) }
     * FOLLOW(E') = FOLLOW(E) = { $, ) }
     * FOLLOW(T)  = FIRST(E') \ {eps} U FOLLOW(E) = { +, -, $, ) }
     * FOLLOW(T') = FOLLOW(T) = { +, -, $, ) }
     * FOLLOW(F)  = FIRST(T') \ {eps} U FOLLOW(T) = { *, /, +, -, $, ) }
     */

    /* FOLLOW(E) */
    add_to_follow(idxE,  "$");
    add_to_follow(idxE,  ")");

    /* FOLLOW(E') = FOLLOW(E) */
    add_to_follow(idxEp, "$");
    add_to_follow(idxEp, ")");

    /* FOLLOW(T) */
    add_to_follow(idxT, "+");
    add_to_follow(idxT, "-");
    add_to_follow(idxT, "$");
    add_to_follow(idxT, ")");

    /* FOLLOW(T') = FOLLOW(T) */
    add_to_follow(idxTp, "+");
    add_to_follow(idxTp, "-");
    add_to_follow(idxTp, "$");
    add_to_follow(idxTp, ")");

    /* FOLLOW(F) */
    add_to_follow(idxF, "*");
    add_to_follow(idxF, "/");
    add_to_follow(idxF, "+");
    add_to_follow(idxF, "-");
    add_to_follow(idxF, "$");
    add_to_follow(idxF, ")");

    print_follow_sets();
    return 0;
}
