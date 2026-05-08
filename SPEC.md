# SPEC.md — CS-346 Compiler Construction Project
## NUST SEECS | BSCS-2023-AB | Dr. Ahmed Rathore / Ms. Urooj Akmal

> **Purpose of this file:** Paste this entire SPEC.md at the start of every new chat session
> so the AI assistant has full context to continue building any module without losing state.
> Keep this file updated as modules are completed.

---

## 0. Project Overview

Build a **complete mini-compiler** from scratch using Flex + Bison + C/C++ + LLVM.
The compiler must tokenise, parse, type-check, generate TAC (Three-Address Code),
optimise it, and finally emit LLVM IR.

**Total Marks:** 106 (capped at 100; 6 marks buffer)
**Stack:** Flex, Bison, GCC/G++, LLVM+Clang, C/C++, Linux/Windows

---

## 1. Directory Structure (Required)

```
project/
├── lexer/          # Module 1 — .l file(s)
├── parser/         # Modules 2 & 3 — .y files
├── first_follow/   # Module 4 — C programs
├── semantic/       # Module 5 — type & scope checker
├── ir/             # Module 6 — IR generator
├── optimizer/      # Module 7 — optimisation passes
├── llvm/           # Module 8 — LLVM test files & .ll outputs
├── Makefile        # builds full project
├── README.txt      # build & run instructions
└── report.pdf      # comprehensive documentation
```

---

## 2. Language Being Compiled (Mini-Language Spec)

The source language supports:
- **Types:** `int`, `float`
- **Keywords:** `if`, `else`, `while`, `return`, `int`, `float`, `function`, `procedure`, `begin`, `end`, `then`
- **Identifiers:** `[a-zA-Z_][a-zA-Z0-9_]*`
- **Integer literals:** `[0-9]+`
- **Float literals:** `[0-9]+\.[0-9]+`
- **Operators:** `+`, `-`, `*`, `/`, `^`, `=`, `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Delimiters:** `(`, `)`, `{`, `}`, `;`, `,`
- **Comments:** `// ...` (single-line) and `/* ... */` (multi-line) — discard, no token
- **Whitespace:** spaces, tabs, newlines — discard

---

## 3. Module-by-Module Specification

---

### MODULE 1 — Lexical Analysis (Flex)
**File:** `lexer/lexer.l`
**Marks:** 8 (4 code + 4 docs)

#### 3.1.1 Baseline Requirements (Lab 05 Task 1)
- Match integer literals: `42`, `0`, `100` → token `INTEGER`
- Match float literals: `3.14`, `0.5` → token `FLOAT`
- Match identifiers: start with lowercase letter; digits in non-initial positions → token `IDENTIFIER`
- Keywords (Lab 05 set): `if`, `then`, `begin`, `end`, `procedure`, `function`
- Arithmetic operators: `+`, `-`, `*`, `/`
- Skip whitespace (spaces, tabs, newlines)
- Unrecognised character → print error message

#### 3.1.2 Baseline Requirements (Lab 05 Task 2 — Postfix Evaluator)
- Accept non-negative integer operands
- Support operators: `+`, `-`, `*`
- Evaluate postfix expression using a stack
- Display stack transitions at every step (push/pop)
- Print final result

#### 3.1.3 Extended Token Set (Full Project)

| Token Category   | Examples                     | Flex Pattern                              |
|------------------|------------------------------|-------------------------------------------|
| Keywords         | int, float, while, return, else | `"int"│"float"│"if"│"else"│"while"│"return"` |
| Identifiers      | x, myVar, result2            | `[a-zA-Z_][a-zA-Z0-9_]*`                 |
| Integer Literals | 42, 0, 100                   | `[0-9]+`                                  |
| Float Literals   | 3.14, 0.5, 2.0               | `[0-9]+\.[0-9]+`                          |
| Operators        | +, -, *, /, ^, =, ==, !=     | Single or double character tokens         |
| Delimiters       | ( ) { } ; ,                  | Single character tokens                   |
| Comments         | `//...` and `/*...*/`        | Discard — no token returned               |
| Whitespace       | spaces, tabs, newlines       | Discard — no token returned               |

#### 3.1.4 Build Commands
```bash
# Linux
flex lexer.l
gcc -o lexer lex.yy.c -lfl
./lexer < input.txt

# Windows
win_flex lexer.l
gcc -o lexer lex.yy.c
lexer.exe < input.txt
```

