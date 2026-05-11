# Compiler Construction Project — Agent Task Breakdown
> CS-346 | NUST SEECS | BSCS-2023-AB
> Stack: Flex, Bison, GCC, LLVM/Clang, C/C++

---

## Project Structure to Create First

```
project/
├── lexer/
├── parser/
├── first_follow/
├── semantic/
├── ir/
├── optimizer/
├── llvm/
├── Makefile
├── README.txt
└── report.pdf
```

**Task 0.1:** Create the full directory skeleton above with placeholder `.gitkeep` files in each folder.

**Task 0.2:** Create a top-level `Makefile` with targets: `all`, `lexer`, `parser`, `semantic`, `ir`, `optimizer`, and `clean`. Each target should compile its respective module. Wire them so `make all` builds everything in order.

**Task 0.3:** Create a `README.txt` describing how to build and run the project end-to-end.

---

## Module 1 — Lexical Analysis (Flex)

### Task 1.1 — Basic Lexer (Lab 05 Baseline)
Create `lexer/lexer_basic.l` that:
- Matches integer literals: `[0-9]+`
- Matches float literals: `[0-9]+\.[0-9]+`
- Matches identifiers: start with lowercase letter, digits allowed after: `[a-z][a-zA-Z0-9]*`
- Recognises keywords: `if`, `then`, `begin`, `end`, `procedure`, `function`
- Recognises operators: `+`, `-`, `*`, `/`
- Skips whitespace (spaces, tabs, newlines)
- Prints an error message for any unrecognised character

### Task 1.2 — Postfix Expression Evaluator
Create `lexer/postfix_eval.c` that:
- Reads tokens (non-negative integers and operators `+`, `-`, `*`)
- Evaluates a postfix expression using a stack
- Prints stack state (push/pop) at each step
- Prints the final result

### Task 1.3 — Extended Lexer (Full Pipeline)
Create `lexer/lexer.l` extending Task 1.1 to also handle:
- Keywords: `int`, `float`, `while`, `return`, `else` (in addition to existing ones)
- Identifiers: `[a-zA-Z_][a-zA-Z0-9_]*`
- Operators: `+`, `-`, `*`, `/`, `^`, `=`, `==`, `!=`
- Delimiters: `(`, `)`, `{`, `}`, `;`, `,`
- Single-line comments `// ...` → discard, no token
- Multi-line comments `/* ... */` → discard, no token
- Whitespace → discard, no token
- All tokens must return the correct token type for Bison integration

### Task 1.4 — Lexer Build & Test
- Add `lexer` target to Makefile: `flex lexer/lexer.l && gcc -o lexer/lexer lexer/lex.yy.c -lfl`
- Create `lexer/input_sample.txt` with sample input covering all token types
- Verify output prints each token with its type

---

## Module 2 — Syntax Analysis / Parsing (Bison)

### Task 2.1 — Postfix Parser
Create `parser/postfix.y` + `parser/postfix_lex.l`:
- Grammar: operands separated by operators in postfix order
- Evaluate using stack-based method
- Print result

### Task 2.2 — Prefix Parser
Create `parser/prefix.y` + reuse/adapt lexer:
- Grammar: operator precedes operands (recursive)
- Evaluate recursively
- Print result

### Task 2.3 — Infix Parser
Create `parser/infix.y`:
- Declare operator precedence and associativity:
  - `%left '+' '-'`
  - `%left '*' '/'`
  - `%right '^'`
- Support: `+`, `-`, `*`, `/`, `^`, `(`, `)`
- Build and print both a full parse tree and a compact AST
- Implement tree traversal: pre-order (prefix), in-order (infix), post-order (postfix)

### Task 2.4 — Parser Build & Test
- Add `parser` target to Makefile for all three variants
- Linux: `bison -d infix.y && flex calc_lex.l && gcc -o parser/PARSER parser/lex.yy.o parser/infix.tab.o -lfl -lm`
- Create sample expressions and verify each parser evaluates them correctly (at least 2 per variant)

