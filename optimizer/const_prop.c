/*
 * Module 7 — Task 3b: Constant Propagation
 * Tracks variables that hold a known constant value and substitutes
 * them at every subsequent use.
 *
 * Example:
 *   x = 5          <- x is const 5
 *   t1 = x + 3     => t1 = 5 + 3   (then const fold can reduce to 8)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ir/ir_gen.h"

#define MAX_CONSTS 128

typedef struct { char var[64]; char val[64]; } ConstEntry;
static ConstEntry env[MAX_CONSTS];
static int env_size = 0;

static int is_numeric(const char *s) {
    if (!s || !*s) return 0;
    const char *p = s; if (*p == '-') p++;
    int d=0, dot=0;
    for(;*p;p++){
        if(isdigit(*p)){d=1;continue;}
        if(*p=='.'&&!dot){dot=1;continue;}
        return 0;
    }
    return d;
}

static void set_const(const char *var, const char *val) {
    for (int i = 0; i < env_size; i++)
        if (strcmp(env[i].var, var) == 0) {
            strncpy(env[i].val, val, 63); return;
        }
    if (env_size < MAX_CONSTS) {
        strncpy(env[env_size].var, var, 63);
        strncpy(env[env_size].val, val, 63);
        env_size++;
    }
}

static void kill_const(const char *var) {
    for (int i = 0; i < env_size; i++)
        if (strcmp(env[i].var, var) == 0) {
            env[i] = env[--env_size]; return;
        }
}

static const char *get_const(const char *var) {
    for (int i = 0; i < env_size; i++)
        if (strcmp(env[i].var, var) == 0) return env[i].val;
    return NULL;
}

static int propagate_arg(char *arg) {
    if (is_numeric(arg)) return 0;
    const char *v = get_const(arg);
    if (v) { strncpy(arg, v, 63); return 1; }
    return 0;
}

TACInstr *const_prop(TACInstr *head) {
    env_size = 0;
    int replaced = 0;

    for (TACInstr *p = head; p; p = p->next) {
        /* Substitute in operands first */
        if (p->op != TAC_LABEL && p->op != TAC_GOTO) {
            replaced += propagate_arg(p->arg1);
            replaced += propagate_arg(p->arg2);
        }

        /* Update constant map for result */
        if (p->op == TAC_ASSIGN && is_numeric(p->arg1)) {
            set_const(p->result, p->arg1);
            printf("[ConstProp] %s = %s\n", p->result, p->arg1);
        } else if (p->result[0]) {
            kill_const(p->result);   /* result overwritten — no longer const */
        }
    }
    printf("[ConstProp] Total replacements: %d\n", replaced);
    return head;
}
