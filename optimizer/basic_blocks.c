/*
 * Module 7 — Task 7.1: Basic Blocks & Control Flow Graph
 * Partitions a TAC instruction list into basic blocks,
 * builds a CFG linking them, and prints the structure.
 *
 * A new basic block begins:
 *   1. At a LABEL instruction
 *   2. After a GOTO, IF_TRUE, IF_FALSE, or RETURN instruction
 *   3. At the very first instruction
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ir/ir_gen.h"

#define MAX_BLOCKS 128

typedef struct BasicBlock {
    int       id;
    TACInstr *first;          /* first instruction in this block */
    TACInstr *last;           /* last instruction in this block  */
    int       num_instrs;

    /* Successors (CFG edges) */
    int       succ[2];        /* at most 2 successors (fall-through + jump target) */
    int       num_succ;

    /* Label name (if block starts with a LABEL, else "") */
    char      label[64];
} BasicBlock;

static BasicBlock blocks[MAX_BLOCKS];
static int        num_blocks = 0;

/* Find block index by label name, or -1 */
static int find_block_by_label(const char *label) {
    for (int i = 0; i < num_blocks; i++)
        if (blocks[i].label[0] && strcmp(blocks[i].label, label) == 0)
            return i;
    return -1;
}

/* Returns 1 if instruction is a block terminator */
static int is_terminator(TACOp op) {
    return op == TAC_GOTO || op == TAC_IF_TRUE || op == TAC_IF_FALSE || op == TAC_RETURN;
}

/* Returns 1 if instruction is a block leader (start of a new block) */
static int is_leader(TACInstr *instr, TACInstr *prev) {
    if (!prev) return 1;                           /* first instruction */
    if (instr->op == TAC_LABEL) return 1;          /* target of a jump  */
    if (is_terminator(prev->op)) return 1;         /* after a jump      */
    return 0;
}

/* Phase 1: Partition into basic blocks */
void partition_into_blocks(TACInstr *head) {
    num_blocks = 0;
    TACInstr *prev = NULL;

    for (TACInstr *p = head; p; p = p->next) {
        if (is_leader(p, prev)) {
            /* Start a new block */
            if (num_blocks > 0)
                blocks[num_blocks - 1].last = prev;

            BasicBlock *bb = &blocks[num_blocks];
            bb->id         = num_blocks;
            bb->first      = p;
            bb->last       = p;
            bb->num_instrs = 0;
            bb->num_succ   = 0;
            bb->succ[0]    = bb->succ[1] = -1;
            bb->label[0]   = '\0';

            if (p->op == TAC_LABEL)
                strncpy(bb->label, p->result, 63);

            num_blocks++;
        }
        if (num_blocks > 0)
            blocks[num_blocks - 1].num_instrs++;

        prev = p;
    }
    if (num_blocks > 0)
        blocks[num_blocks - 1].last = prev;
}

/* Phase 2: Build CFG edges */
void build_cfg(void) {
    for (int i = 0; i < num_blocks; i++) {
        TACInstr *last = blocks[i].last;
        blocks[i].num_succ = 0;

        if (last->op == TAC_GOTO) {
            int tgt = find_block_by_label(last->result);
            if (tgt >= 0) blocks[i].succ[blocks[i].num_succ++] = tgt;
        }
        else if (last->op == TAC_IF_TRUE || last->op == TAC_IF_FALSE) {
            /* Jump target */
            int tgt = find_block_by_label(last->result);
            if (tgt >= 0) blocks[i].succ[blocks[i].num_succ++] = tgt;
            /* Fall-through */
            if (i + 1 < num_blocks)
                blocks[i].succ[blocks[i].num_succ++] = i + 1;
        }
        else if (last->op == TAC_RETURN) {
            /* No successor */
        }
        else {
            /* Fall-through to next block */
            if (i + 1 < num_blocks)
                blocks[i].succ[blocks[i].num_succ++] = i + 1;
        }
    }
}

/* Phase 3: Print blocks and CFG */
void print_basic_blocks(void) {
    printf("\n========================================\n");
    printf("  Basic Blocks & Control Flow Graph\n");
    printf("========================================\n\n");

    for (int i = 0; i < num_blocks; i++) {
        printf("Block B%d", blocks[i].id);
        if (blocks[i].label[0])
            printf(" [label: %s]", blocks[i].label);
        printf(" (%d instructions)\n", blocks[i].num_instrs);

        /* Print instructions in this block */
        for (TACInstr *p = blocks[i].first; ; p = p->next) {
            printf("    ");
            switch (p->op) {
                case TAC_LABEL:  printf("%s:", p->result); break;
                case TAC_GOTO:   printf("goto %s", p->result); break;
                case TAC_IF_TRUE:  printf("if %s goto %s", p->arg1, p->result); break;
                case TAC_IF_FALSE: printf("ifFalse %s goto %s", p->arg1, p->result); break;
                case TAC_RETURN: printf("return %s", p->arg1); break;
                case TAC_ASSIGN: printf("%s = %s", p->result, p->arg1); break;
                default:
                    printf("%s = %s %s %s", p->result, p->arg1,
                           tac_op_name(p->op), p->arg2);
                    break;
            }
            printf("\n");
            if (p == blocks[i].last) break;
        }

        /* Print successors */
        printf("  -> Successors: ");
        if (blocks[i].num_succ == 0) printf("(none — exit)");
        for (int j = 0; j < blocks[i].num_succ; j++) {
            printf("B%d", blocks[i].succ[j]);
            if (j < blocks[i].num_succ - 1) printf(", ");
        }
        printf("\n\n");
    }

    /* Print CFG summary */
    printf("--- CFG Adjacency ---\n");
    for (int i = 0; i < num_blocks; i++) {
        printf("  B%d ->", i);
        for (int j = 0; j < blocks[i].num_succ; j++)
            printf(" B%d", blocks[i].succ[j]);
        if (blocks[i].num_succ == 0) printf(" (exit)");
        printf("\n");
    }
    printf("========================================\n\n");
}

/* Convenience: do all 3 phases */
void analyze_basic_blocks(TACInstr *head) {
    partition_into_blocks(head);
    build_cfg();
    print_basic_blocks();
}
