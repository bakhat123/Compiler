/*
 * Module 7 — Task 5: Loop Optimization — LICM
 * (Loop-Invariant Code Motion)
 *
 * Detects simple while-loop patterns in the TAC:
 *   L_loop:
 *     ifFalse cond goto L_exit
 *     <body instructions>
 *     goto L_loop
 *   L_exit:
 *
 * An instruction inside the loop body is loop-invariant if neither
 * arg1 nor arg2 is assigned anywhere inside the loop body.
 * Such instructions are moved to just before L_loop.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ir/ir_gen.h"

#define MAX_DEFS 128

/* Check if a variable is defined (assigned) inside [body_start, body_end) */
static int defined_in_body(const char *var,
                            TACInstr *body_start, TACInstr *body_end) {
    if (!var || !var[0]) return 0;
    for (TACInstr *p = body_start; p && p != body_end; p = p->next)
        if (p->result[0] && strcmp(p->result, var) == 0) return 1;
    return 0;
}

static int is_const_or_not_defined(const char *arg,
                                    TACInstr *bs, TACInstr *be) {
    if (!arg || !arg[0]) return 1;
    return !defined_in_body(arg, bs, be);
}

TACInstr *loop_invariant_motion(TACInstr *head) {
    int hoisted = 0;
    TACInstr *p = head;

    while (p) {
        /* Look for loop header label */
        if (p->op != TAC_LABEL) { p = p->next; continue; }

        char loop_label[64];
        strncpy(loop_label, p->result, 63);
        TACInstr *loop_header = p;

        /* Next instruction should be the conditional exit */
        TACInstr *cond = p->next;
        if (!cond || cond->op != TAC_IF_FALSE) { p = p->next; continue; }

        char exit_label[64];
        strncpy(exit_label, cond->result, 63);

        /* Find loop body end: goto loop_label */
        TACInstr *body_start = cond->next;
        TACInstr *body_end   = NULL;
        TACInstr *goto_instr = NULL;

        for (TACInstr *q = body_start; q; q = q->next) {
            if (q->op == TAC_GOTO && strcmp(q->result, loop_label) == 0) {
                goto_instr = q;
                body_end   = q;   /* exclusive end */
                break;
            }
        }
        if (!goto_instr) { p = p->next; continue; }

        /* Scan body for loop-invariant instructions */
        TACInstr *prev_q = cond;
        TACInstr *q = body_start;
        while (q && q != body_end) {
            TACInstr *nxt = q->next;
            int invariant = 0;

            switch (q->op) {
                case TAC_ADD: case TAC_SUB: case TAC_MUL: case TAC_DIV:
                case TAC_POW: case TAC_NEG: case TAC_LOG: case TAC_EXP:
                case TAC_ASSIGN:
                    invariant = is_const_or_not_defined(q->arg1, body_start, body_end) &&
                                is_const_or_not_defined(q->arg2, body_start, body_end) &&
                                !defined_in_body(q->result, body_start, body_end);
                    break;
                default: break;
            }

            if (invariant) {
                printf("[LICM] Hoisting out of loop '%s': %s = %s %s %s\n",
                       loop_label, q->result, q->arg1,
                       tac_op_name(q->op), q->arg2);

                /* Detach from body */
                prev_q->next = nxt;

                /* Insert before loop_header */
                /* Find instruction before loop_header */
                if (head == loop_header) {
                    q->next = head;
                    head = q;
                } else {
                    TACInstr *before = head;
                    while (before->next && before->next != loop_header)
                        before = before->next;
                    q->next = loop_header;
                    before->next = q;
                }
                hoisted++;
                q = nxt;
                continue;
            }
            prev_q = q;
            q = nxt;
        }
        p = goto_instr->next;   /* continue after loop */
    }
    printf("[LICM] Total hoisted: %d\n", hoisted);
    return head;
}
