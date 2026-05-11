# 🛠️ CS-346 Compiler Construction Project

> **NUST SEECS — BSCS-2023-AB**  
> A modular, multi-phase compiler built from scratch using **Flex**, **Bison**, **GCC**, and **LLVM/Clang**.

---

## 📋 Overview

This project implements a complete compiler pipeline for a mini-language, covering all major phases of compilation:

```
Source Code → Lexer → Parser → Semantic Analysis → IR Generation → Optimization → Output
```

| Phase | Module | Description |
|-------|--------|-------------|
| **Lexical Analysis** | Module 1 | Flex-based tokenizer with keyword, literal, and operator recognition |
| **Syntax Analysis** | Module 2 | Bison parsers — Postfix, Prefix, and Infix expression evaluation |
| **Extended Grammar** | Module 3 | Support for `log()`, `exp()`, exponentiation (`^`), and floats |
| **LL(1) Parsing** | Module 4 | FIRST/FOLLOW set computation and LL(1) parsing table construction |
| **Semantic Analysis** | Module 5 | Symbol table with nested scopes, type checking, and scope validation |
| **IR Generation** | Module 6 | Three-Address Code (TAC) generation from the AST |
| **Optimization** | Module 7 | 6 optimization passes over the TAC |
| **LLVM IR** | Module 8 | LLVM IR generation and comparison (unoptimized vs `-O3`) |

---

## 🏗️ Project Structure

```
compiler/
├── lexer/
│   ├── lexer.l                # Extended lexer (Bison-integrated)
│   ├── lexer_basic.l          # Basic lexer (Lab 05 baseline)
│   ├── lexer_standalone.l     # Standalone lexer (token printer)
│   ├── postfix_eval.c         # Stack-based postfix evaluator
│   └── input_sample.txt       # Sample input covering all token types
├── parser/
│   ├── parser.y               # Main grammar (full pipeline)
│   ├── postfix.y              # Postfix calculator
│   ├── prefix.y               # Prefix calculator
│   ├── infix.y                # Infix calculator (with precedence)
│   ├── extended.y             # Extended grammar (log/exp/^)
│   ├── calc_lex.l             # Shared calculator lexer
│   └── extended_lex.l         # Extended calculator lexer
├── first_follow/
│   ├── first.c                # FIRST set computation
│   ├── follow.c               # FOLLOW set computation
│   └── ll1_table.c            # LL(1) parsing table builder
├── semantic/
│   ├── symbol_table.h/.c      # Hash-based symbol table with scope stack
│   ├── type_checker.c         # Type checking with int→float promotion
│   ├── scope_checker.c        # Scope validation demo
│   ├── test_type_error.c      # Type error test suite (8 tests)
│   └── test_scope_error.c     # Scope error test suite (7 tests)
├── ir/
│   ├── ir_gen.h               # TAC instruction data structures
│   └── ir_gen.c               # TAC generator and printer
├── optimizer/
│   ├── optimizer.h/.c         # Optimization orchestrator + perf comparison
│   ├── const_fold.c           # Constant Folding
│   ├── const_prop.c           # Constant Propagation
│   ├── cse.c                  # Common Sub-expression Elimination
│   ├── dead_code.c            # Dead Code Elimination
│   ├── control_flow.c         # Unreachable Code Removal
│   ├── loop_opt.c             # Loop-Invariant Code Motion (LICM)
│   ├── basic_blocks.c         # Basic Block partitioning + CFG
│   └── benchmark.sh           # Performance benchmark script
├── llvm/
│   ├── test1.c                # LLVM test: loops, constants, dead stores
│   └── test2.c                # LLVM test: floats, function calls, inlining
├── tests/
│   └── test_program.src       # End-to-end test program
├── main.c                     # Pipeline driver with CLI flags
├── Makefile                   # Build system (15+ targets)
├── README.txt                 # Plain-text build instructions
└── README.md                  # This file
```

---

## ⚡ Quick Start

### Prerequisites

