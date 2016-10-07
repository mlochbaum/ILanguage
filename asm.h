#include "x86_64.h"

typedef void* Asm;

/*
 * i o (input used; output given)
 *   + t: output type
 * +   n: number of inputs
 * + + i: input registers
 * + + o: output register
 * +   u: mask of unmodified registers (can include inputs)
 * + + l: length of assembly code
 *   + a: assembly code
 * 
 * The callee increments l and appends code to a.
 *
 * When used as an input, o can be NO_REG to indicate that the callee
 * can choose. i can be NO_REG to indicate that the callee can choose and
 * modify, or NO_REG_NM to indicate that the callee can choose but not
 * modify.
 */
typedef struct { T t; UC n; Reg* i; Reg o; RegM u; I l; Asm a; } AS;
typedef AS* A;

// Append code for f on the given types to a.
void apply_A(A a, V f, I n, T* x);


// Given a mask, find the first unmarked register.
Reg a_first_reg(RegM);

// Choose a->i and a->o (ensuring that neither is NO_REG or NO_REG_NM)
I choose_reg(A); // One input
I choose_regs(A); // Multiple inputs

// Append l bytes of aa to a
void a_append(A a, I l, Asm aa);

// Return a pointer to writable, executable memory
void *asm_mmap(size_t);

void asm_load(A a, T t, Reg o, Reg i);
void asm_write(A a, T t, Reg o, Reg i);
void a_RfromV(A a, Reg o, Reg i);
void a_RfromT(A a, T t, Reg o, Reg i);

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
 * The 2-part protect is used to protect those registers for the duration
 * of the function.
 * {
 *   PROTECT_1of2(u);
 *   // protected operations using registers u mixed with other operations
 *   // input and output registers in a will be disjoint from u
 *   PROTECT_2of2;
 * }
 *
 * The 3-part protect is used for an operation which happens before
 * anything else.
 * {
 *   PROTECT_1of3(u);
 *   // protected operation using registers u
 *   PROTECT_2of3;
 *   // rest of operations
 *   PROTECT_3of3;
 * }
 */

typedef struct { RegM p; Reg o; } Prot2State;
#define PROTECT_1of2(U) Prot2State prot2 = clear_regs(a, U)
#define PROTECT_2of2 pop_regs_2(a, prot2.p, prot2.o)

typedef struct {RegM p1; RegM m; RegM p2;} Prot3State;
#define PROTECT_1of3(U) Prot3State prot3 = clear_regs_3(a, U)
#define PROTECT_2of3 clear_regs_post(a, prot3.m, prot3.p1)
#define PROTECT_3of3 pop_regs(a, prot3.p2)

Prot2State clear_regs(A a, RegM u);
void pop_regs_2(A a, RegM pop, Reg o);
Prot3State clear_regs_3(A a, RegM u);
void clear_regs_post(A a, RegM mov, RegM pop);
void pop_regs(A a, RegM pop);