---

## Module 3 — Extended Grammar (Functions & Exponentiation)

### Task 3.1 — Extend Grammar
Update `parser/infix.y` (or create `parser/extended.y`) to implement:
```
E → E + T | E – T | T
T → T * F | T / F | F
F → B ^ F | B          (right-associative)
B → ( E ) | id | num | log( E ) | exp( E )
```

### Task 3.2 — Float Handling
- Lexer must use `atof()` or `strtod()` to convert float string tokens to `double`
- All arithmetic must use `double` internally

### Task 3.3 — Error Handling & Debug Mode
- Any invalid input must print a descriptive error message (override `yyerror`)
- Add `#define YYDEBUG 1` and `yydebug = 1` support (enable via compile flag)

### Task 3.4 — Build & Test
- Link with `-lm` for `math.h`
- Use `%right '^'` for right-associativity
- Test with expressions: `log(2.718)`, `exp(1)`, `2^3^2` (should be 512), `log(exp(5))`

---

## Module 4 — First & Follow Sets / LL(1) Parsing Table

### Task 4.1 — FIRST Set Calculator
Create `first_follow/first_sets.c`:
- Read a grammar from input (productions in BNF-like format)
- Compute FIRST sets for all non-terminals
- Handle nullable non-terminals (those that can derive ε)
- Print results in a table

### Task 4.2 — FOLLOW Set Calculator
Create `first_follow/follow_sets.c`:
- Read a grammar and its FIRST sets
- Compute FOLLOW sets for all non-terminals
- Print results in a table

### Task 4.3 — LL(1) Parsing Table Builder
Create `first_follow/ll1_table.c`:
- Take FIRST and FOLLOW sets as input
- Construct the LL(1) parsing table
- Detect and report any FIRST/FOLLOW conflicts
- Print the table

### Task 4.4 — Validate Against Target Grammar
Verify your programs produce the correct output for:
```
E  → T E'
E' → + T E' | – T E' | ε
T  → F T'
T' → * F T' | / F T' | ε
F  → ( E ) | id | num
```
Expected output must match the FIRST/FOLLOW table in the spec exactly.

---

## Module 5 — Semantic Analysis (Type & Scope Checking)

### Task 5.1 — Symbol Table
Create `semantic/symbol_table.h` and `semantic/symbol_table.c`:
- Data structure: hash map or linked list of entries `{name, type, scope_level, value}`
- Functions: `insert(name, type, scope)`, `lookup(name)`, `enter_scope()`, `exit_scope()`
- Use a scope stack to support nested scopes
- Local scope takes precedence over global on lookup

### Task 5.2 — Type Checker
Create `semantic/type_checker.c`:
- Walk the AST produced by the parser
- Track declared type of each variable (`int`, `float`)
- Check operand types are compatible for each operation
- Report error with line number on type mismatch (e.g., `int + float` without cast)
- Implement implicit `int → float` promotion where applicable

### Task 5.3 — Scope Checker
Extend `semantic/type_checker.c` or create `semantic/scope_checker.c`:
- Report error when a variable is used before declaration
- Report error when a variable is declared twice in the same scope
- Handle global vs local scope correctly

### Task 5.4 — Test Cases
Create `semantic/test_type_error.c` and `semantic/test_scope_error.c` — small input programs that trigger each error. Verify correct error messages are printed.

---

## Module 6 — Intermediate Representation (TAC Generation)

### Task 6.1 — TAC Data Structures
Create `ir/tac.h`:
- Define `TACInstruction` struct with fields: `op`, `result`, `arg1`, `arg2`
- Define instruction types: `ASSIGN`, `ADD`, `SUB`, `MUL`, `DIV`, `GOTO`, `IF_GOTO`, `LABEL`, `PARAM`, `CALL`, `RETURN`, `ARRAY_LOAD`, `ARRAY_STORE`
- Temp variable generator: `new_temp()` returns `t1`, `t2`, etc.
- Label generator: `new_label()` returns `L1`, `L2`, etc.

