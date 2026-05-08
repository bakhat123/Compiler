#ifndef IR_GEN_H
#define IR_GEN_H

typedef enum {
    TAC_ASSIGN,
    TAC_ADD, TAC_SUB, TAC_MUL, TAC_DIV,
    TAC_POW, TAC_NEG,
    TAC_LOG, TAC_EXP,
    TAC_GOTO,
    TAC_IF_TRUE, TAC_IF_FALSE,
    TAC_LABEL,
    TAC_PARAM, TAC_CALL, TAC_RETURN,
    TAC_ARRAY_LOAD, TAC_ARRAY_STORE,
    TAC_LT, TAC_GT, TAC_LE, TAC_GE, TAC_EQ, TAC_NEQ
} TACOp;

typedef struct TACInstr {
    TACOp  op;
    char   result[64];
    char   arg1[64];
    char   arg2[64];
    struct TACInstr *next;
} TACInstr;

/* Factory */
TACInstr *make_instr(TACOp op, const char *result,
                     const char *arg1, const char *arg2);

/* Print entire list */
void print_ir(TACInstr *head);

/* Free entire list */
void free_ir(TACInstr *head);

/* Op name (for printing) */
const char *tac_op_name(TACOp op);

#endif /* IR_GEN_H */
