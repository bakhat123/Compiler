/*
 * main.c — Pipeline Orchestrator
 * CS-346 Compiler Construction — NUST SEECS
 *
 * Runs the full compiler pipeline:
 *   Lexer → Parser → Semantic Analysis → IR Generation → Optimization
 *
 * Usage:
 *   ./compiler < input.txt
 *   ./compiler --debug < input.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic/symbol_table.h"
#include "ir/ir_gen.h"
#include "optimizer/optimizer.h"

/* Defined in parser (bison-generated) */
extern int  yyparse(void);
extern int  yydebug;

/* IR list built by parser actions */
extern TACInstr *ir_head;

int main(int argc, char *argv[]) {
    int debug = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) debug = 1;
    }

    printf("========================================\n");
    printf("  CS-346 Mini Compiler — NUST SEECS\n");
    printf("========================================\n\n");

    /* --- Module 5: init semantic state --- */
    init_symbol_table();

    /* --- Module 2/3: Parse (also does lexing + IR gen via actions) --- */
    printf("[Pipeline] Starting parse...\n");
    if (debug) yydebug = 1;

    int parse_result = yyparse();

    if (parse_result != 0) {
        fprintf(stderr, "[Pipeline] Parse failed.\n");
        free_symbol_table();
        return 1;
    }
    printf("[Pipeline] Parse succeeded.\n\n");

    /* --- Module 6: Print raw IR --- */
    printf("[Pipeline] === Module 6: Three-Address Code (before optimization) ===\n");
    print_ir(ir_head);

    /* --- Module 5: Print symbol table --- */
    if (debug) {
        printf("[Pipeline] === Module 5: Symbol Table ===\n");
        print_symbol_table();
    }

    /* --- Module 7: Optimization --- */
    printf("[Pipeline] === Module 7: Optimization Passes ===\n");
    TACInstr *optimized = run_all_optimizations(ir_head);

    printf("[Pipeline] === Module 7: Optimized IR ===\n");
    print_ir(optimized);

    /* --- Module 7 Task 6: Performance comparison --- */
    perf_compare(optimized);

    /* --- Cleanup --- */
    free_ir(optimized);
    free_symbol_table();

    printf("[Pipeline] Compilation complete.\n");
    return 0;
}