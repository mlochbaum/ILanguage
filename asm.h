#include "x86_64.h"

typedef void* Asm;

#define ASM_MIN_ITER 8 // Required number of iterations before compiling

/*
 * r i o (used in R; input used; output given)
 * x + + i: input registers
 *   + + o: output register
 *   +   u: mask of unmodified registers (can include inputs)
 * x + + l: length of assembly code
 * x   + a: assembly code
 * + +   ar: number of variable and constant registers for each function
 * + +   ts: output type for each function
 * + + + lc: index in constant registers
 *   + + cr: registers used for constants
 * + +   cv: values stored in constant registers
 * 
 * The callee increments l and appends code to a.
 *
 * When used as an input, o can be NO_REG to indicate that the callee
 * can choose. i can be NO_REG to indicate that the callee can choose and
 * modify, or NO_REG_NM to indicate that the callee can choose but not
 * modify.
 */
typedef struct { Reg* i; Reg o; RegM u; U l; Asm a;
                 T* ts; U (*ar)[2]; U lc; Reg* cr; Z* cv; } AS;
typedef AS* A;
#define MIN_ARR 8 // Minimum length for cr and cv

void init_A(A a);
// Append register allocation information to a->ar and a->cr
T apply_R(A a, V f, U n, T* x);
// Append code for f on the given types to a.
void apply_A(A a, V f, U n, T* x);
// Also return the output type (*a->ts after application)
T apply_A_t(A a, V f, U n, T* x);

T apply_R_full(A,V,U,T*);
RegM start_A(A,U,RegM);
void apply_A_full(A,V,U,T*);
P finish_A(A,RegM); // Returns a function pointer.
void asm_unmap(A,P);

void request_regs(A, U);
void request_cv(A, Z);
Reg use_cr(A);


// Array of argument registers
extern Reg reg_args[];

// Given a mask, get an unmarked register.
Reg get_reg(RegM);
// Get an register not marked in *u|v and mark it in *u.
Reg get_reg_mark(RegM *u, RegM v);
// Given a mask, count the number of marked registers.
Reg num_marked(RegM u);

// Get the mask of all input registers.
RegM input_mask(A,U);

// Choose a->i and a->o (ensuring that neither is NO_REG or NO_REG_NM)
U choose_reg(A); // One input
U choose_regs(A); // Two inputs
U choose_regn(A,U); // Multiple inputs

// Append l bytes of aa to a
void a_append(A a, U l, Asm aa);
// Copy values back from AX to A
#define UPDATE_A_(A,AX) A->lc=AX.lc; A->ar=AX.ar; A->ts=AX.ts
#define UPDATE_A(A,AX) A->l=AX.l; A->a=AX.a; UPDATE_A_(A,AX)

void asm_jump(A a, UC cond, U label);

void asm_load(A a, T t, Reg o, Reg i);
void asm_write(A a, T t, Reg o, Reg i);
void asm_load_at(A a, T t, Reg o, Reg i, Reg e);
void asm_write_at(A a, T t, Reg o, Reg i, Reg e);
void a_RfromV(A a, Reg o, Reg i);
void a_RfromT(A a, T t, Reg o, Reg i);

void a_del(A a, T t, Reg i);

// Mark input *i as not writeable
void protect_input(Reg *i, RegM *u);

// Append given a UC* literal
#ifndef ASM_RAW
#define ASM_RAW(A, OP) \
  do { UC aa[] = OP; a_append(A, sizeof(aa), aa); } while(0)
#endif
// Append an instruction
#define ASM(A, OP,O,I) ASM_RAW(A, OP(O,I))
#define ASM3(A, OP,O,I,E) ASM_RAW(A, OP(O,I,E))


/*
 * Operations for clearing registers
 * For use when something (e.g. idiv or a function call) requires the use
 * of some specific registers.
 * The assembly object is assumed to be named a.
 *
 * PROTECT(u) protects those registers until UNPROTECT is called.
 * PROTECT_START(u) may be used instead if the registers are not used
 * after a->o is used.
 * {
 *   PROTECT(u);
 *   // protected operations using registers u mixed with other operations
 *   // input and output registers in a will be disjoint from u
 *   UNPROTECT;
 * }
 */

typedef struct { RegM p; Reg o; } ProtState;
#define PROTECT(U) ProtState prot2 = clear_regs(a,n, U,0)
#define UNPROTECT pop_regs_o(a, prot2.p, prot2.o)

#define PROTECT_START(U) ProtState prot2 = clear_regs(a,n, U,1)

ProtState clear_regs(A a, U n, RegM u, C start);
void pop_regs_o(A a, RegM pop, Reg o);
RegM push_regs(A a, RegM u);
void pop_regs(A a, RegM pop);