#### 3.1.5 Deliverables
- [x] `lexer/lexer.l` — implements all token categories
- [x] `lexer/input.txt` — sample input file
- [x] Screenshot of token output (for report)
- [x] Integration with Bison parser (Module 2)

---

### MODULE 2 — Syntax Analysis / Parsing (Bison)
**File:** `parser/postfix.y`, `parser/prefix.y`, `parser/infix.y`
**Marks:** 8 (4 code + 4 docs)

#### 3.2.1 Three Calculator Variants (all required)

| Notation | Example | Description                                    |
|----------|---------|------------------------------------------------|
| Postfix  | `4 8 +` | Operator follows operands; stack-based eval    |
| Prefix   | `+ 4 8` | Operator precedes operands; recursive eval     |
| Infix    | `4 + 8` | Standard notation; precedence/associativity    |

#### 3.2.2 Grammar Concepts Required
- Declare operator precedence with `%left`, `%right`, `%nonassoc`
- Support: `+`, `-`, `*`, `/`, `^`, `(`, `)`
- Parse trees show full derivation
- Syntax/AST trees omit redundant nodes
- Tree traversal: pre-order (prefix), in-order (infix), post-order (postfix)

#### 3.2.3 Build Commands (Infix)
```bash
# Linux
bison -d infix.y
flex calc_lex.l
gcc -o PARSER lex.yy.o infix.tab.o -lfl -lm

# Windows
win_bison -d infix.y
win_flex calc_lex.l
gcc -o PARSER infix.tab.c lex.yy.c -lm
```

#### 3.2.4 Deliverables
- [x] Three `.y` files (or one combined) for postfix, prefix, infix parsers
- [x] Screenshot of each parser evaluating ≥2 sample expressions
- [x] Integration with Module 1 lexer

---

### MODULE 3 — Extended Grammar (Functions, Exponentiation, Math)
**File:** `parser/extended.y`
**Marks:** 8 (4 code + 4 docs)

#### 3.3.1 Extended Grammar (Exact — must implement)
```
E  →  E + T  |  E – T  |  T
T  →  T * F  |  T / F  |  F
F  →  B ^ F  |  B          (right-associative exponentiation)
B  →  ( E )  |  id  |  num  |  log( E )  |  exp( E )
```

#### 3.3.2 Requirements

| Requirement    | Detail                                                     |
|----------------|------------------------------------------------------------|
| Input          | Arithmetic expressions (integer and floating-point)        |
| Output         | Evaluated numerical result                                 |
| Float Handling | Lexer must use `atof()` or `strtod()` for conversion       |
| Whitespace     | Lexer discards spaces, tabs, newlines                      |
| Error Handling | Any invalid input → descriptive error message              |
| Debug Mode     | `#define YYDEBUG 1` and `yydebug = 1`                      |

- Use `%right` for `^` to enforce right-associativity
- Link with `-lm` for `math.h` (`log()`, `exp()`, `pow()`)

#### 3.3.3 Deliverables
- [x] Updated `.y` with exponentiation + `log()`/`exp()` support
- [x] Screenshot showing evaluation of `log()` and `exp()` expressions

---

### MODULE 4 — First & Follow Sets / LL(1) Parsing Table
**Files:** `first_follow/first.c`, `first_follow/follow.c`, `first_follow/ll1_table.c`
**Marks:** 8 (4 code + 4 docs)

#### 3.4.1 Target Grammar
```
E   →  T E'
E'  →  + T E'  |  – T E'  |  ε
T   →  F T'
T'  →  * F T'  |  / F T'  |  ε
F   →  ( E )  |  id  |  num
```

#### 3.4.2 Expected FIRST and FOLLOW Sets

| Non-terminal | Nullable | FIRST            | FOLLOW              |
|--------------|----------|------------------|---------------------|
| E            | No       | `{ (, id, num }` | `{ $, ) }`          |
| E'           | Yes      | `{ +, –, ε }`    | `{ $, ) }`          |
| T            | No       | `{ (, id, num }` | `{ $, ), +, – }`    |
| T'           | Yes      | `{ *, /, ε }`    | `{ $, ), +, – }`    |
| F            | No       | `{ (, id, num }` | `{ *, /, $, ), +, – }` |

#### 3.4.3 LL(1) Compatibility Requirements
- Grammar must be unambiguous
- No left recursion (eliminate before computing sets)
- No FIRST/FOLLOW conflicts for any non-terminal