```bash
# Ubuntu / WSL
sudo apt-get install gcc flex bison make

# Optional (Module 8)
sudo apt-get install clang
```

### Build Everything

```bash
make all
```

### Run the Full Compiler Pipeline

```bash
./compiler < input.txt
```

---

## 🚀 Usage

### Full Pipeline

```bash
./compiler < input.txt                    # Full pipeline
./compiler --debug < input.txt            # With Bison debug trace
./compiler --dump-tokens < input.txt      # Print token stream only
./compiler --dump-tac < input.txt         # Print TAC and stop
./compiler --dump-opt < input.txt         # Show optimized IR only
```

### Individual Module Binaries

```bash
# Module 1 — Lexers
./lexer_only < lexer/input_sample.txt     # Extended token stream
./lexer_basic < lexer/input_sample.txt    # Basic lexer
echo "3 4 + 2 *" | ./postfix_eval        # Postfix evaluator

# Module 2 — Calculators
echo "3 4 +" | ./postfix_calc
echo "+ 3 4" | ./prefix_calc
echo "3 + 4 * 2" | ./infix_calc

# Module 3 — Extended Calculator
echo "2^3^2" | ./extended_calc            # => 512
echo "log(2.718281828)" | ./extended_calc  # => ~1.0

# Module 4 — FIRST/FOLLOW/LL(1)
./first_follow/first
./first_follow/follow
./first_follow/ll1

# Module 5 — Semantic Tests
./semantic_demo                           # Scope checker demo
./test_type                               # Type error tests (8/8)
./test_scope                              # Scope error tests (7/7)
```

### LLVM IR Generation (Module 8)

```bash
make llvm_ir    # Generates .ll files in llvm/
```

---

## 🔧 Optimization Passes (Module 7)

The optimizer runs 6 passes in sequence:

| # | Pass | Description |
|---|------|-------------|
| 1 | **Constant Propagation** | Replace variable uses with known constant values |
| 2 | **Constant Folding** | Evaluate `t1 = 3 + 4` → `t1 = 7` at compile time |
| 3 | **CSE** | Eliminate redundant sub-expression recomputation |
| 4 | **Dead Code Elimination** | Remove instructions whose results are never used |
| 5 | **Unreachable Code Removal** | Remove code after unconditional jumps/returns |
| 6 | **LICM** | Hoist loop-invariant computations out of loops |

Sample performance output:
```
  Run     Without Opt (ms)    With Opt (ms)
  1       0.0320              0.0280
  Avg     0.0334              0.0304
  Speedup: 1.10x
```

---

## 📐 Supported Grammar

```
program    → stmt_list
stmt       → type IDENT ;
           | IDENT = expr ;
           | if ( expr ) then begin stmt_list end
           | while ( expr ) begin stmt_list end
           | return expr ;

expr       → expr + term | expr - term | term
term       → term * factor | term / factor | factor
factor     → base ^ factor | base
base       → ( expr ) | -base | log( expr ) | exp( expr )
           | INT_LIT | FLOAT_LIT | IDENT
```

---

## 🧪 Test Results

| Test Suite | Result |
|------------|--------|
| Lexer token recognition | ✅ All token types identified |
| FIRST sets | ✅ Matches spec |
| FOLLOW sets | ✅ Matches spec |
| LL(1) parsing table | ✅ Correct with demo parse |
| Type checker | ✅ **8/8 tests pass** |
| Scope checker | ✅ **7/7 tests pass** |
| Full pipeline | ✅ Lex → Parse → Semantic → TAC → Optimize |
| End-to-end test | ✅ `tests/test_program.src` |

---

## 📚 Tools & References

- **Flex** — Fast Lexical Analyzer Generator
- **GNU Bison** — Parser Generator (LALR)
- **GCC** — GNU Compiler Collection
- **LLVM/Clang** — Compiler infrastructure for IR generation
- *Compilers: Principles, Techniques, and Tools* (Dragon Book)

---

## 📄 License

Academic project — CS-346 Compiler Construction, NUST SEECS.
