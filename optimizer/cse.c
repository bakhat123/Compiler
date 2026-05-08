/*
 * Module 7 — Task 3d: Common Sub-expression Elimination (CSE)
 * If the same binary expression has already been computed and its
 * operands have not changed, reuse the earlier result instead of
 * recomputing.
 *
 * Example:
 *   t1 = a + b      <- first occurrence
 *   ...             (a and b not redefined)
 *   t2 = a + b      => t2 = t1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ir/ir_gen.h"

#define MAX_EXPRS 128

typedef struct {
    TACOp  op;
    char   arg1[64];
    char   arg2[64];
    char   result[64];   /* temp that holds this value */
} ExprEntry;

static ExprEntry cache[MAX_EXPRS];
static int cache_sz = 0;

/* Invalidate all cache entries that use 'var' as an operand */
static void invalidate(const char *var) {
    int i = 0;
    while (i < cache_sz) {
        if (strcmp(cache[i].arg1, var) == 0 ||
            strcmp(cache[i].arg2, var) == 0 ||
            strcmp(cache[i].result, var) == 0) {
            cache[i] = cache[--cache_sz];
        } else i++;
    }
}

static int find_expr(TACOp op, const char *a1, const char *a2,
                     char *out_result) {
    for (int i = 0; i < cache_sz; i++) {
        if (cache[i].op != op) continue;
        /* Commutative: also check swapped args for + and * */
        int match = (strcmp(cache[i].arg1, a1) == 0 &&
                     strcmp(cache[i].arg2, a2) == 0);
        if (!match && (op == TAC_ADD || op == TAC_MUL))
            match = (strcmp(cache[i].arg1, a2) == 0 &&
                     strcmp(cache[i].arg2, a1) == 0);
        if (match) {
            strncpy(out_result, cache[i].result, 63);
            return 1;
        }
    }
    return 0;
}

static void add_expr(TACOp op, const char *a1, const char *a2,
                     const char *res) {
    if (cache_sz >= MAX_EXPRS) return;
    cache[cache_sz].op = op;
    strncpy(cache[cache_sz].arg1,   a1,  63);
    strncpy(cache[cache_sz].arg2,   a2,  63);
    strncpy(cache[cache_sz].result, res, 63);
    cache_sz++;
}

static int is_binary(TACOp op) {
    return op==TAC_ADD||op==TAC_SUB||op==TAC_MUL||op==TAC_DIV||op==TAC_POW;
}

TACInstr *cse_pass(TACInstr *head) {
    cache_sz = 0;
    int replaced = 0;

    for (TACInstr *p = head; p; p = p->next) {
        if (is_binary(p->op) && p->arg1[0] && p->arg2[0]) {
            char prev[64];
            if (find_expr(p->op, p->arg1, p->arg2, prev)) {
                printf("[CSE] %s = %s %s %s  =>  %s = %s\n",
                       p->result, p->arg1, tac_op_name(p->op), p->arg2,
                       p->result, prev);
                /* Replace with copy */
                strncpy(p->arg1, prev, 63);
                p->arg2[0] = '\0';
                p->op = TAC_ASSIGN;
                replaced++;
            } else {
                add_expr(p->op, p->arg1, p->arg2, p->result);
            }
        }
        /* Any assignment to result invalidates cached exprs using it */
        if (p->result[0]) invalidate(p->result);
    }
    printf("[CSE] Total replaced: %d\n", replaced);
    return head;
}
