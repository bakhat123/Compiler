/*
 * Module 8 — LLVM Test Program 2
 * Float arithmetic + function calls: demonstrates fadd, fmul, call
 *
 * Compile:
 *   clang test2.c -S -emit-llvm -o test2.ll
 *   clang test2.c -S -emit-llvm -O3 -o test2_O3.ll
 *
 * Key IR constructs to observe:
 *   fadd / fmul / fdiv — floating-point arithmetic (vs integer add)
 *   call               — function call instruction
 *   phi                — in -O3, loop converted to phi-node form (SSA)
 *   uitofp / sitofp    — integer-to-float conversion
 */

#include <stdio.h>
#include <math.h>

/* Inlinable helper — -O3 likely inlines this */
static float square(float x) {
    return x * x;
}

/* Loop with float accumulation: watch for vectorization in -O3 */
float dot_product(float *a, float *b, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

/* Mixed int / float: shows sitofp (signed int to float pointer) */
float average(int *arr, int n) {
    int total = 0;
    for (int i = 0; i < n; i++)
        total += arr[i];
    return (float)total / (float)n;
}

int main(void) {
    printf("square(5.0)     = %.2f\n", square(5.0f));     /* 25.00 */

    float a[] = {1.0f, 2.0f, 3.0f};
    float b[] = {4.0f, 5.0f, 6.0f};
    printf("dot_product     = %.2f\n", dot_product(a, b, 3)); /* 32.00 */

    int  arr[] = {10, 20, 30, 40};
    printf("average         = %.2f\n", average(arr, 4));    /* 25.00 */

    printf("log(2.71828)    = %.5f\n", log(2.71828f));     /* ~1.0 */
    return 0;
}