#### 3.4.4 Required Programs
1. **`first.c`** — computes FIRST sets from any grammar input
2. **`follow.c`** — computes FOLLOW sets from any grammar input
3. **`ll1_table.c`** — constructs LL(1) parsing table from FIRST and FOLLOW sets

#### 3.4.5 Deliverables
- [x] Three C programs as above
- [x] Correct FIRST/FOLLOW output matching table above
- [x] LL(1) parsing table printed to stdout

---

### MODULE 5 — Semantic Analysis (Type & Scope Checking)
**Files:** `semantic/symbol_table.h`, `semantic/symbol_table.c`, `semantic/type_checker.c`, `semantic/scope_checker.c`
**Marks:** 8 (4 code + 4 docs)

#### 3.5.1 Task A — Type Checking and Type Casting
- Track declared type of every variable (`int`, `float`, etc.)
- Check operations are performed on compatible types
- Report type error if incompatible types mixed without explicit cast
- Perform implicit/explicit type casting (`int → float` promotion)

#### 3.5.2 Task B — Scope Checking
- Symbol table with **separate entries** for global and local scopes
- When same identifier exists in both scopes → local scope takes precedence
- Report error when variable used before declaration

#### 3.5.3 Data Structures
```c
// Recommended: scope stack
typedef struct Symbol {
    char name[64];
    char type[16];   // "int" or "float"
    int  scope_level;
    struct Symbol *next;
} Symbol;

typedef struct ScopeStack {
    Symbol **scopes;
    int top;
} ScopeStack;
```

#### 3.5.4 Deliverables
- [x] Source files: symbol table, type checker, scope checker
- [x] Screenshot showing correct error for **type mismatch**
- [x] Screenshot showing correct error for **undeclared variable**

---

### MODULE 6 — Intermediate Representation (TAC Generation)
**Files:** `ir/ir_gen.h`, `ir/ir_gen.c`
**Marks:** 10 (6 code + 4 docs)

#### 3.6.1 Required TAC Forms
```
Binary operation:    x = y op z
Unary operation:     x = op y
Copy statement:      x = y
Unconditional jump:  goto L
Conditional jump:    if x relop y goto L
Function call:       param x  /  call f, n  /  return x
Array access:        x = a[i]  and  a[i] = x
```

#### 3.6.2 TAC Data Structure
```c
typedef enum { TAC_ASSIGN, TAC_ADD, TAC_SUB, TAC_MUL, TAC_DIV,
               TAC_GOTO, TAC_IF, TAC_PARAM, TAC_CALL, TAC_RETURN,
               TAC_ARRAY_LOAD, TAC_ARRAY_STORE, TAC_LABEL } TACOp;

typedef struct TACInstr {
    TACOp op;
    char result[64];
    char arg1[64];
    char arg2[64];
    struct TACInstr *next;
} TACInstr;
```

#### 3.6.3 Deliverables
- [x] IR generator source integrated with Module 5 output
- [x] Printed TAC listing for ≥1 non-trivial input program

---

### MODULE 7 — Code Optimisation
**Files:** `optimizer/optimizer.h`, `optimizer/const_fold.c`, `optimizer/const_prop.c`,
`optimizer/cse.c`, `optimizer/dead_code.c`, `optimizer/loop_opt.c`, `optimizer/control_flow.c`
**Marks:** 40 total (Tasks 1–6)

#### Mark Breakdown
| Task | Description | Marks |
|------|-------------|-------|
| Task 1 | Refactoring (clean modular code) | 5 |
| Task 2 | Print IR before optimisation | 5 |
| Task 3 | Implement ≥3 of 5 optimisations | 15 |
| Task 4 | Control flow / unreachable code removal | 7 |
| Task 5 | Loop optimisation (LICM recommended) | 8 |
| Task 6 | Performance comparison table | 5 |

#### 3.7.1 Required Optimisation Techniques (implement ≥3 of 5)

| Technique | Description |
|-----------|-------------|
| **Constant Folding** | Evaluate constant expressions at compile time (`3+4 → 7`) |
| **Constant Propagation** | Replace variables holding a known constant with that constant |
| **Common Sub-expression Elimination (CSE)** | Reuse previously computed results instead of recalculating |
| **Dead Code Elimination** | Remove assignments whose values are never subsequently used |
| **Loop Optimisation** | Loop-invariant code motion (LICM) and/or loop fusion |

#### 3.7.2 Task 4 — Control Flow
- Detect and remove **unreachable code blocks**
- A block is unreachable if no path from entry can reach it

