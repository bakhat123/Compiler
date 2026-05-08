/*
 * Module 7 — Task 3c: Dead Code Elimination
 * Removes TAC instructions whose result is never subsequently read.
 *
 * Algorithm (single backwards pass):
 *   1. Walk list from end to start building a "live" set.
 *   2. An instruction's result is dead if it is not in the live set
 *      when we reach that instruction.
 *   3. After removing, add the instruction's operands to the live set.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ir/ir_gen.h"

#define MAX_LIVE 256

typedef struct { char name[64]; } LiveVar;
static LiveVar live[MAX_LIVE];
static int live_sz = 0;

static void live_add(const char *v) {
    if (!v || !v[0]) return;
    for (int i = 0; i < live_sz; i++)
        if (strcmp(live[i].name, v) == 0) return;
    if (live_sz < MAX_LIVE)
        strncpy(live[live_sz++].name, v, 63);
}

static void live_remove(const char *v) {
    for (int i = 0; i < live_sz; i++)
        if (strcmp(live[i].name, v) == 0) {
            live[i] = live[--live_sz]; return;
        }
}

static int is_live(const char *v) {
    for (int i = 0; i < live_sz; i++)
        if (strcmp(live[i].name, v) == 0) return 1;
    return 0;
}

/* Reverse the list in-place */
static TACInstr *reverse_list(TACInstr *head) {
    TACInstr *prev = NULL, *cur = head, *nxt;
    while (cur) { nxt = cur->next; cur->next = prev; prev = cur; cur = nxt; }
    return prev;
}

TACInstr *dead_code_elim(TACInstr *head) {
    live_sz = 0;

    /* Reverse pass: last instruction's result is always live */
    TACInstr *rev = reverse_list(head);
    TACInstr *prev = NULL, *cur = rev, *new_head_rev = NULL;
    int removed = 0;

    while (cur) {
        TACInstr *nxt = cur->next;
        int keep = 1;

        /* Instructions with side effects are always kept */
        int side_effect = (cur->op == TAC_CALL   || cur->op == TAC_PARAM  ||
                           cur->op == TAC_RETURN  || cur->op == TAC_GOTO   ||
                           cur->op == TAC_IF_TRUE || cur->op == TAC_IF_FALSE ||
                           cur->op == TAC_LABEL   || cur->op == TAC_ARRAY_STORE);

        if (!side_effect && cur->result[0] && !is_live(cur->result)) {
            printf("[DeadCode] Removing dead: %s = ...\n", cur->result);
            free(cur);
            removed++;
            keep = 0;
        }

        if (keep) {
            /* Mark operands as live, result as defined (remove from live) */
            if (cur->result[0]) live_remove(cur->result);
            live_add(cur->arg1);
            live_add(cur->arg2);
            cur->next = new_head_rev;
            new_head_rev = cur;
        }

        cur = nxt;
    }

    /* Reverse back to original order */
    TACInstr *final = reverse_list(new_head_rev);
    printf("[DeadCode] Total removed: %d\n", removed);
    return final;
}
