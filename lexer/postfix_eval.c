/*
 * Module 1 — Task 1.2: Postfix Expression Evaluator
 * Reads tokens (non-negative integers and operators +, -, *)
 * Evaluates a postfix expression using a stack.
 * Prints stack state (push/pop) at each step and the final result.
 *
 * Build: gcc -o postfix_eval postfix_eval.c
 * Usage: echo "3 4 + 2 *" | ./postfix_eval
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STACK 256

static int stack[MAX_STACK];
static int top = -1;

static void push(int val) {
    if (top >= MAX_STACK - 1) {
        fprintf(stderr, "Stack overflow!\n");
        exit(1);
    }
    stack[++top] = val;
    printf("  PUSH %d  =>  stack: [", val);
    for (int i = 0; i <= top; i++) {
        printf("%d", stack[i]);
        if (i < top) printf(", ");
    }
    printf("]\n");
}

static int pop(void) {
    if (top < 0) {
        fprintf(stderr, "Stack underflow!\n");
        exit(1);
    }
    int val = stack[top--];
    printf("  POP  %d  =>  stack: [", val);
    for (int i = 0; i <= top; i++) {
        printf("%d", stack[i]);
        if (i < top) printf(", ");
    }
    printf("]\n");
    return val;
}

int main(void) {
    printf("===================================\n");
    printf("  Postfix Expression Evaluator\n");
    printf("===================================\n\n");
    printf("Enter a postfix expression (e.g., 3 4 + 2 *):\n> ");

    char line[1024];
    if (!fgets(line, sizeof(line), stdin)) return 1;

    printf("\nEvaluating: %s\n", line);

    char *token = strtok(line, " \t\n");
    while (token) {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            /* Operand */
            push(atoi(token));
        } else if (strlen(token) == 1 && (token[0] == '+' || token[0] == '-' || token[0] == '*')) {
            /* Operator */
            printf("\n  Operator: %c\n", token[0]);
            int b = pop();
            int a = pop();
            int result;
            switch (token[0]) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                default:  result = 0; break;
            }
            printf("  Compute: %d %c %d = %d\n", a, token[0], b, result);
            push(result);
        } else {
            fprintf(stderr, "Unknown token: '%s'\n", token);
        }
        token = strtok(NULL, " \t\n");
    }

    if (top == 0) {
        printf("\n===================================\n");
        printf("  Final result: %d\n", stack[0]);
        printf("===================================\n");
    } else {
        fprintf(stderr, "\nError: stack not empty at end (has %d elements)\n", top + 1);
    }
    return 0;
}