#### 3.7.3 Task 5 — Loop Optimisation
- Implement **LICM (Loop-Invariant Code Motion)**: move computations that don't change inside the loop to before the loop
- Optionally implement loop fusion

#### 3.7.4 Task 6 — Performance Comparison
- Measure execution time of program compiled **without** optimisation vs **with** optimisation
- Minimum **5 runs** for each; compute average
- Present as table with speedup ratio column:

| Run | Without Opt (ms) | With Opt (ms) |
|-----|-----------------|---------------|
| 1   | ...             | ...           |
| 2   | ...             | ...           |
| ... | ...             | ...           |
| Avg | ...             | ...           |
| **Speedup** | — | **X.Xx** |

#### 3.7.5 Deliverables
- [x] Optimiser source files per technique above
- [x] Before/after IR output for each technique (clearly labelled)
- [x] Performance comparison table in report

---

### MODULE 8 — LLVM IR Generation
**Files:** `llvm/test1.c`, `llvm/test2.c`, `llvm/test1.ll`, `llvm/test1_O3.ll`, `llvm/test2.ll`, `llvm/test2_O3.ll`
**Marks:** 11 (5 install+gen + 6 analysis)

#### 3.8.1 Installation
```bash
clang --version              # verify Clang is installed
sudo apt-get install clang   # if not installed
```

#### 3.8.2 Key IR Generation Commands

| Command | Output / Purpose |
|---------|-----------------|
| `clang -o test test.c` | Compile to native executable |
| `clang test.c -S -emit-llvm` | Generate **unoptimised** LLVM IR (`.ll` file) |
| `clang test.c -S -emit-llvm -O3` | Generate **optimised** LLVM IR |
| `llvm-as test.ll` | Assemble IR text to bitcode (`.bc`) |
| `llvm-dis test.bc` | Disassemble bitcode back to text |

#### 3.8.3 Required Analysis
- Generate LLVM IR (unoptimised) for **both** test programs; annotate key constructs
- Generate LLVM IR with `-O3` for both programs
- Compare and document differences (what did LLVM eliminate or transform?)
- Identify **≥3 specific optimisations** visible in `-O3` output
- Explain semantics of: `alloca`, `load`, `store`, `ret`, `add`, `call`

#### 3.8.4 Key LLVM IR Instructions to Annotate

| Instruction | Semantics |
|-------------|-----------|
| `alloca`    | Allocates memory on the stack for a local variable |
| `load`      | Loads a value from a memory address into a virtual register |
| `store`     | Stores a value from a register into a memory address |
| `ret`       | Returns a value from the current function |
| `add`       | Integer addition of two operands |
| `call`      | Calls a function with given arguments |

#### 3.8.5 Deliverables
- [x] Screenshot of `clang --version` (verified installation)
- [x] `.ll` files (unoptimised + `-O3`) for both test programs
- [x] Annotated comparison in report (≥3 optimisations identified)

---

## 4. Integration & Pipeline Requirements (Overall)

- All modules must **compile together as a single project** — no separate standalone binaries
- Compiler pipeline must execute **end-to-end** on a test program
- Clean, modular code: **separate files per phase**
- **Makefile** or build script that compiles full project
- Single entry point (e.g. `main.c`) that calls each phase in sequence

### Suggested Makefile Structure
```makefile
CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -Wall -g

all: compiler

compiler: lex.yy.o parser.tab.o semantic.o ir_gen.o optimizer.o
    $(CC) $(CFLAGS) -o compiler $^ -lfl -lm

lex.yy.c: lexer/lexer.l
    $(FLEX) -o lex.yy.c lexer/lexer.l

parser.tab.c parser.tab.h: parser/infix.y
    $(BISON) -d -o parser.tab.c parser/infix.y

clean:
    rm -f *.o lex.yy.c parser.tab.c parser.tab.h compiler
```

---

## 5. Report Structure (Module 9)

**Format:** PDF | **Min pages:** 10 | **Font:** Arial/TNR 11–12pt

