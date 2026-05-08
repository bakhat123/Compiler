/*
 * Module 5 — Type Checker
 * Checks type compatibility; performs int->float promotion.
 */

#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

/* Returns the result type of combining two types with an operator.
 * If incompatible, prints an error and returns "error".
 * Implicit promotion: int op float -> float.
 */
const char *check_binary_op(const char *left_type, const char *right_type,
                             const char *op)
{
    if (strcmp(left_type, "error") == 0 || strcmp(right_type, "error") == 0)
        return "error";

    if (strcmp(left_type, right_type) == 0)
        return left_type;   /* same type — no cast needed */

    /* int + float or float + int -> float (promotion) */
    if ((strcmp(left_type, "int")   == 0 && strcmp(right_type, "float") == 0) ||
        (strcmp(left_type, "float") == 0 && strcmp(right_type, "int")   == 0)) {
        printf("[TypeCheck] Implicit promotion: int -> float for operator '%s'\n", op);
        return "float";
    }

    fprintf(stderr, "Type error: cannot apply '%s' to '%s' and '%s'\n",
            op, left_type, right_type);
    return "error";
}

/* Checks an assignment: lhs_type = rhs_type.
 * Allows int <- int, float <- float, float <- int (promotion).
 * Disallows int <- float (loss of precision — explicit cast required).
 */
int check_assignment(const char *lhs_type, const char *rhs_type,
                     const char *var_name)
{
    if (strcmp(lhs_type, rhs_type) == 0) return 1;

    if (strcmp(lhs_type, "float") == 0 && strcmp(rhs_type, "int") == 0) {
        printf("[TypeCheck] Implicit promotion: int -> float for assignment to '%s'\n",
               var_name);
        return 1;
    }

    fprintf(stderr, "Type error: cannot assign '%s' value to '%s' variable '%s' "
            "(explicit cast required)\n", rhs_type, lhs_type, var_name);
    return 0;
}

/* Returns type of an expression operand given its name (variable or literal) */
const char *type_of(const char *operand) {
    /* Numeric literal detection */
    int is_float = 0;
    const char *p = operand;
    if (*p == '-') p++;
    for (; *p; p++) {
        if (*p == '.') { is_float = 1; continue; }
        if (*p < '0' || *p > '9') { is_float = -1; break; }
    }
    if (is_float == 1) return "float";
    if (is_float == 0) return "int";

    /* Look up in symbol table */
    Symbol *s = lookup_var(operand);
    if (!s) {
        fprintf(stderr, "Type error: undeclared variable '%s'\n", operand);
        return "error";
    }
    return s->type;
}
