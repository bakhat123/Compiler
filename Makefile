# ============================================================
# Makefile — CS-346 Compiler Construction (NUST SEECS)
# Builds full pipeline + individual module test binaries
# ============================================================

CC      = gcc
FLEX    = flex
BISON   = bison
CFLAGS  = -Wall -g -I.
LDFLAGS = -lfl -lm

# ──────────────────────────────────────────────────────────
# Default target: full integrated compiler
# ──────────────────────────────────────────────────────────
all: compiler lexer_only lexer_basic postfix_eval \
     postfix_calc prefix_calc infix_calc extended_calc \
     first_follow_bin semantic_bin test_type test_scope llvm_tests

# ──────────────────────────────────────────────────────────
# Full pipeline binary
# ──────────────────────────────────────────────────────────
compiler: lex.yy.o parser.tab.o semantic/symbol_table.o semantic/type_checker.o \
          ir/ir_gen.o optimizer/const_fold.o optimizer/const_prop.o \
          optimizer/dead_code.o optimizer/cse.o optimizer/control_flow.o \
          optimizer/loop_opt.o optimizer/optimizer.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Generated sources
lex.yy.c: lexer/lexer.l parser.tab.h
	$(FLEX) -o lex.yy.c lexer/lexer.l

parser.tab.c parser.tab.h: parser/parser.y
	$(BISON) -d -o parser.tab.c parser/parser.y

# Object files
lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

semantic/symbol_table.o: semantic/symbol_table.c
	$(CC) $(CFLAGS) -c semantic/symbol_table.c -o $@

semantic/type_checker.o: semantic/type_checker.c
	$(CC) $(CFLAGS) -c semantic/type_checker.c -o $@

ir/ir_gen.o: ir/ir_gen.c
	$(CC) $(CFLAGS) -c ir/ir_gen.c -o $@

optimizer/const_fold.o: optimizer/const_fold.c
	$(CC) $(CFLAGS) -c optimizer/const_fold.c -o $@

optimizer/const_prop.o: optimizer/const_prop.c
	$(CC) $(CFLAGS) -c optimizer/const_prop.c -o $@

optimizer/dead_code.o: optimizer/dead_code.c
	$(CC) $(CFLAGS) -c optimizer/dead_code.c -o $@

optimizer/cse.o: optimizer/cse.c
	$(CC) $(CFLAGS) -c optimizer/cse.c -o $@

optimizer/control_flow.o: optimizer/control_flow.c
	$(CC) $(CFLAGS) -c optimizer/control_flow.c -o $@

optimizer/loop_opt.o: optimizer/loop_opt.c
	$(CC) $(CFLAGS) -c optimizer/loop_opt.c -o $@

optimizer/optimizer.o: optimizer/optimizer.c
	$(CC) $(CFLAGS) -c optimizer/optimizer.c -o $@

# ──────────────────────────────────────────────────────────
# Module 1: Standalone lexer (token stream only)
# ──────────────────────────────────────────────────────────
lexer_only: lexer/lexer_standalone.l
	@echo "Building standalone lexer..."
	$(FLEX) -o lexer/lex_standalone.yy.c lexer/lexer_standalone.l
	$(CC) $(CFLAGS) -o lexer_only lexer/lex_standalone.yy.c -lfl

# Module 1 Task 1.1: Basic Lexer
lexer_basic: lexer/lexer_basic.l
	@echo "Building basic lexer..."
	$(FLEX) -o lexer/lexer_basic.yy.c lexer/lexer_basic.l
	$(CC) $(CFLAGS) -o lexer_basic lexer/lexer_basic.yy.c -lfl

# Module 1 Task 1.2: Postfix Evaluator
postfix_eval: lexer/postfix_eval.c
	@echo "Building postfix evaluator..."
	$(CC) $(CFLAGS) -o postfix_eval lexer/postfix_eval.c

# ──────────────────────────────────────────────────────────
# Module 2: Three calculator parsers
# ──────────────────────────────────────────────────────────
postfix_calc: parser/postfix.y parser/calc_lex.l
	$(BISON) -d -o parser/postfix.tab.c parser/postfix.y
	$(FLEX) -o parser/calc_lex_post.yy.c parser/calc_lex.l
	$(CC) $(CFLAGS) -DPOSTFIX_CALC -Iparser -o postfix_calc parser/postfix.tab.c parser/calc_lex_post.yy.c $(LDFLAGS)

prefix_calc: parser/prefix.y parser/calc_lex.l
	$(BISON) -d -o parser/prefix.tab.c parser/prefix.y
	$(FLEX) -o parser/calc_lex_pre.yy.c parser/calc_lex.l
	$(CC) $(CFLAGS) -DPREFIX_CALC -Iparser -o prefix_calc parser/prefix.tab.c parser/calc_lex_pre.yy.c $(LDFLAGS)