| # | Section | Required Content |
|---|---------|-----------------|
| 1 | Cover Page | University, course, project title, group names & roll numbers, date |
| 2 | Table of Contents | With page numbers |
| 3 | Introduction | Overview of compiler and pipeline being implemented |
| 4 | Module 1 – Lexer | Flex rules explanation, token table, screenshot of token output |
| 5 | Module 2 – Parser | Grammar rules, screenshot of each calculator variant |
| 6 | Module 3 – Extended Grammar | Extended grammar listing, screenshot of `log()`/`exp()` evaluation |
| 7 | Module 4 – First & Follow | FIRST/FOLLOW sets table, LL(1) table, screenshot |
| 8 | Module 5 – Semantic | Symbol table design, type/scope error examples, screenshot |
| 9 | Module 6 – IR Generation | TAC format description, sample TAC output screenshot |
| 10 | Module 7 – Optimisation | Before/after IR per technique, performance comparison table |
| 11 | Module 8 – LLVM | Install screenshot, annotated `.ll` files, opt vs non-opt comparison |
| 12 | Conclusion | Summary of achieved goals + challenges encountered |
| 13 | References | Cited tools, textbooks, documentation |

### Documentation Marks Rubric (per module)
| Criterion | Marks |
|-----------|-------|
| Screenshot of correct output | 1 mark per module |
| Clear explanation of implementation decisions | 0.5 marks per module |
| Before/after IR for each optimisation (Task 3) | Included in Task 3 |
| Performance comparison table (Task 6) | Included in Task 6 |
| LLVM IR annotated with explanations | Included in Module 8 |

---

## 6. Complete Marks Checklist

| Module / Component | Max | Status |
|--------------------|-----|--------|
| Module 1: Lexical Analysis (Flex) | 8 | ⬜ |
| Module 2: Parsing – Postfix / Prefix / Infix | 8 | ⬜ |
| Module 3: Extended Grammar (Functions) | 8 | ⬜ |
| Module 4: First & Follow / LL(1) Table | 8 | ⬜ |
| Module 5: Type & Scope Checking | 8 | ⬜ |
| Module 6: IR Generation (TAC) | 10 | ✅ |
| Module 7 – Task 1: Refactoring | 5 | ✅ |
| Module 7 – Task 2: IR Before Opt. (Print) | 5 | ✅ |
| Module 7 – Task 3: Optimisation (≥3 techs) | 15 | ✅ |
| Module 7 – Task 4: Control Flow / Unreachable | 7 | ✅ |
| Module 7 – Task 5: Loop Optimisation | 8 | ✅ |
| Module 7 – Task 6: Performance Comparison | 5 | ✅ |
| Module 8: LLVM Installation & IR Generation | 5 | ✅ |
| Module 8: Optimised vs Non-optimised Analysis | 6 | ✅ |
| **TOTAL** | **106*** | |

*Capped at 100. 6 marks of buffer.*

---

## 7. Key Design Decisions to Document (for viva)

1. **Why LALR(1) and not LL(1)?** — Bison generates LALR(1); LL(1) is computed separately in Module 4 for pedagogical comparison.
2. **Symbol table implementation** — Hash table or linked list? Scope stack for nested scopes. Justify choice.
3. **TAC representation** — Linked list of instruction structs; why this over an array?
4. **Which 3 optimisations chosen for Task 3** — Justify selection (constant folding + propagation + dead code are the easiest trio with highest marks impact).
5. **LICM correctness** — How do you detect a loop-invariant expression? What is the loop's "pre-header"?
6. **LLVM comparison** — Specific instructions eliminated by `-O3` and why.

---

## 8. Common Pitfalls / Notes

- Flex: keywords must be matched **before** the identifier rule, or they will be tokenised as identifiers.
- Bison: shift/reduce conflicts in infix grammar → solve with `%left`/`%right` precedence declarations.
- For `^` (exponentiation), use `%right` because `2^3^2 = 2^(3^2) = 512`, not `(2^3)^2 = 64`.
- Always link with `-lm` for `log()`, `exp()`, `pow()`.
- When computing FOLLOW sets, `$` (end-of-input) is always in FOLLOW of the start symbol.
- Scope checking: use a **stack of hash maps**; push on entering a block `{`, pop on leaving `}`.
- IR temp variable naming convention: `t1`, `t2`, ... (global counter, never reset).
- For performance comparison (Task 6), use `clock()` or `gettimeofday()` in C, not `time` shell command.
- LLVM `.ll` files must be included in submission ZIP, not just screenshots.
- Report must be **PDF** — Word file will not be accepted.

---

## 9. How to Use This SPEC in New Chat Sessions

**At the start of each new chat, paste:**
```
Here is the full project spec. I am working on [MODULE X].
Here is the code I have so far for the relevant files: [paste code]
Please continue building [specific task].
```

**Update the checklist (Section 6)** as each module is completed by changing ⬜ to ✅.

**Paste only the relevant module section** if context is tight — each module section is self-contained.
