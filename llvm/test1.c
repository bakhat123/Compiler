/*
 * Module 8 — LLVM Test Program 1
 * Simple arithmetic: demonstrates alloca, load, store, add, ret
 *
 * Compile (unoptimised LLVM IR):
 *   clang test1.c -S -emit-llvm -o test1.ll
 *
 * Compile (optimised):
 *   clang test1.c -S -emit-llvm -O3 -o test1_O3.ll
 *
 * Key IR constructs to observe:
 *   alloca  — stack allocation for local variables
 *   store   — writing a value into memory
 *   load    — reading a value from memory
 *   add/sub — integer arithmetic
 *   ret     — return value from function
 */

#include <stdio.h>

/* Simple sum: observe how the loop is handled in IR */
int sum_to_n(int n) {
    int total = 0;
    for (int i = 1; i <= n; i++) {
        total += i;
    }
    return total;
}

/* Constant folding candidate: 3 + 4 should be folded by -O3 */
int constant_expr(void) {
    int a = 3;
    int b = 4;
    int c = a + b;    /* expect: c = 7 after folding */
    return c * 2;
}

/* Dead store candidate */
int dead_store(int x) {
    int y = x + 1;   /* y computed but never returned if overwritten */
    y = x * 2;       /* -O3 should eliminate the first y = x + 1 */
    return y;
}

int main(void) {
    printf("sum_to_n(10)    = %d\n", sum_to_n(10));      /* 55 */
    printf("constant_expr() = %d\n", constant_expr());   /* 14 */
    printf("dead_store(5)   = %d\n", dead_store(5));     /* 10 */
    return 0;
}
