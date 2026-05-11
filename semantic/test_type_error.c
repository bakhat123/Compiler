/*
 * Module 5 — Task 5.4: Type Error Test Cases
 * Small input programs that trigger type-checking errors.
 *
 * Build: gcc -o test_type_error test_type_error.c symbol_table.c type_checker.c
 * Run:   ./test_type_error
 */

#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

/* Defined in type_checker.c */
extern const char *check_binary_op(const char *left_type, const char *right_type,
                                    const char *op);
extern int check_assignment(const char *lhs_type, const char *rhs_type,
                             const char *var_name);
extern const char *type_of(const char *operand);

static int test_num  = 0;
static int pass_count = 0;

static void test(const char *desc, int expect_error) {
    test_num++;
    printf("\n--- Test %d: %s ---\n", test_num, desc);
}

static void check(const char *result, int was_error) {
    printf("  Result type: %s\n", result);
    if (was_error)
        pass_count++;
}

int main(void) {
    printf("================================================\n");
    printf("  Module 5 — Type Error Test Cases\n");
    printf("================================================\n");

    init_symbol_table();

    /* Declare some variables */
    declare_var("x", "int");
    declare_var("y", "float");
    declare_var("z", "int");

    /* Test 1: int + int => OK (int) */
    test("int + int => should succeed (result: int)", 0);
    {
        const char *r = check_binary_op("int", "int", "+");
        printf("  Result: %s  [Expected: int]\n", r);
        if (strcmp(r, "int") == 0) { printf("  PASS\n"); pass_count++; }
        else printf("  FAIL\n");
    }

    /* Test 2: float + float => OK (float) */
    test("float + float => should succeed (result: float)", 0);
    {
        const char *r = check_binary_op("float", "float", "*");
        printf("  Result: %s  [Expected: float]\n", r);
        if (strcmp(r, "float") == 0) { printf("  PASS\n"); pass_count++; }
        else printf("  FAIL\n");
    }

    /* Test 3: int + float => promotion (result: float) */
    test("int + float => implicit promotion to float", 0);
    {
        const char *r = check_binary_op("int", "float", "+");
        printf("  Result: %s  [Expected: float]\n", r);
        if (strcmp(r, "float") == 0) { printf("  PASS\n"); pass_count++; }
        else printf("  FAIL\n");
    }

    /* Test 4: Assignment float <- int => OK (promotion) */
    test("Assignment float <- int => promotion allowed", 0);
    {
        int ok = check_assignment("float", "int", "y");
        printf("  Assignment OK: %d  [Expected: 1]\n", ok);
        if (ok) { printf("  PASS\n"); pass_count++; }
        else printf("  FAIL\n");
    }

    /* Test 5: Assignment int <- float => ERROR (loss of precision) */
    test("Assignment int <- float => should produce error", 1);
    {
        int ok = check_assignment("int", "float", "x");
        printf("  Assignment OK: %d  [Expected: 0]\n", ok);
        if (!ok) { printf("  PASS (error correctly reported)\n"); pass_count++; }
        else printf("  FAIL\n");
    }

    /* Test 6: type_of() for integer literal */
    test("type_of(\"42\") => should return int", 0);
    {
        const char *t = type_of("42");
        printf("  Type: %s  [Expected: int]\n", t);
        if (strcmp(t, "int") == 0) { printf("  PASS\n"); pass_count++; }
        else printf("  FAIL\n");
    }

    /* Test 7: type_of() for float literal */
    test("type_of(\"3.14\") => should return float", 0);
    {
        const char *t = type_of("3.14");
        printf("  Type: %s  [Expected: float]\n", t);
        if (strcmp(t, "float") == 0) { printf("  PASS\n"); pass_count++; }
        else printf("  FAIL\n");
    }

    /* Test 8: type_of() for undeclared variable */
    test("type_of(\"undeclared_var\") => should return error", 1);
    {
        const char *t = type_of("undeclared_var");
        printf("  Type: %s  [Expected: error]\n", t);
        if (strcmp(t, "error") == 0) { printf("  PASS\n"); pass_count++; }
        else printf("  FAIL\n");
    }

    free_symbol_table();

    printf("\n================================================\n");
    printf("  Results: %d / %d tests passed\n", pass_count, test_num);
    printf("================================================\n");
    return 0;
}
