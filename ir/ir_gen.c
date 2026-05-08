/*
 * Module 6 — IR Generation (Three-Address Code)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ir_gen.h"

const char *tac_op_name(TACOp op) {
    switch (op) {
        case TAC_ASSIGN:      return "=";
        case TAC_ADD:         return "+";
        case TAC_SUB:         return "-";
        case TAC_MUL:         return "*";
        case TAC_DIV:         return "/";
        case TAC_POW:         return "^";
        case TAC_NEG:         return "neg";
        case TAC_LOG:         return "log";
        case TAC_EXP:         return "exp";
        case TAC_GOTO:        return "goto";
        case TAC_IF_TRUE:     return "if_true";
        case TAC_IF_FALSE:    return "if_false";
        case TAC_LABEL:       return "label";
        case TAC_PARAM:       return "param";
        case TAC_CALL:        return "call";
        case TAC_RETURN:      return "return";
        case TAC_ARRAY_LOAD:  return "arr_load";
        case TAC_ARRAY_STORE: return "arr_store";
        case TAC_LT:          return "<";
        case TAC_GT:          return ">";
        case TAC_LE:          return "<=";
        case TAC_GE:          return ">=";
        case TAC_EQ:          return "==";
        case TAC_NEQ:         return "!=";
        default:              return "??";
    }
}

TACInstr *make_instr(TACOp op, const char *result,
                     const char *arg1, const char *arg2)
{
    TACInstr *i = calloc(1, sizeof(TACInstr));
    i->op = op;
    if (result) strncpy(i->result, result, 63);
    if (arg1)   strncpy(i->arg1,   arg1,   63);
    if (arg2)   strncpy(i->arg2,   arg2,   63);
    i->next = NULL;
    return i;
}

void print_ir(TACInstr *head) {
    printf("\n========== Three-Address Code (IR) ==========\n");
    int n = 0;
    for (TACInstr *p = head; p; p = p->next, n++) {
        switch (p->op) {
            case TAC_LABEL:
                printf("%s:\n", p->result);
                break;
            case TAC_GOTO:
                printf("  [%3d]  goto %s\n", n, p->result);
                break;
            case TAC_IF_TRUE:
                printf("  [%3d]  if %s goto %s\n", n, p->arg1, p->result);
                break;
            case TAC_IF_FALSE:
                printf("  [%3d]  ifFalse %s goto %s\n", n, p->arg1, p->result);
                break;
            case TAC_RETURN:
                printf("  [%3d]  return %s\n", n, p->arg1);
                break;
            case TAC_PARAM:
                printf("  [%3d]  param %s\n", n, p->arg1);
                break;
            case TAC_CALL:
                printf("  [%3d]  %s = call %s, %s\n", n,
                       p->result, p->arg1, p->arg2);
                break;
            case TAC_ARRAY_LOAD:
                printf("  [%3d]  %s = %s[%s]\n", n,
                       p->result, p->arg1, p->arg2);
                break;
            case TAC_ARRAY_STORE:
                printf("  [%3d]  %s[%s] = %s\n", n,
                       p->result, p->arg1, p->arg2);
                break;
            case TAC_ASSIGN:
                printf("  [%3d]  %s = %s\n", n, p->result, p->arg1);
                break;
            case TAC_NEG:
                printf("  [%3d]  %s = neg %s\n", n, p->result, p->arg1);
                break;
            case TAC_LOG:
                printf("  [%3d]  %s = log(%s)\n", n, p->result, p->arg1);
                break;
            case TAC_EXP:
                printf("  [%3d]  %s = exp(%s)\n", n, p->result, p->arg1);
                break;
            default:
                printf("  [%3d]  %s = %s %s %s\n", n,
                       p->result, p->arg1, tac_op_name(p->op), p->arg2);
                break;
        }
    }
    printf("=============================================\n\n");
}

void free_ir(TACInstr *head) {
    while (head) {
        TACInstr *t = head->next;
        free(head);
        head = t;
    }
}
