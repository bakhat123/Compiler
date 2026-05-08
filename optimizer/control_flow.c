/*
 * Module 7 — Task 4: Control Flow / Unreachable Code Removal
 *
 * Strategy (linear scan — suitable for TAC without explicit CFG):
 *   After an unconditional GOTO or RETURN, any instruction that is NOT
 *   a LABEL is unreachable until the next LABEL is seen.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ir/ir_gen.h"

TACInstr *remove_unreachable(TACInstr *head) {
    int removed = 0;
    int unreachable = 0;         /* flag: currently in dead region */

    TACInstr *prev = NULL, *cur = head, *new_head = NULL;

    while (cur) {
        TACInstr *nxt = cur->next;

        if (cur->op == TAC_LABEL) {
            /* Label reachable from somewhere — reset flag */
            unreachable = 0;
        }

        if (unreachable) {
            printf("[CtrlFlow] Removing unreachable: ");
            if (cur->op == TAC_ASSIGN)
                printf("%s = %s\n", cur->result, cur->arg1);
            else
                printf("%s = %s %s %s\n", cur->result,
                       cur->arg1, tac_op_name(cur->op), cur->arg2);
            free(cur);
            removed++;
        } else {
            /* Append to new list */
            if (!new_head) new_head = cur;
            if (prev) prev->next = cur;
            cur->next = NULL;
            prev = cur;

            /* After unconditional jump / return, mark unreachable */
            if (cur->op == TAC_GOTO || cur->op == TAC_RETURN)
                unreachable = 1;
        }
        cur = nxt;
    }
    printf("[CtrlFlow] Unreachable instructions removed: %d\n", removed);
    return new_head ? new_head : head;
}