### Task 6.2 — TAC Generator
Create `ir/tac_gen.c`:
- Walk the AST from Module 5
- Emit TAC for:
  - Binary ops: `t1 = y + z`
  - Unary ops: `t1 = -y`
  - Copy: `x = y`
  - Unconditional jump: `goto L`
  - Conditional jump: `if x < y goto L`
  - Function calls: `param x`, `call f, n`, `return x`
  - Array access: `x = a[i]`, `a[i] = x`

### Task 6.3 — TAC Printer
Create `ir/tac_print.c`:
- Print the full TAC instruction list in readable format
- Test with a non-trivial input program (e.g., a function with a loop and conditional)

### Task 6.4 — Integration
Wire Module 6 after Module 5 in the pipeline: source → lex → parse → semantic → TAC output.

---

## Module 7 — Code Optimisation

### Task 7.1 — Refactor TAC into Basic Blocks
Create `optimizer/basic_blocks.c`:
- Partition the TAC instruction list into basic blocks (split at labels and jumps)
- Build a Control Flow Graph (CFG) linking blocks

### Task 7.2 — Print IR Before Optimisation
Add a `--print-ir` flag (or function call) that dumps the unoptimised TAC before any pass runs. Label the output clearly as `=== IR BEFORE OPTIMISATION ===`.

### Task 7.3a — Constant Folding
Create `optimizer/const_fold.c`:
- Scan TAC for instructions where both operands are constants
- Evaluate at compile time and replace with a single assignment (e.g., `t1 = 3 + 4` → `t1 = 7`)
- Print before/after IR

### Task 7.3b — Constant Propagation
Create `optimizer/const_prop.c`:
- Track variables assigned a constant value
- Replace later uses of those variables with the constant directly
- Print before/after IR

### Task 7.3c — Common Sub-expression Elimination (CSE)
Create `optimizer/cse.c`:
- Detect expressions computed more than once with the same operands
- Replace redundant computations with a reference to the first result
- Print before/after IR

### Task 7.3d — Dead Code Elimination
Create `optimizer/dead_code.c`:
- Mark all variables that are ever used as an operand (liveness analysis)
- Remove assignment instructions whose result is never used
- Print before/after IR

### Task 7.4 — Control Flow / Unreachable Code
Create `optimizer/unreachable.c`:
- Walk the CFG and mark reachable blocks using BFS/DFS from the entry block
- Remove (or flag) any unreachable basic blocks
- Print before/after IR

### Task 7.5 — Loop Optimisation (LICM)
Create `optimizer/loop_opt.c`:
- Detect loops in the CFG (back edges)
- Identify loop-invariant computations (operands not modified inside the loop)
- Hoist invariant instructions to the loop pre-header block
- Print before/after IR

### Task 7.6 — Performance Comparison
Create `optimizer/benchmark.sh` (or a C timing harness):
- Run the unoptimised and optimised versions of a test program 5 times each
- Record execution time for each run
- Compute average and speedup ratio
- Output a table:
  ```
  | Run | Unoptimised (ms) | Optimised (ms) |
  |-----|-----------------|----------------|
  | 1   | ...             | ...            |
  | ... | ...             | ...            |
  | Avg | ...             | ...            |
  | Speedup: X.Xx         |                |
  ```

---

## Module 8 — LLVM IR Generation

### Task 8.1 — Verify LLVM Installation
- Run `clang --version` and capture output (screenshot for report)
- If not installed: `sudo apt-get install clang`

### Task 8.2 — Write Two C Test Programs
Create `llvm/test1.c` — a program with loops and conditionals (e.g., compute factorial).
Create `llvm/test2.c` — a program with arithmetic expressions and function calls.

### Task 8.3 — Generate Unoptimised LLVM IR
For each test program:
```bash
clang test1.c -S -emit-llvm -o test1_unopt.ll
clang test2.c -S -emit-llvm -o test2_unopt.ll
```

