/*
 * Module 5 — Symbol Table (Scope Stack via linked list)
 * Supports nested scopes; local variables shadow globals.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

int current_scope = 0;

/* Single linked list; entries sorted by scope_level */
static Symbol *table_head = NULL;

void init_symbol_table(void) {
    table_head    = NULL;
    current_scope = 0;
}

void enter_scope(void) {
    current_scope++;
    printf("[Scope] Entering scope level %d\n", current_scope);
}

void exit_scope(void) {
    /* Remove all symbols at current scope level */
    Symbol **pp = &table_head;
    while (*pp) {
        if ((*pp)->scope_level == current_scope) {
            Symbol *tmp = *pp;
            *pp = tmp->next;
            free(tmp);
        } else {
            pp = &(*pp)->next;
        }
    }
    printf("[Scope] Exiting scope level %d\n", current_scope);
    current_scope--;
}

int declare_var(const char *name, const char *type) {
    /* Check for duplicate in current scope */
    Symbol *s = table_head;
    while (s) {
        if (s->scope_level == current_scope && strcmp(s->name, name) == 0) {
            fprintf(stderr, "Semantic error: '%s' already declared in scope %d\n",
                    name, current_scope);
            return 0;
        }
        s = s->next;
    }
    Symbol *ns = malloc(sizeof(Symbol));
    strncpy(ns->name,  name,  MAX_NAME  - 1);
    strncpy(ns->type,  type,  MAX_TYPE  - 1);
    ns->name[MAX_NAME-1] = ns->type[MAX_TYPE-1] = '\0';
    ns->scope_level = current_scope;
    ns->next = table_head;
    table_head = ns;
    return 1;
}

/* Lookup: most-recent (highest scope) wins */
Symbol *lookup_var(const char *name) {
    Symbol *best = NULL;
    Symbol *s    = table_head;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (!best || s->scope_level > best->scope_level)
                best = s;
        }
        s = s->next;
    }
    return best;
}

Symbol *lookup_local(const char *name) {
    Symbol *s = table_head;
    while (s) {
        if (s->scope_level == current_scope && strcmp(s->name, name) == 0)
            return s;
        s = s->next;
    }
    return NULL;
}

void print_symbol_table(void) {
    printf("\n--- Symbol Table ---\n");
    printf("%-20s %-8s %-8s\n", "Name", "Type", "Scope");
    printf("%-20s %-8s %-8s\n", "----", "----", "-----");
    Symbol *s = table_head;
    while (s) {
        printf("%-20s %-8s %-8d\n", s->name, s->type, s->scope_level);
        s = s->next;
    }
    printf("--------------------\n\n");
}

void free_symbol_table(void) {
    Symbol *s = table_head;
    while (s) {
        Symbol *t = s->next;
        free(s);
        s = t;
    }
    table_head = NULL;
}
