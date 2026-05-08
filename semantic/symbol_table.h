#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_NAME  64
#define MAX_TYPE  16
#define MAX_SCOPE 32

typedef struct Symbol {
    char name[MAX_NAME];
    char type[MAX_TYPE];   /* "int" or "float" */
    int  scope_level;
    struct Symbol *next;
} Symbol;

/* Public API */
void   init_symbol_table(void);
void   enter_scope(void);
void   exit_scope(void);
int    declare_var(const char *name, const char *type);
Symbol *lookup_var(const char *name);
Symbol *lookup_local(const char *name);
void   print_symbol_table(void);
void   free_symbol_table(void);

extern int current_scope;

#endif /* SYMBOL_TABLE_H */
