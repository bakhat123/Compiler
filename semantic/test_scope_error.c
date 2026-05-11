/*
 * Module 5 — Task 5.4: Scope Error Test Cases
 * Small programs that trigger scope-related errors:
 *   - Use before declaration
 *   - Duplicate declaration in same scope
 *   - Shadowing across scopes
 *
 * Build: gcc -o test_scope_error test_scope_error.c symbol_table.c type_checker.c
 * Run:   ./test_scope_error
 */

#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

static int test_num  = 0;
static int pass_count = 0;

static void run_test(const char *desc) {
    test_num++;
    printf("\n--- Test %d: %s ---\n", test_num, desc);
}

int main(void) {
    printf("================================================\n");
    printf("  Module 5 — Scope Error Test Cases\n");
    printf("================================================\n");

    /* ========== Test 1: Use before declaration ========== */
    run_test("Use variable before declaration => error");
    {
        init_symbol_table();
        Symbol *s = lookup_var("undeclared");
        if (!s) {
            fprintf(stderr, "  Semantic error: undeclared variable 'undeclared'\n");
            printf("  PASS (error correctly detected)\n");
            pass_count++;
        } else {
            printf("  FAIL (variable should not exist)\n");
        }
        free_symbol_table();
    }

    /* ========== Test 2: Duplicate declaration in same scope ========== */
    run_test("Duplicate declaration in same scope => error");
    {
        init_symbol_table();
        int ok1 = declare_var("x", "int");
        int ok2 = declare_var("x", "float");  /* should fail */
        if (ok1 && !ok2) {
            printf("  PASS (duplicate correctly rejected)\n");
            pass_count++;
        } else {
            printf("  FAIL (ok1=%d, ok2=%d)\n", ok1, ok2);
        }
        free_symbol_table();
    }

    /* ========== Test 3: Same name in different scopes => OK ========== */
    run_test("Same name in different scopes => allowed (shadowing)");
    {
        init_symbol_table();
        declare_var("x", "int");              /* scope 0 */
        enter_scope();                        /* scope 1 */
        int ok = declare_var("x", "float");   /* shadow — should succeed */
        if (ok) {
            printf("  PASS (shadowing allowed)\n");
            pass_count++;
        } else {
            printf("  FAIL\n");
        }
        exit_scope();
        free_symbol_table();
    }

    /* ========== Test 4: Local shadows global ========== */
    run_test("Local variable shadows global variable");
    {
        init_symbol_table();
        declare_var("a", "int");          /* global scope 0 */
        enter_scope();                    /* scope 1 */
        declare_var("a", "float");        /* local shadow */

        Symbol *s = lookup_var("a");
        if (s && s->scope_level == 1 && strcmp(s->type, "float") == 0) {
            printf("  Lookup 'a' at scope 1: found type='%s' scope=%d\n",
                   s->type, s->scope_level);
            printf("  PASS (local correctly shadows global)\n");
            pass_count++;
        } else {
            printf("  FAIL\n");
        }
        exit_scope();
        free_symbol_table();
    }

    /* ========== Test 5: After exit_scope, global is visible ========== */
    run_test("After exit_scope, global variable visible again");
    {
        init_symbol_table();
        declare_var("b", "int");
        enter_scope();
        declare_var("b", "float");
        exit_scope();  /* local 'b' removed */

        Symbol *s = lookup_var("b");
        if (s && s->scope_level == 0 && strcmp(s->type, "int") == 0) {
            printf("  Lookup 'b' after exit_scope: type='%s' scope=%d\n",
                   s->type, s->scope_level);
            printf("  PASS (global restored)\n");
            pass_count++;
        } else {
            printf("  FAIL\n");
        }
        free_symbol_table();
    }

    /* ========== Test 6: Nested scopes (depth 3) ========== */
    run_test("Nested scopes depth 3 — innermost lookup");
    {
        init_symbol_table();
        declare_var("v", "int");     /* scope 0 */
        enter_scope();               /* scope 1 */
        declare_var("v", "float");   /* scope 1 */
        enter_scope();               /* scope 2 */

        /* 'v' should resolve to scope 1 (innermost that has it) */
        Symbol *s = lookup_var("v");
        if (s && s->scope_level == 1) {
            printf("  Lookup 'v' at scope 2: found at scope=%d type='%s'\n",
                   s->scope_level, s->type);
            printf("  PASS\n");
            pass_count++;
        } else {
            printf("  FAIL\n");
        }
        exit_scope();
        exit_scope();
        free_symbol_table();
    }

    /* ========== Test 7: Variable removed after exit_scope ========== */
    run_test("Variable declared in inner scope not visible after exit");
    {
        init_symbol_table();
        enter_scope();
        declare_var("temp", "int");
        exit_scope();

        Symbol *s = lookup_var("temp");
        if (!s) {
            printf("  Lookup 'temp' after exit_scope: not found\n");
            printf("  PASS (correctly removed)\n");
            pass_count++;
        } else {
            printf("  FAIL (should have been removed)\n");
        }
        free_symbol_table();
    }

    printf("\n================================================\n");
    printf("  Results: %d / %d tests passed\n", pass_count, test_num);
    printf("================================================\n");
    return 0;
}