### Task 8.4 — Generate Optimised LLVM IR
```bash
clang test1.c -S -emit-llvm -O3 -o test1_opt.ll
clang test2.c -S -emit-llvm -O3 -o test2_opt.ll
```

### Task 8.5 — Annotate IR Files
In each `.ll` file (or in the report), add inline comments explaining:
- `alloca` — allocates stack space for a local variable
- `load` — reads a value from a memory address into a register
- `store` — writes a register value to a memory address
- `ret` — returns from a function
- `add` / `fadd` — integer / float addition
- `call` — calls a function

### Task 8.6 — Compare & Document Differences
Write a section in the report documenting:
- What instructions disappeared between unoptimised and `-O3` versions
- At least 3 specific optimisations visible (e.g., constant folding, dead store elimination, inlining)
- Line-count or instruction-count comparison

### Task 8.7 — Assemble and Disassemble (optional verification)
```bash
llvm-as test1_unopt.ll -o test1.bc
llvm-dis test1.bc -o test1_roundtrip.ll
```
Verify the round-trip file matches the original.

---

## Integration & Pipeline Wiring

### Task 9.1 — End-to-End Driver
Create `main.c` (or `compiler.c`) at project root:
- Accepts a source file as argument
- Calls each phase in order: Lexer → Parser → Semantic → IR Gen → Optimiser → print final IR
- Add a `--dump-tokens`, `--dump-ast`, `--dump-tac`, `--dump-opt` flag for each phase output

### Task 9.2 — Makefile Integration
Update root `Makefile` so that `make all` compiles every module and links them into a single `compiler` binary.

### Task 9.3 — End-to-End Test
Create `tests/test_program.src` — a non-trivial test program in your mini-language with:
- Variable declarations (int and float)
- Arithmetic expressions
- A while loop
- An if-else
- A function call
Run it through the full pipeline and verify each phase produces correct output.

---

## Report Checklist (fill in as you go)

| Section | Content Needed | Done? |
|---------|---------------|-------|
| Cover Page | Names, roll numbers, date | ☐ |
| Table of Contents | With page numbers | ☐ |
| Introduction | Compiler overview, pipeline diagram | ☐ |
| Module 1 | Flex rules explanation, token table, screenshot | ☐ |
| Module 2 | Grammar rules, screenshot of all 3 calculators | ☐ |
| Module 3 | Extended grammar listing, log/exp screenshot | ☐ |
| Module 4 | FIRST/FOLLOW table, LL(1) table, screenshot | ☐ |
| Module 5 | Symbol table design, error screenshots | ☐ |
| Module 6 | TAC format, sample TAC output screenshot | ☐ |
| Module 7 | Before/after IR per technique, perf table | ☐ |
| Module 8 | clang --version screenshot, annotated .ll, comparison | ☐ |
| Conclusion | Achievements and challenges | ☐ |
| References | Tools, textbooks, docs cited | ☐ |

---

## Quick Deliverables Checklist

- [ ] `lexer/lexer.l` — extended lexer
- [ ] `lexer/input_sample.txt` + token output screenshot
- [ ] `parser/postfix.y`, `parser/prefix.y`, `parser/infix.y`
- [ ] `parser/extended.y` — with log/exp/exponentiation
- [ ] `first_follow/first_sets.c`, `follow_sets.c`, `ll1_table.c`
- [ ] `semantic/symbol_table.c`, `type_checker.c`, `scope_checker.c`
- [ ] `ir/tac.h`, `tac_gen.c`, `tac_print.c`
- [ ] `optimizer/basic_blocks.c`, `const_fold.c`, `const_prop.c`, `cse.c`, `dead_code.c`, `unreachable.c`, `loop_opt.c`
- [ ] `optimizer/benchmark.sh` + performance table
- [ ] `llvm/test1.c`, `test2.c`, `*.ll` files (unopt + opt)
- [ ] `Makefile` (full build)
- [ ] `README.txt`
- [ ] `report.pdf` (min 10 pages, PDF format)