infix_calc: parser/infix.y parser/calc_lex.l
	$(BISON) -d -o parser/infix.tab.c parser/infix.y
	$(FLEX) -o parser/calc_lex_inf.yy.c parser/calc_lex.l
	$(CC) $(CFLAGS) -Iparser -o infix_calc parser/infix.tab.c parser/calc_lex_inf.yy.c $(LDFLAGS)

# ──────────────────────────────────────────────────────────
# Module 3: Extended calculator (log, exp, ^)
# ──────────────────────────────────────────────────────────
extended_calc: parser/extended.y parser/extended_lex.l
	$(BISON) -d -o parser/extended.tab.c parser/extended.y
	$(FLEX) -o parser/extended.yy.c parser/extended_lex.l
	$(CC) $(CFLAGS) -Iparser -o extended_calc parser/extended.tab.c parser/extended.yy.c $(LDFLAGS)

# ──────────────────────────────────────────────────────────
# Module 4: First / Follow / LL(1) table (standalone)
# ──────────────────────────────────────────────────────────
first_follow_bin: first_follow/first.c first_follow/follow.c first_follow/ll1_table.c
	$(CC) $(CFLAGS) -o first_follow/first   first_follow/first.c
	$(CC) $(CFLAGS) -o first_follow/follow  first_follow/follow.c
	$(CC) $(CFLAGS) -o first_follow/ll1     first_follow/ll1_table.c

# ──────────────────────────────────────────────────────────
# Module 5: Semantic analysis standalone demo
# ──────────────────────────────────────────────────────────
semantic_bin: semantic/symbol_table.c semantic/type_checker.c semantic/scope_checker.c
	$(CC) $(CFLAGS) -o semantic_demo \
	    semantic/scope_checker.c semantic/symbol_table.c semantic/type_checker.c

# Module 5 Task 5.4: Test programs
test_type: semantic/test_type_error.c semantic/symbol_table.c semantic/type_checker.c
	$(CC) $(CFLAGS) -o test_type \
	    semantic/test_type_error.c semantic/symbol_table.c semantic/type_checker.c

test_scope: semantic/test_scope_error.c semantic/symbol_table.c semantic/type_checker.c
	$(CC) $(CFLAGS) -o test_scope \
	    semantic/test_scope_error.c semantic/symbol_table.c semantic/type_checker.c

# ──────────────────────────────────────────────────────────
# Module 8: LLVM test programs
# ──────────────────────────────────────────────────────────
llvm_tests:
	$(CC) $(CFLAGS) -o llvm/test1 llvm/test1.c -lm
	$(CC) $(CFLAGS) -o llvm/test2 llvm/test2.c -lm
	@echo "Run: clang llvm/test1.c -S -emit-llvm -o llvm/test1.ll"
	@echo "Run: clang llvm/test1.c -S -emit-llvm -O3 -o llvm/test1_O3.ll"
	@echo "Run: clang llvm/test2.c -S -emit-llvm -o llvm/test2.ll"
	@echo "Run: clang llvm/test2.c -S -emit-llvm -O3 -o llvm/test2_O3.ll"

# ──────────────────────────────────────────────────────────
# Generate LLVM IR (requires clang)
# ──────────────────────────────────────────────────────────
llvm_ir:
	clang llvm/test1.c -S -emit-llvm -o llvm/test1.ll
	clang llvm/test1.c -S -emit-llvm -O3 -o llvm/test1_O3.ll
	clang llvm/test2.c -S -emit-llvm -o llvm/test2.ll
	clang llvm/test2.c -S -emit-llvm -O3 -o llvm/test2_O3.ll
	@echo "LLVM IR files generated in llvm/"

# ──────────────────────────────────────────────────────────
# Debug build
# ──────────────────────────────────────────────────────────
debug: CFLAGS += -DDEBUG -DYYDEBUG=1
debug: compiler

# ──────────────────────────────────────────────────────────
# Clean
# ──────────────────────────────────────────────────────────
clean:
	rm -f *.o lex.yy.c parser.tab.c parser.tab.h
	rm -f parser/*.tab.c parser/*.tab.h parser/*.yy.c
	rm -f lexer/*.yy.c
	rm -f semantic/*.o ir/*.o optimizer/*.o
	rm -f compiler lexer_only lexer_basic postfix_eval
	rm -f postfix_calc prefix_calc infix_calc extended_calc
	rm -f first_follow/first first_follow/follow first_follow/ll1
	rm -f semantic_demo test_type test_scope
	rm -f llvm/test1 llvm/test2

.PHONY: all clean debug llvm_ir llvm_tests
