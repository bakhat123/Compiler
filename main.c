/*
 * main.c — Pipeline Orchestrator (Task 9.1)
 * CS-346 Compiler Construction — NUST SEECS
 *
 * Runs the full compiler pipeline:
 *   Lexer → Parser → Semantic Analysis → IR Generation → Optimization
 *
 * Usage:
 *   ./compiler < input.txt
 *   ./compiler --debug       < input.txt
 *   ./compiler --dump-tokens < input.txt
 *   ./compiler --dump-tac    < input.txt
 *   ./compiler --dump-opt    < input.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic/symbol_table.h"
#include "ir/ir_gen.h"
#include "optimizer/optimizer.h"

/* Defined in parser (bison-generated) */
extern int  yyparse(void);

/* yydebug may or may not be defined by bison depending on YYDEBUG.
 * We provide a fallback definition here. */
#if !defined(YYDEBUG)
int yydebug = 0;
#else
extern int yydebug;
#endif

/* Defined in lexer (flex-generated) */
extern int  yylex(void);
extern char *yytext;
extern int  line_num;

/* IR list built by parser actions */
extern TACInstr *ir_head;

/* Flag variables */
static int flag_debug      = 0;
static int flag_dump_tokens = 0;
static int flag_dump_tac   = 0;
static int flag_dump_opt   = 0;

static void print_usage(void) {
    printf("Usage: ./compiler [options] < input_file\n");
    printf("Options:\n");
    printf("  --debug        Enable Bison debug trace\n");
    printf("  --dump-tokens  Print token stream and exit\n");
    printf("  --dump-tac     Print raw TAC (before optimization) and exit\n");
    printf("  --dump-opt     Print optimized IR only\n");
    printf("  --help         Show this help\n");
}

int main(int argc, char *argv[]) {
    /* Parse command-line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0)       flag_debug = 1;
        else if (strcmp(argv[i], "--dump-tokens") == 0) flag_dump_tokens = 1;
        else if (strcmp(argv[i], "--dump-tac") == 0)    flag_dump_tac = 1;
        else if (strcmp(argv[i], "--dump-opt") == 0)    flag_dump_opt = 1;
        else if (strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        }
    }

    printf("========================================\n");
    printf("  CS-346 Mini Compiler — NUST SEECS\n");
    printf("========================================\n\n");

    /* --- --dump-tokens: lex only, print tokens --- */
    if (flag_dump_tokens) {
        printf("[Pipeline] === Token Stream ===\n\n");
        printf("%-10s  %-14s  %s\n", "[Line]", "Token Code", "Lexeme");
        printf("%-10s  %-14s  %s\n", "------", "----------", "------");
        int tok;
        while ((tok = yylex()) != 0) {
            printf("[Line %2d]  %-14d  %s\n", line_num, tok, yytext);
        }
        printf("\n[Pipeline] Token dump complete.\n");
        return 0;
    }

    /* --- Module 5: init semantic state --- */
    init_symbol_table();

    /* --- Module 2/3: Parse (also does lexing + IR gen via actions) --- */
    printf("[Pipeline] Starting parse...\n");
    if (flag_debug) yydebug = 1;

    int parse_result = yyparse();

    if (parse_result != 0) {
        fprintf(stderr, "[Pipeline] Parse failed.\n");
        free_symbol_table();
        return 1;
    }
    printf("[Pipeline] Parse succeeded.\n\n");

    /* --- Module 6: Print raw IR --- */
    if (!flag_dump_opt) {
        printf("[Pipeline] === Module 6: Three-Address Code (before optimization) ===\n");
        print_ir(ir_head);
    }

    /* If --dump-tac, stop here */
    if (flag_dump_tac) {
        free_symbol_table();
        printf("[Pipeline] TAC dump complete.\n");
        return 0;
    }

    /* --- Module 5: Print symbol table --- */
    if (flag_debug) {
        printf("[Pipeline] === Module 5: Symbol Table ===\n");
        print_symbol_table();
    }

    /* --- Module 7: Optimization --- */
    printf("[Pipeline] === Module 7: Optimization Passes ===\n");
    TACInstr *optimized = run_all_optimizations(ir_head);

    printf("[Pipeline] === Module 7: Optimized IR ===\n");
    print_ir(optimized);

    /* --- Module 7 Task 6: Performance comparison --- */
    if (!flag_dump_opt)
        perf_compare(optimized);

    /* --- Cleanup --- */
    free_ir(optimized);
    free_symbol_table();

    printf("[Pipeline] Compilation complete.\n");
    return 0;
}