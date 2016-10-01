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

// Append given a UC* literal
#define ASM_RAW(A, OP) \
  do { UC aa[] = OP; a_append(A, sizeof(aa), aa); } while(0)
// Append an instruction
#define ASM(A, OP,O,I) ASM_RAW(A, OP(O,I))
#define ASM3(A, OP,O,I,E) ASM_RAW(A, OP(O,I,E))
