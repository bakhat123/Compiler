===============================================================
  CS-346 Compiler Construction Project
  NUST SEECS — BSCS-2023-AB
  Stack: Flex, Bison, GCC, LLVM/Clang, C/C++
===============================================================

1. PREREQUISITES
   - GCC (or compatible C compiler)
   - GNU Flex  (lexer generator)
   - GNU Bison (parser generator)
   - GNU Make
   - Clang/LLVM (for Module 8 only)
   - Math library (libm)
   - Flex library (libfl)

   On Ubuntu/WSL:
     sudo apt-get install gcc flex bison make clang

   On MSYS2/MinGW:
     pacman -S mingw-w64-x86_64-gcc flex bison make

2. BUILD (all modules)
     make all

   Individual modules:
     make lexer_only       # Standalone lexer (token stream)
     make postfix_calc     # Postfix calculator
     make prefix_calc      # Prefix calculator
     make infix_calc       # Infix calculator
     make extended_calc    # Extended calculator (log/exp/^)
     make first_follow_bin # FIRST/FOLLOW/LL(1) programs
     make semantic_bin     # Semantic demo (scope checker)
     make compiler         # Full pipeline binary
     make llvm_tests       # LLVM test programs

   Debug build (enables Bison trace):
     make debug

   Clean:
     make clean

3. RUN THE FULL PIPELINE
     ./compiler < input.txt
     ./compiler --debug < input.txt          # with Bison debug trace
     ./compiler --dump-tokens < input.txt    # show token stream
     ./compiler --dump-tac < input.txt       # show raw TAC only
     ./compiler --dump-opt < input.txt       # show optimised IR only

4. RUN INDIVIDUAL MODULES

   Module 1 — Lexer:
     ./lexer_only < lexer/input_sample.txt

   Module 2 — Calculators:
     echo "3 4 +" | ./postfix_calc
     echo "+ 3 4" | ./prefix_calc
     echo "3 + 4" | ./infix_calc

   Module 3 — Extended Calculator:
     echo "2^3^2" | ./extended_calc
     echo "log(2.718)" | ./extended_calc

   Module 4 — FIRST/FOLLOW/LL(1):
     ./first_follow/first
     ./first_follow/follow
     ./first_follow/ll1

   Module 5 — Semantic Tests:
     ./semantic_demo
     make test_type && ./test_type
     make test_scope && ./test_scope

   Module 8 — LLVM IR Generation:
     make llvm_ir
     # Produces llvm/test1.ll, test1_O3.ll, test2.ll, test2_O3.ll

5. END-TO-END TEST
     ./compiler < tests/test_program.src

6. PROJECT STRUCTURE

   project/
   ├── lexer/          Flex lexer sources + test input
   ├── parser/         Bison parsers (postfix, prefix, infix, extended)
   ├── first_follow/   FIRST set, FOLLOW set, LL(1) table programs
   ├── semantic/       Symbol table, type checker, scope checker
   ├── ir/             Three-address code (TAC) generation
   ├── optimizer/      Optimization passes (const fold/prop, CSE, DCE, etc.)
   ├── llvm/           LLVM IR test programs
   ├── tests/          End-to-end test programs
   ├── main.c          Pipeline driver
   ├── Makefile         Top-level build system
   └── README.txt      This file

7. MODULE SUMMARY

   Module 1: Lexical Analysis (Flex)
   Module 2: Syntax Analysis — Postfix/Prefix/Infix parsers (Bison)
   Module 3: Extended Grammar — log/exp/exponentiation
   Module 4: FIRST/FOLLOW Sets & LL(1) Parsing Table
   Module 5: Semantic Analysis — symbol table, type/scope checking
   Module 6: Intermediate Representation — Three-Address Code generation
   Module 7: Code Optimization — const fold, const prop, CSE, DCE,
             unreachable code removal, loop-invariant code motion
   Module 8: LLVM IR Generation & Comparison

===============================================================
