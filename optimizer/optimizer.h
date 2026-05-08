#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "../ir/ir_gen.h"

/* Module 7 — Optimization passes
 * Each pass takes the IR list head and returns (possibly new) head.
 */

/* Task 3 — Three optimization techniques */
TACInstr *const_fold(TACInstr *head);       /* Constant Folding      */
TACInstr *const_prop(TACInstr *head);       /* Constant Propagation  */
TACInstr *dead_code_elim(TACInstr *head);   /* Dead Code Elimination */
TACInstr *cse_pass(TACInstr *head);         /* Common Sub-expression */

/* Task 4 — Control flow / unreachable code removal */
TACInstr *remove_unreachable(TACInstr *head);

/* Task 5 — Loop optimization (LICM) */
TACInstr *loop_invariant_motion(TACInstr *head);

/* Run all passes in sequence */
TACInstr *run_all_optimizations(TACInstr *head);

/* Performance helpers */
void perf_compare(TACInstr *head);

#endif /* OPTIMIZER_H */
