/*
 * Module 5 — Scope Checker
 * Demonstrates scope rules with a standalone test driver.
 * In the full pipeline this is integrated via the symbol table.
 */

#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

/* Scope-checking test: simulates a program with nested scopes */
static void run_scope_test(void) {
    printf("=== Scope Checker Test ===\n\n");

    init_symbol_table();

    /* Global scope (level 0) */
    printf("-- Declaring globals --\n");
    declare_var("x", "int");
    declare_var("y", "float");

    /* Lookup before entering inner scope */
    Symbol *s = lookup_var("x");
    printf("Lookup 'x' at scope 0: found '%s' type='%s'\n\n",
           s ? s->name : "(not found)", s ? s->type : "?");

    /* Enter inner scope (e.g., inside a function body) */
    enter_scope();
    declare_var("x", "float");   /* shadows global x */
    declare_var("z", "int");

    s = lookup_var("x");
    printf("Lookup 'x' at scope 1: found scope=%d type='%s' (local shadows global)\n\n",
           s ? s->scope_level : -1, s ? s->type : "?");

    print_symbol_table();

    /* Try using undeclared variable */
    s = lookup_var("w");
    if (!s)
        fprintf(stderr, "Semantic error: undeclared variable 'w'\n\n");

    /* Exit inner scope; 'x' (local) and 'z' are removed */
    exit_scope();

    s = lookup_var("x");
    printf("Lookup 'x' after exit_scope: scope=%d type='%s' (back to global)\n\n",
           s ? s->scope_level : -1, s ? s->type : "?");

    /* Type mismatch demonstration */
    printf("-- Type Mismatch Test --\n");
    extern int check_assignment(const char*, const char*, const char*);
    check_assignment("int", "float", "x");   /* should error */
    check_assignment("float", "int", "y");   /* should promote */

    free_symbol_table();
    printf("\n=== Scope Checker Done ===\n");
}

int main(void) {
    run_scope_test();
    return 0;
}
