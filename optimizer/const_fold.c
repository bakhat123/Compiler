/*
 * Module 7 — Task 3a: Constant Folding
 * Evaluates expressions where both operands are numeric constants at
 * compile time, replacing the instruction with a simple assignment.
 *
 * Example:  t1 = 3 + 4   =>   t1 = 7
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "../ir/ir_gen.h"

/* Returns 1 if the string is a numeric constant (int or float) */
static int is_const(const char *s) {
    if (!s || !*s) return 0;
    const char *p = s;
    if (*p == '-') p++;
    int has_digit = 0, has_dot = 0;
    for (; *p; p++) {
        if (isdigit(*p)) { has_digit = 1; continue; }
        if (*p == '.' && !has_dot) { has_dot = 1; continue; }
        return 0;
    }
    return has_digit;
}

static double to_num(const char *s) { return atof(s); }

static void store_result(char *buf, double v) {
    /* Use integer representation if possible */
    if (v == (long long)v)
        sprintf(buf, "%lld", (long long)v);
    else
        sprintf(buf, "%.6g", v);
}

TACInstr *const_fold(TACInstr *head) {
    int folded = 0;
    for (TACInstr *p = head; p; p = p->next) {
        /* Unary ops with constant operand */
        if (p->op == TAC_NEG && is_const(p->arg1)) {
            double v = -to_num(p->arg1);
            store_result(p->arg1, v);
            p->op = TAC_ASSIGN;
            p->arg2[0] = '\0';
            folded++;
            continue;
        }
        if (p->op == TAC_LOG && is_const(p->arg1)) {
            double a = to_num(p->arg1);
            if (a > 0) {
                store_result(p->arg1, log(a));
                p->op = TAC_ASSIGN; p->arg2[0] = '\0'; folded++;
            }
            continue;
        }
        if (p->op == TAC_EXP && is_const(p->arg1)) {
            store_result(p->arg1, exp(to_num(p->arg1)));
            p->op = TAC_ASSIGN; p->arg2[0] = '\0'; folded++;
            continue;
        }

        /* Binary ops — both args must be constants */
        if (!is_const(p->arg1) || !is_const(p->arg2)) continue;

        double a = to_num(p->arg1), b = to_num(p->arg2), v = 0;
        int do_fold = 1;

        switch (p->op) {
            case TAC_ADD: v = a + b; break;
            case TAC_SUB: v = a - b; break;
            case TAC_MUL: v = a * b; break;
            case TAC_DIV:
                if (b == 0) { fprintf(stderr, "[ConstFold] Division by zero — skipped\n"); do_fold=0; }
                else v = a / b;
                break;
            case TAC_POW: v = pow(a, b); break;
            case TAC_LT:  v = (a <  b); break;
            case TAC_GT:  v = (a >  b); break;
            case TAC_LE:  v = (a <= b); break;
            case TAC_GE:  v = (a >= b); break;
            case TAC_EQ:  v = (a == b); break;
            case TAC_NEQ: v = (a != b); break;
            default: do_fold = 0;
        }

        if (do_fold) {
            store_result(p->arg1, v);
            p->arg2[0] = '\0';
            p->op = TAC_ASSIGN;
            folded++;
            printf("[ConstFold] %s = %.6g %s %.6g  =>  %s = %s\n",
                   p->result, a, tac_op_name(p->op), b, p->result, p->arg1);
        }
    }
    printf("[ConstFold] Total folded: %d\n", folded);
    return head;
}
